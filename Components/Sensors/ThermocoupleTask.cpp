/**
 ******************************************************************************
 * File Name          : ThermocoupleTask.cpp
 * Description        : Primary thermocouple task, default task for the system.
 ******************************************************************************
*/
#include <Sensors/Inc/ThermocoupleTask.hpp>
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "main.h"
#include "DebugTask.hpp"
#include "Task.hpp"


/**
 * @brief Constructor for FlightTask
 */
ThermocoupleTask::ThermocoupleTask() : Task(THERMOCOUPLE_TASK_QUEUE_DEPTH_OBJS)
{
}

/**
 * @brief Initialize the FlightTask
 */
void ThermocoupleTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize thermocouple task twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)ThermocoupleTask::RunTask,
            (const char*)"ThermocoupleTask",
            (uint16_t)THERMOCOUPLE_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)THERMOCOUPLE_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "ThermocoupleTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the IR Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void ThermocoupleTask::Run(void * pvParams)
{
    uint32_t tempSecondCounter = 0; // TODO: Temporary counter, would normally be in HeartBeat task or HID Task, unless FlightTask is the HeartBeat task

    while (1) {
            Command cm;

            //Wait forever for a command
            qEvtQueue->ReceiveWait(cm);

            //Process the command
            HandleCommand(cm);

            //Every cycle, print something out (for testing)
            SOAR_PRINT("ThermocoupleTask::Run() - [%d] Seconds\n", tempSecondCounter++);
        }
}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void ThermocoupleTask::HandleCommand(Command& cm)
{
    //TODO: Since this task will stall for a few milliseconds, we may need a way to eat the whole queue (combine similar eg. REQUEST commands and eat to WDG command etc)
    //TODO: Maybe a HandleEvtQueue instead that takes in the whole queue and eats the whole thing in order of non-blocking to blocking

    //Switch for the GLOBAL_COMMAND
    switch (cm.GetCommand()) {
    case REQUEST_COMMAND: {
        HandleRequestCommand(cm.GetTaskCommand());
    }
    case TASK_SPECIFIC_COMMAND: {
        break;
    }
    default:
        SOAR_PRINT("LoadCellTask - Received Unsupported Command {%d}\n", cm.GetCommand());
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
    case THERMOCOUPLE_REQUEST_NEW_SAMPLE:
    	SampleThermocouple();
        break;
    case THERMOCOUPLE_REQUEST_TRANSMIT:
        SOAR_PRINT("Stubbed: Thermocouple task transmit not implemented\n");
        break;
    case THERMOCOUPLE_REQUEST_DEBUG:
        SOAR_PRINT("\t-- Thermocouple Data --\n");
        ThermocoupleTask::SampleThermocouple();
        break;
    default:
        SOAR_PRINT("UARTTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
        break;
    }
}

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


	//Storable Data ------------------------------------------------------------------------------

	uint8_t dataBuffer1[4];
	//See Above bit mem-map

	uint8_t Error1=0;// Thermocouple Connection acknowledge Flag
	uint32_t sign1=0;
	int Temp1=0;

	//Read ---------------------------------------------------------------------------------------

    //Read From Thermocouple 1 first
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET); //begin read with CS pin low
	HAL_SPI_Receive(SystemHandles::SPI_Thermocouple, dataBuffer1, 4, 1000); //Fill the data buffer with data from TC1
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET); //end read with setting CS pin to high again

	SOAR_PRINT("------------1-------------\n");

	for(int i = 0; i<4; i++){
		SOAR_PRINT("databufferTC1[%d] are: %d\n",i, dataBuffer1[i]);
	}
	SOAR_PRINT("\n");


	double temp_debug_1 = 0;

	Error1=dataBuffer1[3]&0x07;								  // Error Detection
	sign1=(dataBuffer1[0]&(0x80))>>7;							  // Sign Bit calculation

	if(dataBuffer1[3] & 0x07){								  // Returns Error Number
		temp_debug_1 = (-1*(dataBuffer1[3] & 0x07));
	}
	else if(sign1==1){									  // Negative Temperature
		Temp1 = (dataBuffer1[0] << 6) | (dataBuffer1[1] >> 2);
		Temp1&=0b01111111111111;
		Temp1^=0b01111111111111;
		temp_debug_1 = (double)-Temp1/4;
	}
	else												  // Positive Temperature
	{
		Temp1 = (dataBuffer1[0] << 6) | (dataBuffer1[1] >> 2);
		temp_debug_1 = ((double)Temp1 / 4);
	}

	temp_debug_1 = temp_debug_1*100;
	SOAR_PRINT(
				"\t-- The new Temp as big number say its read by TC1 is %d \n"
				, (int)temp_debug_1);

	SOAR_PRINT("\t-- The new Temp say its read by TC1 is %d.%d C \n"
			"-------------------------\n", (int)temp_debug_1/100, (uint8_t)(int)temp_debug_1%100);



	uint8_t dataBuffer2[4];
	//See Above bit mem-map

	uint8_t Error2=0;// Thermocouple Connection acknowledge Flag
	uint32_t sign2=0;
	int Temp2=0;

	//Read ---------------------------------------------------------------------------------------

	//Read From Thermocouple 1 first
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET); //begin read with CS pin low
	HAL_SPI_Receive(SystemHandles::SPI_Thermocouple, dataBuffer2, 4, 1000); //Fill the data buffer with data from TC1
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET); //end read with setting CS pin to high again


	SOAR_PRINT("------------2-------------\n");

	for(int i = 0; i<4; i++){
		SOAR_PRINT("databufferTC2[%d] are: %d\n",i, dataBuffer2[i]);
	}
	SOAR_PRINT("\n");


	double temp_debug_2 = 0;

	Error2=dataBuffer2[3]&0x07;								  // Error Detection
	sign2=(dataBuffer2[0]&(0x80))>>7;							  // Sign Bit calculation

	if(dataBuffer2[3] & 0x07){								  // Returns Error Number
		temp_debug_2 = (-1*(dataBuffer2[3] & 0x07));
	}
	else if(sign2==1){									  // Negative Temperature
		Temp2 = (dataBuffer2[0] << 6) | (dataBuffer2[1] >> 2);
		Temp2&=0b01111111111111;
		Temp2^=0b01111111111111;
		temp_debug_2 = (double)-Temp2/4;
	}
	else												  // Positive Temperature
	{
		Temp2 = (dataBuffer2[0] << 6) | (dataBuffer2[1] >> 2);
		temp_debug_2 = ((double)Temp2 / 4);
	}

	temp_debug_2 = temp_debug_2*100;
	SOAR_PRINT(
				"\tThe new Temp as big number say its read by TC2 is %d \n", (int)temp_debug_2);

	SOAR_PRINT("\tThe new Temp say its read by TC2 is %d.%d C \n"
			"-------------------------\n", (int)temp_debug_2/100, (uint8_t)(int)temp_debug_2%100);

}
