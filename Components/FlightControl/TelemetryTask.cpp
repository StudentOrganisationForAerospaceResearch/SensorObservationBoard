/**
 ******************************************************************************
 * File Name          : TelemetryTask.cpp
 * Description        : Primary telemetry task, default task for the system.
 ******************************************************************************
*/
#include "TelemetryTask.hpp"
#include "GPIO.hpp"
#include "SystemDefines.hpp"
#include "SOBProtocolTask.hpp"
#include "FlightTask.hpp"
#include "LoadCellTask.hpp"
#include "ThermocoupleTask.hpp"

/**
 * @brief Constructor for TelemetryTask
 */
TelemetryTask::TelemetryTask() : Task(TELEMETRY_TASK_QUEUE_DEPTH_OBJS)
{
    loggingDelayMs = TELEMETRY_DEFAULT_LOGGING_RATE_MS;
}

/**
 * @brief Initialize the TelemetryTask
 */
void TelemetryTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize telemetry task twice");

    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)TelemetryTask::RunTask,
            (const char*)"TelemetryTask",
            (uint16_t)TELEMETRY_TASK_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)TELEMETRY_TASK_RTOS_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    SOAR_ASSERT(rtValue == pdPASS, "TelemetryTask::InitTask() - xTaskCreate() failed");
}

/**
 * @brief Instance Run loop for the Telemetry Task, runs on scheduler start as long as the task is initialized.
 * @param pvParams RTOS Passed void parameters, contains a pointer to the object instance, should not be used
 */
void TelemetryTask::Run(void* pvParams)
{
    while (1) {
        //Process all commands in queue this cycle
        Command cm;
		while (qEvtQueue->Receive(cm))
            HandleCommand(cm);

    }
}

/**
 * @brief Handles a command from the command queue
 * @param cm Command to handle
 */
void TelemetryTask::HandleCommand(Command& cm)
{
    //Switch for the GLOBAL_COMMAND
    switch (cm.GetCommand()) {
    case TELEMETRY_SEND_DATA: {
        RunLogSequence();
	break;
    }
    case TELEMETRY_SEND_EOF: {
    	SendEOF();
    	break;
     }
    case TELEMETRY_LOADCELL_EOF:
    case TELEMETRY_THERMOCOUPLE_EOF: {
    	if(counter == 2)
    	{
    		SendEOF();
    	}
    	else
    	{
    		counter++;
    	}
     break;
    }

    default:
        SOAR_PRINT("TelemetryTask - Received Unsupported Command {%d}\n", cm.GetCommand());
        break;
    }

    //No matter what we happens, we must reset allocated data
    cm.Reset();
}

/**
 * @brief Runs a full logging sample/send sequence.
 *        can assume this is called with a period of loggingDelayMs
 */
void TelemetryTask::RunLogSequence()
{
	// Load Cell
    LoadCellTask::Inst().SendCommand(Command(REQUEST_COMMAND, (uint16_t)LOADCELL_REQUEST_NEW_SAMPLE));
    LoadCellTask::Inst().SendCommand(Command(REQUEST_COMMAND, (uint16_t)LOADCELL_REQUEST_TRANSMIT));

    // Thermocouple
    ThermocoupleTask::Inst().SendCommand(Command(REQUEST_COMMAND, THERMOCOUPLE_REQUEST_NEW_SAMPLE));
	ThermocoupleTask::Inst().SendCommand(Command(REQUEST_COMMAND, THERMOCOUPLE_REQUEST_TRANSMIT));

}

void TelemetryTask::SendEOF()
{
	//SOB_FAST_SAMPLE_IR for the command message
	Proto::CommmandMessage dataEndMsg;
	dataEndMsg.set_source(Proto::Node::NODE_SOB);
	dataEndMsg.set_target(Proto::Node::NODE_RCU);
	dataEndMsg.set_message_id((uint32_t)Proto::MessageID::MSG_COMMAND);

	EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE> dataEndWriteBuffer;
	dataEndMsg.serialize(dataEndWriteBuffer);

	SOBxProtocolTask::SendProtobufMessage(dataEndWriteBuffer, Proto::MessageID::MSG_COMMAND);
}
