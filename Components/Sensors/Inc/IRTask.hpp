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


private:
    // Private Functions
    IRTask();        // Private constructor
    IRTask(const IRTask&);                        // Prevent copy-construction
    IRTask& operator=(const IRTask&);            // Prevent assignment
};

#endif    // SOAR_IRTASK_HPP_
