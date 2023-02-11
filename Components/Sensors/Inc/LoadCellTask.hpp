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


private:
    // Private Functions
    LoadCellTask();        // Private constructor
    LoadCellTask(const LoadCellTask&);                        // Prevent copy-construction
    LoadCellTask& operator=(const LoadCellTask&);            // Prevent assignment
};

#endif    // SOAR_LOADCELLTASK_HPP_
