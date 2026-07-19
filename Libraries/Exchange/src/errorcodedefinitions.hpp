#pragma once

#include <string>
#include <Components/ExtraClasses/Error.h>

namespace Exchange {

/**
 * @brief The ErrorCode enum Error codes for internal usage
 */
enum ErrorCode : int
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
    InterfaceEncUnknown,
    InterfaceEncInitError,
    InterfaceEncInvalidPubkey,
    InterfaceEncMsgEncError,
    InterfaceEncMsgDecError,

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

class Error : public ExtraClasses::ErrorBase
{
public:
    using ExtraClasses::ErrorBase::ErrorBase;

    // Inheritance issues
    Error(const ErrorBase &err) : ErrorBase(err) { }
    Error(ErrorBase &&err) : ErrorBase(std::move(err)) {}

    // Inheritance issues
    template <typename ErrorT>
    Error &operator=(ErrorT&& err) {
        setCode(err.getCode());
        setDetailText(err.getDetailText());
        return *this;
    }

    // Error interface
    std::string errorCodeToText(int errc) const override;
};
using ErrorUser = ExtraClasses::ErrorUserBase<Exchange::Error>;

}