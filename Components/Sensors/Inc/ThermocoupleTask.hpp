/**
 ******************************************************************************
 * File Name          : ThermoCoupleTask.hpp
 * Description        : Primary Thermocouple task, default task for the system.
 ******************************************************************************
*/
#ifndef SOAR_THERMOCOUPLETASK_HPP_
#define SOAR_THERMOCOUPLETASK_HPP_
#include "Task.hpp"
#include "SystemDefines.hpp"


/* Macros/Enums ------------------------------------------------------------*/
enum THERMOCOUPLE_TASK_COMMANDS {
    THERMOCOUPLE_NONE = 0,
    THERMOCOUPLE_REQUEST_NEW_SAMPLE,  // Get a new thermocouple sample, task will be blocked for polling time
    THERMOCOUPLE_REQUEST_TRANSMIT,    // Send the current thermocouple data over the Radio
    THERMOCOUPLE_REQUEST_DEBUG        // Send the current thermocouple data over the Debug UART
};


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

    void Run(void * pvParams); // Main run code


    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    void SampleThermocoupleData();


private:
    // Private Functions
    ThermocoupleTask();        // Private constructor
    ThermocoupleTask(const ThermocoupleTask&);                        // Prevent copy-construction
    ThermocoupleTask& operator=(const ThermocoupleTask&);            // Prevent assignment
};

#endif    // SOAR_THERMOCOUPLETASK_HPP_
