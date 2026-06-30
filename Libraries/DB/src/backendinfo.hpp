#pragma once

#include "recordobjects.hpp"

namespace DBRecords {

/**
 * @brief The BackendType enum Device type of backend runner
 */
enum BackendType : int
{
    Default = 0, // PC
    Android,
};

/**
 * @brief The BackendInfo class Backend info record
 * @note Last online time must be proceed using DB functions
 */
class BackendInfo : public Database::RecordBaseS
{
public:
    BackendInfo();

    // RecordBase interface
    /**
     * @throws std::invalid_argument if ID length is not 64
     */
    void setId(const std::string &id) noexcept(false) override;
    Database::record_t toRecord() const override;
    bool initFromRecord(const Database::record_t &iRecord) override;

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
    virtual bool fromJson(const std::string& iJson);

private:
    BackendType m_type {BackendType::Default};
    std::string m_ip;
    uint16_t    m_port;
    std::string m_displayName;
    std::string m_token;
};

} // namespace DBRecords
