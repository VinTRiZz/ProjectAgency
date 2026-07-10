#include "error.hpp"

#include <Components/Logger/Logger.h>

namespace Exchange
{

Error::Error(ErrorCode errCode, const std::string &detailText) :
    std::exception(),
    m_errorCode {errCode},
    m_detailText {detailText}
{

}

const char *Error::what() const noexcept
{
    m_dumpText = getErrorText() + (m_detailText.empty() ? std::string() : std::string(" (") + m_detailText + ")");
    return m_dumpText.c_str();
}

void Error::printSelfStd() const
{
    std::cerr << what() << std::endl; // Logger can not be stated or invalid
}

void Error::printSelf()
{
    COMPLOG_ERROR(what());
}

void Error::printSelf(ErrorCode errCode, const std::string &detailText)
{
    COMPLOG_ERROR(errorCodeToText(errCode), detailText);
}

void Error::reset() {
    setCode(ErrorCode::NoError);
}

void Error::setCode(ErrorCode errCode)
{
    m_errorCode = errCode;
    if (m_errorCode == ErrorCode::NoError) {
        m_detailText.clear();
    }
}

ErrorCode Error::getCode() const
{
    return m_errorCode;
}

bool Error::isOk() const
{
    return (m_errorCode == ErrorCode::NoError);
}

Error::operator bool() const
{
    return isOk();
}

std::string Error::getErrorText() const
{
    return errorCodeToText(m_errorCode);
}

void Error::setDetailText(const std::string &errText)
{
    m_detailText = errText;
}

std::string Error::getErrorDetailText() const
{
    return m_detailText;
}



bool ErrorUser::isValid() const {
    return m_error.isOk();
}

Error ErrorUser::getError() const {
    return m_error;
}

}