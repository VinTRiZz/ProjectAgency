#include "errorcodedefinitions.hpp"

namespace Exchange
{

#define EXCHANGE_CODE_TO_TEXT(a) case ErrorCode::a: return #a
std::string Error::errorCodeToText(int code) const
{
    switch (code)
    {
    EXCHANGE_CODE_TO_TEXT(NoError);

    EXCHANGE_CODE_TO_TEXT(SystemUnknown);
    EXCHANGE_CODE_TO_TEXT(SystemNotImplemented);
    EXCHANGE_CODE_TO_TEXT(SystemInvalidConfig);
    EXCHANGE_CODE_TO_TEXT(SystemObjectNotInited);
    EXCHANGE_CODE_TO_TEXT(SystemInvalidArgument);
    EXCHANGE_CODE_TO_TEXT(SystemDBError);

    EXCHANGE_CODE_TO_TEXT(ProtocolUnknown);
    EXCHANGE_CODE_TO_TEXT(ProtocolInvalidVersion);
    EXCHANGE_CODE_TO_TEXT(ProtocolNoData);
    EXCHANGE_CODE_TO_TEXT(ProtocolInvalidData);
    EXCHANGE_CODE_TO_TEXT(ProtocolJsonException);
    EXCHANGE_CODE_TO_TEXT(ProtocolWSInvalidEventId);
    EXCHANGE_CODE_TO_TEXT(ProtocolWSInvalidEventType);

    EXCHANGE_CODE_TO_TEXT(InterfaceUnknown);
    EXCHANGE_CODE_TO_TEXT(InterfaceInvalidAddress);
    EXCHANGE_CODE_TO_TEXT(InterfaceStartFailed);
    EXCHANGE_CODE_TO_TEXT(InterfaceStopFailed);
    EXCHANGE_CODE_TO_TEXT(InterfaceConnectionError);

    EXCHANGE_CODE_TO_TEXT(GuiUnknown);
    EXCHANGE_CODE_TO_TEXT(GuiServerProcessingFail);

    EXCHANGE_CODE_TO_TEXT(GuiModelUnknown);
    EXCHANGE_CODE_TO_TEXT(GuiModelInvalidModel);
    EXCHANGE_CODE_TO_TEXT(GuiModelInvalidIndex);

    case ErrorCode::ExtraUnknown: return errorCodeToText(ErrorCode::SystemUnknown);
    }
    return errorCodeToText(ErrorCode::SystemUnknown);
}

}