#pragma once

#include <string>
#include <stdint.h>

#include "serializableobject.hpp"

namespace DataObjects {

/**
 * @brief The BackendDisplayInfo class Info to display in GUI
 */
struct BackendDisplayInfo : public SerializableObject
{
    // Infrastructure fields
    bool        isOnline {false};
    std::string name     {"Unnamed"};

    // Network configuration
    std::string ip;
    uint16_t    port {};

    // AI fields
    std::string model;
    std::string sysprompt;
    std::string currentPrompt;

    // SerializableObject interface
    std::string toJson() const override;
    bool readJson(const std::string_view &iString) override;
};

}