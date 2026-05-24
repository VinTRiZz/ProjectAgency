#include "fscontext.hpp"

#include <Components/Logger/Logger.h>
#include <Components/Filework/Common.h>
#include <Components/Filework/TemporaryFile.h>
#include <Components/Encryption/Encoding.h>

#include <nlohmann/json.hpp>

namespace FS {

bool FS::Context::save(const std::filesystem::path &saveFile)
{
    nlohmann::json res;
    res["root"] = Encryption::encodeHex(m_rootdir);
    auto& objects = res["objects"];
    for (const auto& [_, pObj] : m_knownObjects) {
        nlohmann::json objData;
        objData["path"] = Encryption::encodeHex(pObj->getPath());

        auto& mdata = pObj->getMetadata();
        objData["label"] = Encryption::encodeHex(mdata.label);
        objData["description"] = Encryption::encodeHex(mdata.description);
        auto& kwords = objData["keywords"];
        for (auto& kw : mdata.keywords) {
            kwords.push_back(Encryption::encodeHex(kw));
        }

        objects.push_back(objData);
    }
    try {
        Filework::TemporaryFile tmpf(saveFile);
        tmpf << res.dump();
        tmpf.accept();
        return true;
    } catch (const std::exception& ex) {
        COMPLOG_WARNING("FS::Context: Failed to save file:", ex.what());
    }
    return false;
}

bool FS::Context::load(const std::filesystem::path &saveFile)
{
    std::string fileData;
    if (!Filework::Common::readFileData(saveFile, fileData)) {
        COMPLOG_WARNING("FS::Context: Failed to load file");
        return false;
    }

    try {
        auto parsedContext = nlohmann::json::parse(fileData);
        m_rootdir = std::string(parsedContext["root"]);

        for (const auto& objData : parsedContext["objects"]) {
            auto fpath = Encryption::decodeHex(objData["path"]);

            PathMetadata mdata;
            mdata.label = Encryption::decodeHex(objData["label"]);
            mdata.label = Encryption::decodeHex(objData["description"]);
            for (auto& kw : objData["keywords"]) {
                mdata.keywords.insert(Encryption::decodeHex(kw));
            }

            auto& pObj = m_knownObjects[mdata.label];
            pObj = std::make_shared<Object>();
            pObj->setRoot(m_rootdir);
            pObj->setPath(fpath);
            pObj->getMetadata() = mdata;
        }

    } catch (const std::exception& ex) {
        COMPLOG_WARNING("FS::Context: Failed to parse file data:", ex.what());
    }
    return false;
}

void FS::Context::setContextRoot(const std::filesystem::path &rootDir)
{
    if (!std::filesystem::exists(rootDir)) {
        COMPLOG_WARNING("FS::Context: Invalid root dir to set:", rootDir);
        return;
    }
    m_rootdir = rootDir;
    for (auto& [_, pObj] : m_knownObjects) {
        pObj->setRoot(m_rootdir);
    }
}

std::filesystem::path FS::Context::getRoot() const
{
    return m_rootdir;
}

void FS::Context::addKnownObject(const Object &obj)
{
    m_knownObjects[obj.getMetadata().label] = std::make_shared<Object>(std::move(obj));
}

FS::ObjectPtr FS::Context::getObject(const std::string &label) const
{
    auto objIt = m_knownObjects.find(label);
    if (m_knownObjects.end() != objIt) {
        return objIt->second;
    }
    return {};
}

void FS::Context::removeKnownObject(const std::string &objectLabel)
{
    m_knownObjects.erase(objectLabel);
}

} // namespace FS
