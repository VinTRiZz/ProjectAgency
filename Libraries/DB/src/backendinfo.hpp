#pragma once

#include "recordobjects.hpp"

#include <memory>

namespace DBRecords {

/**
 * @brief The AIBackendDeviceType enum Device type of backend runner
 */
enum AIBackendDeviceType : int
{
    Default = 0, // PC
    Android,

    SYS_Devtype_max // To check if type is valid
};

class AIBackendInfo;
using AIBackendInfoPtr = std::shared_ptr<AIBackendInfo>;

/**
 * @brief The AIBackendInfo class Backend info record
 * @note Last online time must be proceed using DB functions
 */
class AIBackendInfo : public Database::RecordBaseS
{
public:
    AIBackendInfo();

    // RecordBase interface
    /**
     * @throws std::invalid_argument if ID length is not 64
     */
    void setId(const std::string &id) noexcept(false) override;
    Database::record_t toRecord() const override;
    bool initFromRecord(const Database::record_t &iRecord) override;

    void setType(AIBackendDeviceType typ);
    AIBackendDeviceType getType() const;

    void setIp(const std::string& ip);
    std::string getIp() const;

    void setPort(const uint16_t &port);
    int64_t getPort() const;

    void setDisplayName(const std::string& displayName);
    std::string getDisplayName() const;

    /**
     * @throws std::invalid_argument if ID length is not 64
     * @note Token is not saved to DB. Get it from app settings
     */
    void setToken(const std::string& token);
    std::string getToken() const;

    std::string getFullAddress() const;

    // For editting from user's GUI
    virtual std::string toJson() const;
    virtual bool readJson(const std::string& iJson);

private:
    AIBackendDeviceType m_type {AIBackendDeviceType::Default};
    std::string m_ip;
    uint16_t    m_port;
    std::string m_displayName;
    std::string m_token;
};

} // namespace DBRecords
