#pragma once

#include <memory>
#include <filesystem>
#include <vector>
#include <set>

#include <Components/ExtraClasses/Containers/TreeObject.h>

namespace FS {

/**
 * @brief The ObjectType enum   Type of an object
 */
enum class ObjectType : int
{
    Unknown = -1, // All other than known files are restricted for using
    Directory,
    RegularFile,
    Reference,
};

class Object;
using ObjectPtr = std::shared_ptr<FS::Object>;

/**
 * @brief The PathMetadata class Configuration of path for using in context
 */
struct PathMetadata
{
    std::string label;
    std::string description;
    std::set<std::string> keywords;
};

/**
 * @brief The Object class  Directory, file, reference, etc.
 */
class Object
{
public:
    Object() = default;
    explicit Object(const std::filesystem::path& fpath);

    void setRoot(const std::filesystem::path& fpath);
    std::filesystem::path getRoot() const;

    bool setPath(const std::filesystem::path& fpath);
    std::filesystem::path getPath() const;

    std::vector<std::string> getEntries(const std::string& filterStr) const;

    ObjectType getType() const;
    PathMetadata& getMetadata();
    const PathMetadata& getMetadata() const;

private:
    ObjectType              m_type {ObjectType::Unknown};
    std::filesystem::path   m_root {std::filesystem::current_path().root_path()};
    std::filesystem::path   m_path;
    PathMetadata            m_metadata;
};

} // namespace FS
