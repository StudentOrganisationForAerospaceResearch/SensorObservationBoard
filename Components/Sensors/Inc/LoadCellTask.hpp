/**
 ******************************************************************************
 * File Name          : LoadCellTask.hpp
 * Description        : Primary LoadCell task, default task for the system.
 ******************************************************************************
*/
#ifndef SOAR_LOADCELLTASK_HPP_
#define SOAR_LOADCELLTASK_HPP_
#include "Task.hpp"
#include "SystemDefines.hpp"


/* Macros/Enums ------------------------------------------------------------*/
enum LOADCELL_TASK_COMMANDS {
    LOADCELL_NONE = 0,
    LOADCELL_REQUEST_NEW_SAMPLE,// Get a new barometer sample, task will be blocked for polling time
    LOADCELL_REQUEST_TRANSMIT,    // Send the current barometer data over the Radio
    LOADCELL_REQUEST_DEBUG        // Send the current barometer data over the Debug UART
};


class LoadCellTask : public Task
{
public:
    static LoadCellTask& Inst() {
        static LoadCellTask inst;
        return inst;
    }

    void InitTask();

protected:
    static void RunTask(void* pvParams) { LoadCellTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    void Run(void * pvParams); // Main run code


    void HandleCommand(Command& cm);
    void HandleRequestCommand(uint16_t taskCommand);

    void SampleLoadCellData();


private:
    // Private Functions
    LoadCellTask();        // Private constructor
    LoadCellTask(const LoadCellTask&);                        // Prevent copy-construction
    LoadCellTask& operator=(const LoadCellTask&);            // Prevent assignment
};

#endif    // SOAR_LOADCELLTASK_HPP_
