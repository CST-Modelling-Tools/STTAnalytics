#ifndef SURFACEMAP_H
#define SURFACEMAP_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>

class SurfaceMap
{
public:
    explicit SurfaceMap(const std::unordered_map<uint64_t, std::string>& surfaceIdToPath);

    bool isHeliostat(uint64_t surfaceId) const;
    bool isReceiver(uint64_t surfaceId) const;

    std::string getHeliostatName(uint64_t surfaceId) const;
    std::string getReceiverName(uint64_t surfaceId) const;

    const std::unordered_set<std::string>& getAllHeliostatNames() const;
    const std::unordered_set<std::string>& getAllReceiverNames() const;

private:
    std::unordered_map<uint64_t, std::string> m_surfaceIdToPath;
    std::unordered_map<uint64_t, std::string> m_heliostatNames;
    std::unordered_map<uint64_t, std::string> m_receiverNames;

    std::unordered_set<std::string> m_allHeliostatNames;
    std::unordered_set<std::string> m_allReceiverNames;

    void extractNames();
    std::string extractNameAfter(const std::string& path, const std::string& keyword) const;
};

#endif // SURFACEMAP_H