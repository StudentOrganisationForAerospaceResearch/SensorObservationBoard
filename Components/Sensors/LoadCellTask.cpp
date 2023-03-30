/**
 ******************************************************************************
 * File Name          : LoadCellTask.cpp
 * Description        : Primary LoadCell task, default task for the system.
 ******************************************************************************
*/
#include <Sensors/Inc/LoadCellTask.hpp>
#include "GPIO.hpp"
#include "SystemDefines.hpp"


/**
 * @brief Constructor for FlightTask
 */
LoadCellTask::LoadCellTask() : Task(LOADCELL_TASK_QUEUE_DEPTH_OBJS)
{
}

/**
 * @brief Initialize the FlightTask
 */
void LoadCellTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize flight task twice");
    
    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)LoadCellTask::RunTask,
            (const char*)"LoadCellTask",
            (uint16_t)LOADCELL_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)LOADCELL_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "LoadCellTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the Flight Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void LoadCellTask::Run(void * pvParams)
{

	Command cm(REQUEST_COMMAND, LOADCELL_REQUEST_INIT);
	HandleCommand(cm);
    while (1) {

    	HAL_Delay(500);
    	Command cm1(REQUEST_COMMAND, LOADCELL_REQUEST_TARE);
    	HandleCommand(cm1);

    }
}


/**
 * @brief Handles a command
 * @param cm Command reference to handle
 */
void LoadCellTask::HandleCommand(Command& cm)
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
void LoadCellTask::HandleRequestCommand(uint16_t taskCommand)
{
    //Switch for task specific command within DATA_COMMAND
    switch (taskCommand) {
    case LOADCELL_REQUEST_INIT:
    	LoadCellInit(Clk_GPIO_Port, Clk_Pin , Data_GPIO_Port, Data_Pin);
    	break;
    case LOADCELL_REQUEST_TARE:
    	LoadCellTare();
    	break;
    case LOADCELL_REQUEST_CALIBRATE:
    	LoadCellCalibrate();
    	break;
    case LOADCELL_REQUEST_NEW_SAMPLE:
    	SampleLoadCellData();
        break;
    case LOADCELL_REQUEST_TRANSMIT:
        SOAR_PRINT("Stubbed: LoadCell task transmit not implemented\n");
        break;
    case LOADCELL_REQUEST_DEBUG:
        SOAR_PRINT(" LoadCell Data: %d\n", measuredWeight);
        break;
    default:
        SOAR_PRINT("UARTTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
        break;
    }
}

void LoadCellTask::LoadCellInit(GPIO_TypeDef *clk_gpio, uint16_t clk_pin, GPIO_TypeDef *dat_gpio, uint16_t dat_pin)
{
	hx711_init(&loadcell,clk_gpio , clk_pin ,dat_gpio, dat_pin);
}

void LoadCellTask::LoadCellTare()
{
	SOAR_PRINT("No mass should be on the Load Cell");
	hx711_tare(&loadcell, 10);
	SOAR_PRINT("Tare ADC value %d", loadcell.offset);
	HAL_Delay(50);
	value_noload = hx711_value_ave(&loadcell,10);
	SOAR_PRINT("Average ADC value %d", value_noload);
	HAL_Delay(50);

}

void LoadCellTask::LoadCellCalibrate()
{
	SOAR_PRINT("Apply a known mass to the Load Cell");
	value_loadraw = hx711_value_ave(&loadcell, 10);
	hx711_calibration(&loadcell, value_noload, value_loadraw, knownmass);
}


void LoadCellTask::SampleLoadCellData()
{
	measuredWeight = hx711_weight(&loadcell, 10);

}
