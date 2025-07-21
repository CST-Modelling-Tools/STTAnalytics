#ifndef PHOTONPROCESSOR_H
#define PHOTONPROCESSOR_H

#include "Photon.h"
#include "SurfaceMap.h"

#include <string>
#include <map>
#include <vector>
#include <unordered_map>

class PhotonProcessor
{
public:
    PhotonProcessor(const std::string& folderPath, const SurfaceMap& surfaceMap);

    void processPhotons(const std::string& outputCsvFile);

private:
    std::string folderPath;
    const SurfaceMap& surfaceMap;
    uint64_t totalPhotons = 0;

    std::unordered_map<uint64_t, Photon> photonById;

    double readBigEndianDouble(std::ifstream& stream) const;
    std::vector<std::string> getSortedPhotonFiles() const;
};

#endif // PHOTONPROCESSOR_H