#ifndef SURFACE_MAP_H
#define SURFACE_MAP_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

class SurfaceMap
{
public:
    SurfaceMap(const std::unordered_map<uint64_t, std::string>& surfaceData);

    bool isHeliostat(uint64_t surfaceId) const;
    bool isReceiver(uint64_t surfaceId) const;

    std::string getReceiverName(uint64_t surfaceId) const;
    std::string getHeliostatName(uint64_t surfaceId) const;

    size_t getReceiverCount() const;
    size_t getHeliostatCount() const;
    size_t getTotalSurfaceCount() const;

    std::vector<std::string> getReceiverNames() const;

private:
    std::unordered_map<uint64_t, std::string> m_surfacePaths;
    std::unordered_map<uint64_t, std::string> m_heliostatNames;
    std::unordered_map<uint64_t, std::string> m_receiverNames;

    std::string extractHeliostatName(const std::string& path) const;
    std::string extractReceiverName(const std::string& path) const;
};

#endif // SURFACE_MAP_H