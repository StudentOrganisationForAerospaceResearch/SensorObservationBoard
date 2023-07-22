/**
 ******************************************************************************
 * File Name          : SOBProtocolTask.hpp
 * Description        : Protocol task, specific to SOB
 ******************************************************************************
*/
#ifndef SOAR_SOBPROTOCOL_HPP_
#define SOAR_SOBPROTOCOL_HPP_
#include "ProtocolTask.hpp"
#include "Task.hpp"
#include "SystemDefines.hpp"
#include "UARTTask.hpp"

/* Enums ------------------------------------------------------------------*/

/* Class ------------------------------------------------------------------*/
class SOBProtocolTask : public ProtocolTask
{
public:
    static SOBProtocolTask& Inst() {
        static SOBProtocolTask inst;
        return inst;
    }

    void InitTask();

    static void SendProtobufMessage(EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE>& writeBuffer, Proto::MessageID msgId)
    {
        Inst().ProtocolTask::SendProtobufMessage(writeBuffer, msgId);
    }

    static void SendRCUCommand(Proto::RCUCommand::Command cmd)
    {
        Proto::CommandMessage cmdMsg;
        Proto::RCUCommand rcuCmd;
        cmdMsg.set_source(Proto::Node::NODE_SOB);
        cmdMsg.set_target(Proto::Node::NODE_RCU);
        rcuCmd.set_command_enum(cmd);
        cmdMsg.set_rcu_command(rcuCmd);
        EmbeddedProto::WriteBufferFixedSize<DEFAULT_PROTOCOL_WRITE_BUFFER_SIZE> writeBuffer;
        cmdMsg.serialize(writeBuffer);
        SOBProtocolTask::SendProtobufMessage(writeBuffer, Proto::MessageID::MSG_COMMAND);
    }

protected:
    static void RunTask(void* pvParams) { SOBProtocolTask::Inst().Run(pvParams); } // Static Task Interface, passes control to the instance Run();

    // These handlers will receive a buffer and size corresponding to a decoded message
    void HandleProtobufCommandMessage(EmbeddedProto::ReadBufferFixedSize<PROTOCOL_RX_BUFFER_SZ_BYTES>& readBuffer);
    void HandleProtobufControlMesssage(EmbeddedProto::ReadBufferFixedSize<PROTOCOL_RX_BUFFER_SZ_BYTES>& readBuffer);
    void HandleProtobufTelemetryMessage(EmbeddedProto::ReadBufferFixedSize<PROTOCOL_RX_BUFFER_SZ_BYTES>& readBuffer);
    
    // Member variables

private:
    SOBProtocolTask();        // Private constructor
    SOBProtocolTask(const SOBProtocolTask&);                        // Prevent copy-construction
    SOBProtocolTask& operator=(const SOBProtocolTask&);            // Prevent assignment
};

#endif    // SOAR_SOBPROTOCOL_HPP_
