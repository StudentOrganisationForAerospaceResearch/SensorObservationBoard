/**
 ******************************************************************************
 * File Name          : IRTask.cpp
 * Description        : Primary IR task, default task for the system.
 ******************************************************************************
*/
#include "IRTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "../../Drivers/mlx90614 Driver/mlx90614.h"


/**
 * @brief Constructor for IRTask
 */
IRTask::IRTask() : Task(IR_TASK_QUEUE_DEPTH_OBJS)
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

        Command cm;

        //Wait forever for a command
        qEvtQueue->ReceiveWait(cm);

        //Process the command
        HandleCommand(cm);

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
	    case IR_REQUEST_DEBUG: {
	        SOAR_PRINT("|IR_TASK| Object Temp: %d, Ambient Temp: %d, MCU Timestamp: %u\n", static_cast<int>(irSample.object_temp * 100),
	        static_cast<int>(irSample.ambient_temp * 100),irSample.timestamp);
	        break;
	    }
	    default:
	        SOAR_PRINT("IRTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
	        break;
	    }
}

/**
 * @brief Samples the IR sensor data (object and ambient temperature).
 * @param no params
 */
void IRTask::SampleIRTemperature()
{
	irSample.object_temp = MLX90614_ReadTemp(hi2c1,MLX90614_DEFAULT_SA,MLX90614_TOBJ1);
	irSample.ambient_temp = MLX90614_ReadTemp(hi2c1,MLX90614_DEFAULT_SA,MLX90614_TAMB);
	irSample.timestamp = HAL_GetTick();
}

