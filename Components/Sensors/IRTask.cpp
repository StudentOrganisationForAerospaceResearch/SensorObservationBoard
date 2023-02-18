/**
 ******************************************************************************
 * File Name          : IRTask.cpp
 * Description        : Primary flight task, default task for the system.
 ******************************************************************************
*/
#include "IRTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "../../Drivers/mlx90614 Driver/mlx90614.h"


/**
 * @brief Constructor for IRTask
 */
IRTask::IRTask() : Task(FLIGHT_TASK_QUEUE_DEPTH_OBJS)
{
}

/**
 * @brief Initialize the IRTask
 */
void IRTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize IR task twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)IRTask::RunTask,
            (const char*)"IRTask",
            (uint16_t)IR_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)IR_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "IRTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the IR Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void IRTask::Run(void * pvParams)
{

    while (1) {


        float tempReading = MLX90614_ReadTemp(hi2c1,0x5A,0x07);
        int temp =  static_cast<int>(tempReading);
        //Every cycle, print something out (for testing)
        SOAR_PRINT("Temperature reading: [%d]\n", temp );
        //SOAR_PRINT("IRTask::Run() - [%d] Seconds\n", tempSecondCounter++);

    }
}

/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void IRTask::HandleCommand(Command& cm)
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
        SOAR_PRINT("IRTask - Received Unsupported Command {%d}\n", cm.GetCommand());
        break;
    }

    //No matter what we happens, we must reset allocated data
    cm.Reset();
}


/**
 * @brief Handles a Request Command
 * @param taskCommand The command to handle
 */
void IRTask::HandleRequestCommand(uint16_t taskCommand)
{
    //Switch for task specific command within DATA_COMMAND
    switch (taskCommand) {
    case IR_REQUEST_NEW_SAMPLE:
    	SampleIRTemperature();
        break;
    case IR_REQUEST_TRANSMIT:
        SOAR_PRINT("Stubbed: IR task transmit not implemented\n");
        break;
    case IR_REQUEST_DEBUG:
        SOAR_PRINT("\t-- IR Data --\n");
        SOAR_PRINT(" Temp (C)       : %d.%d\n", 10,10);
        break;
    default:
        SOAR_PRINT("UARTTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
        break;
    }
}

void IRTask::SampleIRTemperature()
{

}
