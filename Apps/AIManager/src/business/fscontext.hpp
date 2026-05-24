#pragma once

#include <string>
#include <map>

#include <expected>

#include "object.hpp"

namespace FS {

/**
 * @brief The Context class Filesystem context, for example, project directory
 */
class Context
{
public:
    /**
     * @brief save      Save data in JSON format into a file
     * @param saveFile
     * @return          false if failed
     */
    bool save(const std::filesystem::path& saveFile);

    /**
     * @brief load      Load context from a file. Invalidates context
     * @param saveFile
     * @return          false if failed
     */
    bool load(const std::filesystem::path& saveFile);

    /**
     * @brief setContextRoot Set root directory of all objects
     * @param rootDir
     */
    void setContextRoot(const std::filesystem::path& rootDir);
    std::filesystem::path getRoot() const;

    /**
     * @brief addKnownObject Add object as known
     * @param obj
     */
    void addKnownObject(const Object& obj);

    /**
     * @brief getObject Get object by it's label
     * @param label
     * @return          nullptr if object not found
     */
    ObjectPtr getObject(const std::string& label) const;

    /**
     * @brief removeKnownObject Remove object, if exist
     * @param objectLabel       Label of object
     */
    void removeKnownObject(const std::string& objectLabel);

private:
    std::filesystem::path m_rootdir {"/"};
    std::map<std::string, ObjectPtr> m_knownObjects;
};

} // namespace FS
