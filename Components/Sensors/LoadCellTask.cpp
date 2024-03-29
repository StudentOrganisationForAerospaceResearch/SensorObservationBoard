/**
 ******************************************************************************
 * File Name          : LoadCellTask.cpp
 * Description        : Primary LoadCell task, default task for the system.
 ******************************************************************************
*/
#include <stdlib.h>
#include "LoadCellTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "SOBProtocolTask.hpp"

/**
 * @brief Constructor for LoadCellTask
 */
LoadCellTask::LoadCellTask() : Task(LOADCELL_TASK_QUEUE_DEPTH_OBJS)
{
}

/**
 * @brief Initialize the LoadCellTask
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
 * @brief Instance Run loop for the LoadCellTask, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void LoadCellTask::Run(void * pvParams)
{
	hx711_init(&loadcell, LC_CLK_GPIO_Port, LC_CLK_Pin , LC_DATA_GPIO_Port, LC_DATA_Pin);
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
void LoadCellTask::HandleCommand(Command& cm)
{
    //TODO: Since this task will stall for a few milliseconds, we may need a way to eat the whole queue (combine similar eg. REQUEST commands and eat to WDG command etc)
    //TODO: Maybe a HandleEvtQueue instead that takes in the whole queue and eats the whole thing in order of non-blocking to blocking

	//NOTE: if receiving corrupt data from load cell task, consider disabling/enabling interrupts before/after reading load cell with bit banging
    //Switch for the GLOBAL_COMMAND
    switch (cm.GetCommand()) {
    case REQUEST_COMMAND: {
        HandleRequestCommand(cm.GetTaskCommand());
        break;
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
    case LOADCELL_REQUEST_TARE: {
    	LoadCellTare();
    	break;
    }
    case LOADCELL_REQUEST_CALIBRATE: {
       	LoadCellCalibrate();
       	break;
    }
    case LOADCELL_REQUEST_NEW_SAMPLE: {
    	SampleLoadCellData();
        break;
    }
    case LOADCELL_REQUEST_TRANSMIT: {
    	TransmitProtocolLoadCellData();
        break;
    }
    case LOADCELL_REQUEST_CALIBRATION_DEBUG: {
    	SOAR_PRINT("Load Cell offset %d \n", loadcell.offset);
    	SOAR_PRINT("Load Cell coef %d.%d \n", (int)loadcell.coef, abs(int(loadcell.coef * 1000) % 1000));
    	SOAR_PRINT("Load Cell calibration weight %d.%d grams\n", (int)calibration_mass_g, abs(int(calibration_mass_g * 1000) % 1000));
    	break;
    }
    case LOADCELL_REQUEST_DEBUG: {
        SOAR_PRINT("Load Cell read weight: %d.%d grams\n", (int)rocket_mass_sample.weight_g, abs(int(rocket_mass_sample.weight_g * 1000) % 1000));
        break;
    }
    default:
        SOAR_PRINT("LoadCellTask - Received Unsupported REQUEST_COMMAND {%d}\n", taskCommand);
        break;
    }
}

/**
 * @brief Sets up the load cell during tare. We need to call this before weighing
 * any mass. This is the second call.
 * @param none
 */
void LoadCellTask::LoadCellTare()
{
	hx711_reset_coef_offset(&loadcell);
	hx711_tare(&loadcell, 10);
	SOAR_PRINT("Load Cell offset %d \n", loadcell.offset);
}
/**
 * @brief Calculates the calibration coefficient for calibration with a known mass.
 * This is the third call.
 * @param none
 */
void LoadCellTask::LoadCellCalibrate()
{
	if (calibration_mass_g <= 0) {
		SOAR_PRINT("Load Cell requires positive, nonzero calibration weight\n");
		return;
	}

	int32_t load_raw = hx711_value_ave(&loadcell, 10);
	hx711_calibration(&loadcell, loadcell.offset, load_raw, calibration_mass_g);
	SOAR_PRINT("Load Cell coef %d.%d \n", (int)loadcell.coef, abs(int(loadcell.coef * 1000) % 1000));
}

/**
 * @brief This samples the weight of any given mass after calibration. This is the
 * fourth call.
 * @param none
 */
void LoadCellTask::SampleLoadCellData()
{
	uint32_t ADCdata;
	rocket_mass_sample.weight_g = hx711_weight(&loadcell, 10, ADCdata);
	rocket_mass_sample.timestamp_ms = HAL_GetTick();
}

void LoadCellTask::TransmitProtocolLoadCellData()
{
    Proto::TelemetryMessage msg;
	msg.set_source(Proto::Node::NODE_SOB);
	msg.set_target(Proto::Node::NODE_RCU);
	msg.set_message_id((uint32_t)Proto::MessageID::MSG_TELEMETRY);

	Proto::LRLoadCell loadCellSample;
	loadCellSample.set_rocket_mass(rocket_mass_sample.weight_g);
	msg.set_lr(loadCellSample);

	EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE> writeBuffer;
	msg.serialize(writeBuffer);

    // Send the load cell data
    SOBProtocolTask::SendProtobufMessage(writeBuffer, Proto::MessageID::MSG_TELEMETRY);
}
