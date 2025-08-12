#include "SurfaceMap.h"
#include <sstream>
#include <iostream>

SurfaceMap::SurfaceMap(const std::unordered_map<uint64_t, std::string>& surfaceData)
    : m_surfacePaths(surfaceData)
{
    for (const auto& [id, path] : surfaceData)
    {
        if (path.find("/Heliostats/") != std::string::npos)
        {
            m_heliostatNames[id] = extractHeliostatName(path);
        }
        else if (path.find("/Receivers/") != std::string::npos)
        {
            m_receiverNames[id] = extractReceiverName(path);
        }

     }

//    std::cout << "Registered Receivers:\n";
//    for (const auto& pair : m_receiverNames)
//        std::cout << "  ID " << pair.first << ": " << pair.second << "\n";

//    std::cout << "Registered Heliostats:\n";
//    for (const auto& pair : m_heliostatNames)
//        std::cout << "  ID " << pair.first << ": " << pair.second << "\n";
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
    return (it != m_heliostatNames.end()) ? it->second : "UnknownHeliostat";
}

std::string SurfaceMap::getReceiverName(uint64_t surfaceId) const
{
    auto it = m_receiverNames.find(surfaceId);
    return (it != m_receiverNames.end()) ? it->second : "UnknownReceiver";
}

size_t SurfaceMap::getReceiverCount() const
{
    return m_receiverNames.size();
}

size_t SurfaceMap::getHeliostatCount() const
{
    return m_heliostatNames.size();
}

size_t SurfaceMap::getTotalSurfaceCount() const
{
    return m_surfacePaths.size();
}

std::vector<std::string> SurfaceMap::getReceiverNames() const
{
    std::vector<std::string> names;
    for (const auto& [id, name] : m_receiverNames)
        names.push_back(name);
    return names;
}

std::string SurfaceMap::extractHeliostatName(const std::string& path) const
{
    std::stringstream ss(path);
    std::string segment;
    while (std::getline(ss, segment, '/'))
    {
        if (segment.size() >= 2 && segment[0] == 'H' && std::isdigit(segment[1]))
            return segment;
    }
    return "";  // not a heliostat
}

std::string SurfaceMap::extractReceiverName(const std::string& path) const
{
    std::istringstream stream(path);
    std::string token;
    std::string lastMatch = "UnknownReceiver";

    while (std::getline(stream, token, '/'))
    {
        if (token.find("Receiver") != std::string::npos)
            lastMatch = token;
    }

    return lastMatch;
}

const std::unordered_map<uint64_t, std::string>& SurfaceMap::getHeliostatNames() const
{
    return m_heliostatNames;
}

const std::unordered_map<uint64_t, std::string>& SurfaceMap::getReceiverNamesMap() const
{
    return m_receiverNames;
}
