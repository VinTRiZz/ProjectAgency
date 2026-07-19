#include "encryptedexchangemaster.hpp"

#include <cstring>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/RSA.h>
#include <Components/Encryption/Encoding.h>

namespace Exchange {

struct EncryptedExchangeMaster::Impl
{
    EVP_PKEY* pkey {nullptr};
};

EncryptedExchangeMaster::EncryptedExchangeMaster() :
    d{new Impl}
{

}

EncryptedExchangeMaster::~EncryptedExchangeMaster()
{

}

bool EncryptedExchangeMaster::init()
{
    d->pkey = Encryption::rsaGenerateKeys();
    auto isInited = (nullptr != d->pkey);
    if (!isInited) {
        m_error.setCode(ErrorCode::InterfaceEncInitError);
        m_error.setDetailText(Encryption::getEncryptionErrorText());
    }
    return isInited;
}

std::string EncryptedExchangeMaster::getPubkey() const
{
    return Encryption::rsaKeyToString(d->pkey);
}

std::optional<std::string> EncryptedExchangeMaster::encrypt(const std::string &inputStr, const std::string &pubKeyStr) const
{
    auto pubKey = Encryption::rsaKeyFromString(pubKeyStr);
    if (!pubKey) {
        m_error.setCode(ErrorCode::InterfaceEncInvalidPubkey);
        m_error.setDetailText(Encryption::getEncryptionErrorText());
        return {};
    }
    auto res = Encryption::rsaEncryptString(pubKey, inputStr);
    if (!res.has_value()) {
        m_error.setCode(ErrorCode::InterfaceEncMsgEncError);
        m_error.setDetailText(Encryption::getEncryptionErrorText());
        return {};
    }
    auto resH = Encryption::encodeHex(*res);
    if (resH.empty()) {
        m_error.setCode(ErrorCode::InterfaceEncMsgEncError);
        m_error.setDetailText(Encryption::getEncryptionErrorText());
    }
    return resH;
}

std::optional<std::string> EncryptedExchangeMaster::decrypt(const std::string &encStr) const
{
    auto res = Encryption::rsaDecryptString(d->pkey, Encryption::decodeHex(encStr));
    if (!res.has_value()) {
        m_error.setCode(ErrorCode::InterfaceEncMsgDecError);
        m_error.setDetailText(Encryption::getEncryptionErrorText());
        return {};
    }
    return res;
}

}