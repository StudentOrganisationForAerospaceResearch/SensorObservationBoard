/**
  ******************************************************************************
  * File Name          : ThermocoupleTask.cpp
  *
  * Description        : This file contains constants and functions designed to
  *                      obtain accurate temperature readings from
  *                      the thermocouples on the plumbing bay board. A
  *                      thread task is included that will wait for update and transmit
  *                      requests for reading and updating the temperature fields.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "ThermocoupleTask.hpp"
#include "main.h"
#include "DebugTask.hpp"
#include "Task.hpp"
#include "TelemetryMessage.hpp"
#include "SOBProtocolTask.hpp"

/* Macros --------------------------------------------------------------------*/

/* Structs -------------------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/
#define ERROR_TEMPERATURE_VALUE 9999
#define TEMPERATURE_OFFSET 6.0 //in degrees Celsius

/* Values should not be modified, non-const due to HAL and C++ strictness) ---*/
constexpr int CMD_TIMEOUT = 150;

/* Variables -----------------------------------------------------------------*/

/* Prototypes ----------------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/**
 * @brief Default constructor
 */
ThermocoupleTask::ThermocoupleTask() : Task(THERMOCOUPLE_TASK_QUEUE_DEPTH_OBJS)
{
	//Data is stored locally in object not using MALOC
}

/**
 * @brief Creates a task for the FreeRTOS Scheduler
 */
void ThermocoupleTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize Thermocouple task twice");

    // Start the task
    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)ThermocoupleTask::RunTask,
            (const char*)"ThermocoupleTask",
            (uint16_t)THERMOCOUPLE_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)THERMOCOUPLE_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    //Ensure creation succeeded
    SOAR_ASSERT(rtValue == pdPASS, "ThermocoupleTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief ThermocoupleTask run loop
 * @param pvParams Currently unused task context
 */
void ThermocoupleTask::Run(void * pvParams)
{
    while (1) {
        Command cm;

        //Wait forever for a command
        qEvtQueue->ReceiveWait(cm);

        //Process the command
        HandleCommand(cm);
    }
}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void ThermocoupleTask::HandleCommand(Command& cm)
{
    //Switch for the GLOBAL_COMMAND
    switch (cm.GetCommand()) {
    case REQUEST_COMMAND: {
        HandleRequestCommand(cm.GetTaskCommand()); //Sends task specific request command to task request handler
        break;
    }
    case TASK_SPECIFIC_COMMAND: {
        break; //No task specific commands need
    }
    default:
        SOAR_PRINT("ThermocoupleTask - Received Unsupported Command {%d}\n", cm.GetCommand());
        break;
    }
    //No matter what we happens, we must reset allocated data
    cm.Reset();
}

/**
 * @brief Handles a Request Command
 * @param taskCommand The command to handle
 */
void ThermocoupleTask::HandleRequestCommand(uint16_t taskCommand)
{
    //Switch for task specific command within DATA_COMMAND
    switch (taskCommand) {
    case THERMOCOUPLE_REQUEST_NEW_SAMPLE: //Sample TC and store in class fields
    	SampleThermocouple();
        break;
    case THERMOCOUPLE_REQUEST_TRANSMIT: //Sending data to PI
        TransmitProtocolThermoData();
        break;
    case THERMOCOUPLE_REQUEST_DEBUG: //Output TC data
        ThermocoupleDebugPrint();
        break;
    default:
        SOAR_PRINT("UARTTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
        break;
    }
}

/**
 * @brief Transmits a protocol barometer data sample
 */
void ThermocoupleTask::TransmitProtocolThermoData()
{
    SOAR_PRINT("Thermocouple Task Transmit...\n");
    //ThermocoupleDebugPrint();

    Proto::TelemetryMessage msg;
    msg.set_source(Proto::Node::NODE_SOB);
    msg.set_target(Proto::Node::NODE_RCU);
    msg.set_message_id((uint32_t)Proto::MessageID::MSG_TELEMETRY);
    Proto::SOBTemp tempData;
	tempData.set_tc1_temp(temperature1);
	tempData.set_tc2_temp(temperature2);
	msg.set_tempsob(tempData);

    EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE> writeBuffer;
    msg.serialize(writeBuffer);

    // Send the thermocouple data
    SOBProtocolTask::SendProtobufMessage(writeBuffer, Proto::MessageID::MSG_TELEMETRY);
}


/**
 * @brief display any error messages and the temperature
 */
void ThermocoupleTask::ThermocoupleDebugPrint()
{
	//thermo 1 print
	if(dataBuffer1[1] & 0x01)
	{
		SOAR_PRINT("There is an Error with Thermocouple 1 \n\n");
	}
	else
	{
		SOAR_PRINT("Thermocouple 1 is reading %d.%d C \n\n" , temperature1/100, temperature1%100);
	}

	//thermo 2 print
	if(dataBuffer2[1] & 0x01)
	{
		SOAR_PRINT("There is an Error with Thermocouple 2 \n\n");
	}
	else
	{
		SOAR_PRINT("Thermocouple 2 is reading %d.%d C \n\n", temperature2/100, temperature2%100);
	}
}

/**
 * @brief This method converts the thermocouple data buffer information to readable a temperature
 * takes the array containing temperature data, returns a temperature value
 */
int16_t ThermocoupleTask::ExtractTempurature(uint8_t temperatureData[])
{
	int temperature=0;

	if(!(temperatureData[1] & 0x01)){ //if there is not an error with TC1 compute temperature
		if((temperatureData[0]&(0x80))>>7==1)  // Negative Temperature (check if sign bit is 1)
		{
			//or together the 2 parts of the temperature after multiplying to the correct position
			//if there is no error read all bits in buffer[0] and [1] will be temperature values
			temperature = (temperatureData[0] << 6) | (temperatureData[1] >> 2);

			//since the temperature is negative we need to do 2's compliment calculations
			temperature^=0b11111111111111; //first XOR all 14 bits of temp data including the sign bit (bits 31-18)
									 //this will flip the bits
			temperature+=0b1; //then add 1

			//here we first divide by 4 as the lower 2 bits are decimal bits, then because of this we scale
			//the number to fit in an int_16t so it includes the decimal digits and we
			//also correct the temperature by 3.2C which was the approximate error recorded
			return (int16_t)((((double)-temperature / 4)-TEMPERATURE_OFFSET)*100);
		}
		else  // Positive Temperature
		{
			//or together the 2 parts of the temperature after multiplying to the correct position
			//if there is no error read all bits in buffer[0] and [1] will be temperature values
			temperature = (temperatureData[0] << 6) | (temperatureData[1] >> 2);

			//here we scale the number to fit in an int_16t so it includes the decimal digits and we
			//also correct the temperature by 3.2C which was the approximate error recorded
			return (int16_t)((((double)temperature / 4)-TEMPERATURE_OFFSET)*100);
		}
	}
	else
	{
		return (int16_t)ERROR_TEMPERATURE_VALUE; //there is an error detected with TC1
	}
}

/**
 * @brief This method receives the voltage reading through spi from the thermocouple readings
 */
void ThermocoupleTask::SampleThermocouple()
{
	/*DATA FROM MAX31855KASA+T ------------------------------------------------------

	32 bits Memory Map

		D31-D18 : Thermocoupler Temperature Data

			D31 : Sign bit

			D30-D18 : Temperature Value (2's complement) from 2^10 to 2^-2

		D17 : Reserved Bit

		D16 : Fault (if high shows fault is detected, more specific fault messages at D2 - D0)

		D15-D4 :  Internal Temperature Data (reference junction temperature)

			D15 : Sign bit

			D14-D4 : Temperature Value (2's complement) from 2^6 to 2^-4

		D3 : Reserved

		D2-D0 : Fault Detection Bits

			D2 : SCV Fault (displays high if TC shorts to Vcc)

			D1 : SCG Fault (displays high if TC shorts to GND)

			D0 : Thermocouple has no Connection (displays high)

	*///------------------------------------------------------------------------------

	SOAR_PRINT("\n-- Sample Thermocouple Data --\n");

	uint8_t tempDataBuffer5[5] = {0};
	//See Above bit mem-map

	//Read ---------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);


    //Read From Thermocouple 1 first
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); //begin read with CS pin low
	HAL_Delay(10);
	HAL_SPI_Receive(SystemHandles::SPI_Thermocouple, tempDataBuffer5, 5, 1000); //Fill the data buffer with data from TC1
	HAL_Delay(10);
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //end read with setting CS pin to high again

	for(int i = 0; i<4; i++){
		dataBuffer1[i] = tempDataBuffer5[i+1];
	}

	temperature1 = ExtractTempurature(dataBuffer1);

	//reset the buffer array to all 0s
	memset(tempDataBuffer5, 0, 5);

	//Read ---------------------------------------------------------------------------------------
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);

	//Read From Thermocouple 1 first
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET); //begin read with CS pin low
	HAL_Delay(10);
	HAL_SPI_Receive(SystemHandles::SPI_Thermocouple, tempDataBuffer5, 5, 1000); //Fill the data buffer with data from TC1
	HAL_Delay(10);
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET); //end read with setting CS pin to high again

	for(int i = 0; i<4; i++){
		dataBuffer2[i] = tempDataBuffer5[i+1];
	}

	temperature2 = ExtractTempurature(dataBuffer2);
}
