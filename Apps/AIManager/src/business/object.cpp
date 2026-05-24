#include "object.hpp"

#include <Components/Filework/Common.h>
#include <Components/Logger/Logger.h>

namespace FS {

Object::Object(const std::filesystem::path &fpath)
{
    setPath(fpath);
}

void Object::setRoot(const std::filesystem::path &fpath)
{
    auto fpathAbs = std::filesystem::absolute(fpath);
    if (!std::filesystem::exists(fpathAbs) || !std::filesystem::is_directory(fpathAbs)) {
        COMPLOG_WARNING("FS::Object: Invalid root dir to set:", fpath);
        return;
    }
    m_root = fpath;
    if (0 != std::filesystem::absolute(m_path).u8string().find(m_root.u8string())) {
        setPath(m_root);
    }
}

std::filesystem::path Object::getRoot() const
{
    return m_root;
}

bool Object::setPath(const std::filesystem::path &fpath)
{
    if (!std::filesystem::exists(fpath) || (0 != std::filesystem::absolute(fpath).u8string().find(m_root.u8string()))) {
        COMPLOG_WARNING("FS::Object: Detected try to cd out of root dir");
        return false;
    }
    if (std::filesystem::is_directory(fpath)) {
        m_type = ObjectType::Directory;
    } else if (std::filesystem::is_symlink(fpath)) {
        m_type = ObjectType::Reference;
    } else if (std::filesystem::is_regular_file(fpath)) {
        m_type = ObjectType::RegularFile;
    } else {
        m_type = ObjectType::Unknown;
    }
    m_path = fpath;
    return true;
}

std::filesystem::path Object::getPath() const
{
    return m_path;
}

std::vector<std::string> Object::getEntries(const std::string &filterStr) const
{
    return Filework::Common::getContentNames(m_path, filterStr);
}

ObjectType Object::getType() const
{
    return m_type;
}

PathMetadata &Object::getMetadata()
{
    return m_metadata;
}

const PathMetadata &Object::getMetadata() const
{
    return m_metadata;
}

} // namespace FS
