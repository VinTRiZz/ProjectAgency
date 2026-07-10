#pragma once

#include <string>

#include "errorcodedefinitions.hpp"

namespace Exchange
{


/**
 * @brief The ErrorObject class Class for error describing
 */
class Error : public std::exception
{
public:
    Error() = default;
    Error(ErrorCode errCode, const std::string& detailText = {});

    // exception interface
    const char *what() const noexcept override;

    // USED IN CONDITIONS WITH NO LOGGER AVAILABLE
    void printSelfStd() const;

    // USED IN CONDITIONS WITH LOGGER "ALIVE"
    void printSelf();
    static void printSelf(
        ErrorCode errCode,
        const std::string& detailText = {});

    // Reset restores to NoError state (sugar)
    void reset();
    void setCode(ErrorCode errCode);
    ErrorCode getCode() const;

    bool isOk() const;
    operator bool() const;

    std::string getErrorText() const;

    void setDetailText(const std::string& errText);
    std::string getErrorDetailText() const;

private:
    ErrorCode           m_errorCode {ErrorCode::NoError};
    std::string         m_detailText;
    mutable std::string m_dumpText; // std::exception inherit workaround (see method what() )
};


/**
 * @brief The ErrorUser class Subclass for error handling
 */
class ErrorUser
{
public:
    bool isValid() const;

    Error getError() const;

protected:
    mutable Error m_error;
};


}