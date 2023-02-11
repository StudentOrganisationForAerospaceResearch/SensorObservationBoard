/**
 ******************************************************************************
 * File Name          : ThermoCoupleTask.hpp
 * Description        : Primary IR task, default task for the system.
 ******************************************************************************
*/
#ifndef SOAR_THERMOCOUPLETASK_HPP_
#define SOAR_THERMOCOUPLETASK_HPP_
#include "Task.hpp"
#include "SystemDefines.hpp"
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


private:
    // Private Functions
    ThermocoupleTask();        // Private constructor
    ThermocoupleTask(const ThermocoupleTask&);                        // Prevent copy-construction
    ThermocoupleTask& operator=(const ThermocoupleTask&);            // Prevent assignment
};

#endif    // SOAR_THERMOCOUPLETASK_HPP_
