#pragma once

#include <ProjectAgency/Exchange/Error.h>
#include "serializableobject.hpp"

namespace Exchange {

struct ObjectSetting : public SerializableObject,
                      public ErrorUser
{
    std::string m_name;
    std::string m_value;

    // SerializableObject interface
    std::string toJson() const;
    bool readJson(const std::string_view &iString);
};

}