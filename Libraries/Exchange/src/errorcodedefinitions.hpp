#pragma once

#include <string>
#include <map>
#include <stdint.h>

namespace Exchange {

/**
 * @brief The ErrorCode enum Error codes for internal usage
 */
enum class ErrorCode : uint16_t
{
    NoError = 0,

    // =============================== //
    // == NETWORK AND SYSTEM ERRORS == //
    // =============================== //

    // 0** --> System errors (program errors)
    SystemUnknown = 1,
    SystemNotImplemented,
    SystemInvalidConfig,
    SystemObjectNotInited,
    SystemInvalidArgument,
    SystemDBError,


    // 1** --> Protocol errors (exchange)
    ProtocolUnknown = 100,
    ProtocolInvalidVersion,
    ProtocolNoData,
    ProtocolInvalidData,
    ProtocolJsonException,
    ProtocolWSInvalidEventId,
    ProtocolWSInvalidEventType,


    // 2** --> Interface (TCP/IP transport layer) errors
    InterfaceUnknown = 200,
    InterfaceInvalidAddress,
    InterfaceStartFailed,
    InterfaceStopFailed,
    InterfaceConnectionError,

    // =============================== //
    // ========= GUI ERRORS ========== //
    // =============================== //

    // 10** --> Common errors
    GuiUnknown = 1000,
    GuiServerProcessingFail,

    // 11** --> Model errors
    GuiModelUnknown = 1100,
    GuiModelInvalidModel,
    GuiModelInvalidIndex,

    // =============================== //
    // ======== EXTRA ERRORS ========= //
    // =============================== //
    ExtraUnknown = 10000
};

std::string errorCodeToText(ErrorCode code);

}