#pragma once

#include <ProjectAgency/Exchange/Error.h>
#include "serializableobject.hpp"

namespace Exchange {

class ObjectSetting : public SerializableObject,
                      public ErrorUser
{

};

}