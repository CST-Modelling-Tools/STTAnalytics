#ifndef PHOTONPROCESSOR_H
#define PHOTONPROCESSOR_H

#include "SurfaceMap.h"
#include "tonatiuhreader.h"

#include <cstdint>
#include <string>
#include <map>

class PhotonProcessor
{
public:
    PhotonProcessor(const std::string& folderPath, const SurfaceMap& surfaceMap, double powerPerPhoton);
    void processPhotons(const std::string& outputCsvFile);

private:
    std::string folderPath;
    const SurfaceMap& surfaceMap;
    double powerPerPhoton;
    std::uint64_t totalPhotons = 0;
};

#endif // PHOTONPROCESSOR_H