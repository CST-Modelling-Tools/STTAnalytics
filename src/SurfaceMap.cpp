#include "SurfaceMap.h"
#include <regex>
#include <stdexcept>

SurfaceMap::SurfaceMap(const std::unordered_map<uint64_t, std::string>& surfaceIdToPath)
    : m_surfaceIdToPath(surfaceIdToPath)
{
    extractNames();
}

void SurfaceMap::extractNames()
{
    for (const auto& [id, path] : m_surfaceIdToPath)
    {
        if (path.find("/HeliostatField/Heliostats/") != std::string::npos)
        {
            std::string name = extractNameAfter(path, "/Heliostats/");
            m_heliostatNames[id] = name;
            m_allHeliostatNames.insert(name);
        }
        else if (path.find("/Receivers/") != std::string::npos)
        {
            std::string name = extractNameAfter(path, "/Receivers/");
            m_receiverNames[id] = name;
            m_allReceiverNames.insert(name);
        }
    }
}

std::string SurfaceMap::extractNameAfter(const std::string& path, const std::string& keyword) const
{
    size_t start = path.find(keyword);
    if (start == std::string::npos)
        return {};

    start += keyword.length();
    size_t end = path.find('/', start);
    if (end == std::string::npos)
        end = path.length();

    return path.substr(start, end - start);
}

bool SurfaceMap::isHeliostat(uint64_t surfaceId) const
{
    return m_heliostatNames.find(surfaceId) != m_heliostatNames.end();
}

bool SurfaceMap::isReceiver(uint64_t surfaceId) const
{
    return m_receiverNames.find(surfaceId) != m_receiverNames.end();
}

std::string SurfaceMap::getHeliostatName(uint64_t surfaceId) const
{
    auto it = m_heliostatNames.find(surfaceId);
    return it != m_heliostatNames.end() ? it->second : "";
}

std::string SurfaceMap::getReceiverName(uint64_t surfaceId) const
{
    auto it = m_receiverNames.find(surfaceId);
    return it != m_receiverNames.end() ? it->second : "";
}

const std::unordered_set<std::string>& SurfaceMap::getAllHeliostatNames() const
{
    return m_allHeliostatNames;
}

const std::unordered_set<std::string>& SurfaceMap::getAllReceiverNames() const
{
    return m_allReceiverNames;
}