#pragma once

#include <string>

namespace DataObjects
{

class SerializableObject
{
public:
    virtual ~SerializableObject() = default;

    virtual std::string toJson() const = 0;
    virtual bool readJson(const std::string_view& iString) = 0;
};

}