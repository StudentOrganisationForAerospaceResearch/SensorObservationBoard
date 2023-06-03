/**
 ******************************************************************************
 * File Name          : ThermocoupleTask.cpp
 * Description        : Primary thermocouple task, default task for the system.
 ******************************************************************************
*/
#include <Sensors/Inc/ThermocoupleTask.hpp>
#include "GPIO.hpp"
#include "SystemDefines.hpp"


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
    	SampleThermocoupleData();
        break;
    case THERMOCOUPLE_REQUEST_TRANSMIT:
        SOAR_PRINT("Stubbed: Thermocouple task transmit not implemented\n");
        break;
    case THERMOCOUPLE_REQUEST_DEBUG:
        SOAR_PRINT("\t-- Thermocouple Data --\n");
        SOAR_PRINT(" Thermocouple Data       : %d.%d\n", 10,10);
        break;
    default:
        SOAR_PRINT("UARTTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
        break;
    }
}

void ThermocoupleTask::SampleThermocoupleData()
{

}