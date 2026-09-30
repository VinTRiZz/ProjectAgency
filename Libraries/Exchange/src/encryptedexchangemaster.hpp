#pragma once

#include <string>
#include <memory>
#include <optional>

#include <ProjectAgency/Exchange/Error.h>

namespace Exchange {

/**
 * @brief The EncryptedExchangeMaster class Class, used as a proxy in assymetric data exchange
 */
class EncryptedExchangeMaster : public ErrorUser
{
public:
    EncryptedExchangeMaster();
    ~EncryptedExchangeMaster();

    bool init(); // Replaces old RSA key

    std::string getPubkey() const;
    bool setEncryptionKey(const std::string& pubkeyStr);
    bool canEncrypt() const;
    std::optional<std::string> encrypt(const std::string& inputStr) const;
    std::optional<std::string> decrypt(const std::string& encStr) const;

private:

    struct Impl;
    std::shared_ptr<Impl> d;
};

}