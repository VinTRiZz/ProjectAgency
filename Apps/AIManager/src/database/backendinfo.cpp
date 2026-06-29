#include "backendinfo.hpp"

#include <stdexcept>

namespace DBRecords {

BackendInfo::BackendInfo() :
    Database::RecordBaseS("sch_manager.t_backends", "device") {

}

void BackendInfo::setId(const std::string &id) noexcept(false)
{
    if (id.size() != 64) {
        throw std::invalid_argument("Invalid id length (expected 64 symbols)");
    }
    Database::RecordBaseS::setId(id);
}

Database::record_t BackendInfo::toRecord() const
{
    auto res = Database::RecordBaseS::toRecord();

    // init

    return res;
}

bool BackendInfo::initFromRecord(const Database::record_t &iRecord)
{
    auto initRes = Database::RecordBaseS::initFromRecord(iRecord);
    if (!initRes) {
        return false;
    }

    // init
    return false;
}

} // namespace DBRecords
