#pragma once

#include <Components/Database/RecordBase.h>

#include <ProjectAgency/Exchange/Error.h>

#include <memory>

namespace DBRecords {

class AIRole;
using AIRolePtr = std::shared_ptr<AIRole>;

/**
 * @brief The AIRole class Class, describing model role
 */
class AIRole : public Database::RecordBaseI
{
public:
    AIRole();

    // RecordBase interface
    Database::DBRowNamed toRecord() const override;
    bool initFromRecord(const Database::DBRowNamed &iRecord) override;

    void setVersion(unsigned version);
    unsigned getVersion() const;

    void setName(const std::string& name);
    std::string getName() const;

    void setType(const std::string& type);
    std::string getType() const;

    void setConfig(const std::string& configJson);
    std::string getConfig() const;

    // For editting from user's GUI
    virtual std::string toJson() const;
    virtual bool fromJson(const std::string& iJson);

private:
    unsigned m_version {};
    std::string m_name;
    std::string m_type;
    std::string m_configJson;
};

} // namespace DBRecords
