#include "errorcodedefinitions.hpp"

namespace Exchange
{

#define EXCHANGE_CODE_TO_TEXT(a) case ErrorCode::a: return #a

std::string errorCodeToText(ErrorCode code)
{
    switch (code)
    {
    EXCHANGE_CODE_TO_TEXT(NoError);

    // 0** --> System errors (program errors)
    EXCHANGE_CODE_TO_TEXT(SystemUnknown);
    EXCHANGE_CODE_TO_TEXT(SystemNotImplemented);
    EXCHANGE_CODE_TO_TEXT(SystemInvalidConfig);
    EXCHANGE_CODE_TO_TEXT(SystemInvalidArgument);

    // 1** --> Protocol errors (exchange)
    EXCHANGE_CODE_TO_TEXT(ProtocolUnknown);
    EXCHANGE_CODE_TO_TEXT(ProtocolInvalidVersion);
    EXCHANGE_CODE_TO_TEXT(ProtocolNoData);
    EXCHANGE_CODE_TO_TEXT(ProtocolInvalidData);
    EXCHANGE_CODE_TO_TEXT(ProtocolJsonException);
    EXCHANGE_CODE_TO_TEXT(ProtocolWSInvalidEventId);
    EXCHANGE_CODE_TO_TEXT(ProtocolWSInvalidEventType);

    // 2** --> Interface (TCP/IP transport layer) errors
    EXCHANGE_CODE_TO_TEXT(InterfaceUnknown);
    EXCHANGE_CODE_TO_TEXT(InterfaceInvalidAddress);
    EXCHANGE_CODE_TO_TEXT(InterfaceStartFailed);
    EXCHANGE_CODE_TO_TEXT(InterfaceStopFailed);
    EXCHANGE_CODE_TO_TEXT(InterfaceConnectionError);

    // 10** --> Model errors
    EXCHANGE_CODE_TO_TEXT(GuiModelUnknown);
    EXCHANGE_CODE_TO_TEXT(GuiModelInvalidModel);
    EXCHANGE_CODE_TO_TEXT(GuiModelInvalidIndex);

    EXCHANGE_CODE_TO_TEXT(ExtraCustom);
    }
    return errorCodeToText(ErrorCode::SystemUnknown);
}

}