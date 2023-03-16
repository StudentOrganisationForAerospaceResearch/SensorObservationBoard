/**
 ******************************************************************************
 * File Name          : IRTask.hpp
 * Description        : Primary IR task, default task for the system.
 ******************************************************************************
*/
#ifndef SOAR_IRTASK_HPP_
#define SOAR_IRTASK_HPP_
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "../../Drivers/mlx90614 Driver/mlx90614.h"


/* Macros/Enums ------------------------------------------------------------*/
enum IR_TASK_COMMANDS {
    IR_NONE = 0,
    IR_REQUEST_NEW_SAMPLE,// Get a new barometer sample, task will be blocked for polling time
    IR_REQUEST_TRANSMIT,    // Send the current barometer data over the Radio
    IR_REQUEST_DEBUG,        // Send the current barometer data over the Debug UART
	IR_REQUEST_TIMESTAMP, 	// Get timestamp for IR sensors
};


class IRTask : public Task
{
public:
    static IRTask& Inst() {
        static IRTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { IRTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code
    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    void SampleIRTemperature();
    float objectTemp;
    float ambientTemp;
    uint32_t timestampIR;



private:
    // Private Functions
    IRTask();        // Private constructor
    IRTask(const IRTask&);                        // Prevent copy-construction
    IRTask& operator=(const IRTask&);            // Prevent assignment
};

#endif    // SOAR_IRTASK_HPP_
