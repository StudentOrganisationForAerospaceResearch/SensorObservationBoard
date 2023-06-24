/**
 ******************************************************************************
 * File Name          : Thermocouple.hpp
 * Description        : TermcoupleTask contains termpurature data along with functions to
 * 						read the thermocouples, transmit the tempuature data,
 * 						and send a debug message to a terminal
 ******************************************************************************
*/

#ifndef SOAR_SENSOR_THERMOCOUPLE_TASK_HPP_
#define SOAR_SENSOR_THERMOCOUPLE_TASK_HPP_

/* Includes ------------------------------------------------------------------*/

#include "Task.hpp"
#include "SystemDefines.hpp"

/* Macros/Enums ------------------------------------------------------------*/
enum THERMOCOUPLE_TASK_COMMANDS {
	THERMOCOUPLE_NONE= 0,
	THERMOCOUPLE_REQUEST_NEW_SAMPLE,	// Get a new temperature sample
	THERMOCOUPLE_REQUEST_TRANSMIT,		// Send the current temperature over the Protobuff
	THERMOCOUPLE_REQUEST_DEBUG       	// Send the current temperature data over the Debug UART
};

/* Class ------------------------------------------------------------------*/
class ThermocoupleTask : public Task
{
public:
    static ThermocoupleTask& Inst() {
        static ThermocoupleTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { ThermocoupleTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void* pvParams);    // Main run code

    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    // Sampling
    void TransmitProtocolThermoData();
    void SampleThermocouple();
    void ThermocoupleDebugPrint();
    int16_t ExtractTempurature(uint8_t temperatureData[]);

    //Fields
    uint8_t dataBuffer1[4] = {0};
    uint8_t dataBuffer2[4] = {0};
    int16_t temperature1 = 0;
    int16_t temperature2 = 0;

private:
    ThermocoupleTask();                                        	// Private constructor
    ThermocoupleTask(const ThermocoupleTask&);                  // Prevent copy-construction
    ThermocoupleTask& operator=(const ThermocoupleTask&);       // Prevent assignment
};

#endif 		//SOAR_SENSOR_THERMOCOUPLE_TASK_HPP_
