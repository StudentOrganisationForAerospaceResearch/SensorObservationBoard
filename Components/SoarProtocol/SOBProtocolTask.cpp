/**
 ******************************************************************************
 * File Name          : SOBProtocolTask.hpp
 * Description        : Protocol task, specific to SOB
 ******************************************************************************
*/
#include "SOBProtocolTask.hpp"

#include "FlightTask.hpp"
#include "ReadBufferFixedSize.h"
#include "LoadCellTask.hpp"

/**
 * @brief Initialize the SOBProtocolTask
 */
void SOBProtocolTask::InitTask()
{
    // Make sure the task is not already initialized
    SOAR_ASSERT(rtTaskHandle == nullptr, "Cannot initialize Protocol task twice");

    // Start the task
    BaseType_t rtValue =
        xTaskCreate((TaskFunction_t)SOBProtocolTask::RunTask,
            (const char*)"ProtocolTask",
            (uint16_t)TASK_PROTOCOL_STACK_DEPTH_WORDS,
            (void*)this,
            (UBaseType_t)TASK_PROTOCOL_PRIORITY,
            (TaskHandle_t*)&rtTaskHandle);

    //Ensure creation succeded
    SOAR_ASSERT(rtValue == pdPASS, "ProtocolTask::InitTask - xTaskCreate() failed");
}

/**
 * @brief Default constructor
 */
SOBProtocolTask::SOBProtocolTask() : ProtocolTask(Proto::Node::NODE_SOB)
{
}

/**
 * @brief Handle a command message
 */
void SOBProtocolTask::HandleProtobufCommandMessage(EmbeddedProto::ReadBufferFixedSize<PROTOCOL_RX_BUFFER_SZ_BYTES>& readBuffer)
{
    Proto::CommandMessage msg;
    msg.deserialize(readBuffer);

    // Verify the target node, if it isn't as expected, do nothing
    if (msg.get_target() != Proto::Node::NODE_SOB)
        return;

    // If the message does not have a SOB command, do nothing
    if (!msg.has_sob_command())
        return;

    SOAR_PRINT("PROTO-INFO: Received SOB Command Message\n");

    // Process the SOB command
    switch (msg.get_sob_command().get_command_enum())
    {
    case Proto::SobCommand::Command::SOB_TARE_LOAD_CELL: {
        SOAR_PRINT("PROTO-INFO: Received SOB Tare Load Cell Command\n");
        LoadCellTask::Inst().SendCommand(Command(REQUEST_COMMAND, (uint16_t)LOADCELL_REQUEST_TARE));
        break;
    }
    case Proto::SobCommand::Command::SOB_CALIBRATE_LOAD_CELL: {
        SOAR_PRINT("PROTO-INFO: Received SOB Calibrate Load Cell Command\n");

		// update calibration mass directly
        int32_t mass_mg = msg.get_sob_command().get_command_param();
		LoadCellTask::Inst().SetCalibrationMassGrams((float)mass_mg / 1000);

		// send calibration command to queue -- could be blocking if we protect the LC read
		LoadCellTask::Inst().SendCommand(Command(REQUEST_COMMAND, LOADCELL_REQUEST_CALIBRATE));
		break;
    }
    case Proto::SobCommand::Command::SOB_SLOW_SAMPLE_IR:
    case Proto::SobCommand::Command::SOB_FAST_SAMPLE_IR:
    case Proto::SobCommand::Command::SOB_LAST:
    default:
        break;
    }

}

/**
 * @brief Handle a control message
 */
void SOBProtocolTask::HandleProtobufControlMesssage(EmbeddedProto::ReadBufferFixedSize<PROTOCOL_RX_BUFFER_SZ_BYTES>& readBuffer)
{

}

/**
 * @brief Handle a telemetry message
 */
void SOBProtocolTask::HandleProtobufTelemetryMessage(EmbeddedProto::ReadBufferFixedSize<PROTOCOL_RX_BUFFER_SZ_BYTES>& readBuffer)
{

}
