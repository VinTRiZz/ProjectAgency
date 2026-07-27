#pragma once

#include <ProjectAgency/Exchange/Error.h>
#include "serializableobject.hpp"

namespace Exchange {

/**
 * @brief The DatabaseConfiguration class Configuration of db
 */
struct DatabaseConfiguration : public SerializableObject,
                              public ErrorUser
{
    std::string m_dbAddress;
    std::string m_dbName;
    uint16_t    m_dbPort;
    std::string m_dbUsername;
    std::string m_dbPassword;

    // SerializableObject interface
    std::string toJson() const override;
    bool readJson(const std::string_view &iString) override;

    // operators
    bool operator ==(const DatabaseConfiguration& conf) const;
    bool operator !=(const DatabaseConfiguration& conf) const;
};

} // namespace Exchange
