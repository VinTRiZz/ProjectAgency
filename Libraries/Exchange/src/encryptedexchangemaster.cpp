#include "encryptedexchangemaster.hpp"

#include <cstring>

#include <Components/Logger/Logger.h>
#include <Components/Encryption/RSA.h>
#include <Components/Encryption/Encoding.h>
#include <Components/ExtraClasses/DataFragmentator.h>

namespace Exchange {

// RSA does not process data in blocks more than N bytes (~230)
static constexpr auto RSA_ENCRYPTION_MAX_LEN {200};

struct EncryptedExchangeMaster::Impl
{
    std::shared_ptr<EVP_PKEY> pkey {};
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
        m_error.setDetailText(std::string("Key decoding: ") + Encryption::getEncryptionErrorText());
        return {};
    }
    return Encryption::rsaEncryptString(pubKey, inputStr);
}

std::optional<std::string> EncryptedExchangeMaster::decrypt(const std::string &encStr) const
{
    auto dec = Encryption::rsaDecryptString(d->pkey, encStr);
    if (!dec.has_value()) {
        m_error.setCode(ErrorCode::InterfaceEncMsgDecError);
        m_error.setDetailText(std::string("Decryption process: ") + Encryption::getEncryptionErrorText());
    }
    return dec;
}

}