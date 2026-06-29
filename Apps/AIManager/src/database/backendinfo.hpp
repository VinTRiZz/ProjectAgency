#pragma once

#include <array>
#include <optional>

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

private:
    BackendType m_type {BackendType::Default};
    std::string m_ip;
    uint16_t    m_port;
    std::optional<int64_t> m_modelRole;
};

} // namespace DBRecords
