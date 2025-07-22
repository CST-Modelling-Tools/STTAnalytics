#ifndef PHOTONPROCESSOR_H
#define PHOTONPROCESSOR_H

#include "Photon.h"
#include "SurfaceMap.h"
#include "tonatiuhreader.h"

#include <string>
#include <map>
#include <vector>
#include <unordered_map>

class PhotonProcessor
{
public:
    PhotonProcessor(const std::string& folderPath, const SurfaceMap& surfaceMap, double powerPerPhoton);
    void processPhotons(const std::string& outputCsvFile);

private:
    std::string folderPath;
    const SurfaceMap& surfaceMap;
    double powerPerPhoton;
    uint64_t totalPhotons = 0;

    std::unordered_map<uint64_t, Photon> photonById;
};

#endif // PHOTONPROCESSOR_H