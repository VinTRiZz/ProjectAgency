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
        m_error.setDetailText(std::string("Key decoding: ") + Encryption::getEncryptionErrorText());
        return {};
    }

    auto splittedData = ExtraClasses::DataInfo::split(inputStr, RSA_ENCRYPTION_MAX_LEN);
    std::string resH;
    for (auto& pt : splittedData) {
        auto res = Encryption::rsaEncryptString(pubKey, pt);
        if (!res.has_value()) {
            m_error.setCode(ErrorCode::InterfaceEncMsgEncError);
            m_error.setDetailText(std::string("Encryption process: ") + Encryption::getEncryptionErrorText());
            return {};
        }

        auto tmpPart = Encryption::encodeHex(*res);
        if (tmpPart.empty()) {
            m_error.setCode(ErrorCode::InterfaceEncMsgEncError);
            m_error.setDetailText(std::string("Result encoding: ") + Encryption::getEncryptionErrorText());
            return {};
        }
        resH += tmpPart;
    }
    return resH;
}

std::optional<std::string> EncryptedExchangeMaster::decrypt(const std::string &encStr) const
{
    auto decodedInput = Encryption::decodeHex(encStr);
    auto splittedData = ExtraClasses::DataInfo::split(decodedInput, 256); // 256 is RSA block size

    std::string res;
    for (auto& pt : splittedData) {
        auto decPt = Encryption::rsaDecryptString(d->pkey, pt);
        if (!decPt.has_value()) {
            m_error.setCode(ErrorCode::InterfaceEncMsgDecError);
            m_error.setDetailText(std::string("Decryption process: ") + Encryption::getEncryptionErrorText());
            return {};
        }
        res += *decPt;
    }
    return res;
}

}