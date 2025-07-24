#include "PhotonProcessor.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cstring>

namespace fs = std::filesystem;

static double readBigEndianDouble(std::ifstream& file)
{
    char buffer[8];
    file.read(buffer, 8);
    uint64_t temp = 0;
    for (int i = 0; i < 8; ++i)
        temp = (temp << 8) | static_cast<unsigned char>(buffer[i]);

    double result;
    std::memcpy(&result, &temp, sizeof(double));
    return result;
}

PhotonProcessor::PhotonProcessor(const std::string& folderPath_, const SurfaceMap& surfaceMap_, double powerPerPhoton_)
    : folderPath(folderPath_), surfaceMap(surfaceMap_), powerPerPhoton(powerPerPhoton_)
{
}

void PhotonProcessor::processPhotons(const std::string& outputCsvFile)
{
    std::vector<std::string> photonFiles;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            if (name.rfind("photons_", 0) == 0 &&
                name.size() >= 4 && name.compare(name.size() - 4, 4, ".dat") == 0)
            {
                photonFiles.push_back(entry.path().string());
            }
        }
    }

    std::sort(photonFiles.begin(), photonFiles.end());

    std::map<std::string, std::map<std::string, double>> energyMap;

    uint64_t photonCounter = 0;
    uint64_t skipped = 0;
    uint64_t rayCounter = 0;
    std::vector<Photon> ray;

    for (const std::string& fileName : photonFiles)
    {
        std::cout << fileName << "\n";
        std::ifstream file(fileName, std::ios::binary);
        if (!file) {
            std::cerr << "Error opening file: " << fileName << "\n";
            continue;
        }

        while (file.peek() != EOF)
        {
            Photon photon;
            photon.id         = readBigEndianDouble(file);
            photon.x          = readBigEndianDouble(file);
            photon.y          = readBigEndianDouble(file);
            photon.z          = readBigEndianDouble(file);
            photon.side       = readBigEndianDouble(file);
            photon.previousId = readBigEndianDouble(file);
            photon.nextId     = readBigEndianDouble(file);
            photon.surfaceId  = readBigEndianDouble(file);

            ++photonCounter;
            ray.push_back(photon);

            if (photon.nextId == 0)
            {
                ++rayCounter;
                if (ray.size() >= 2)
                {
                    const Photon& penultimate = ray[ray.size() - 2];
                    const Photon& last = ray.back();

                    auto heliostatID = static_cast<uint64_t>(penultimate.surfaceId);
                    auto receiverID  = static_cast<uint64_t>(last.surfaceId);
                    auto sideFlag    = static_cast<uint32_t>(penultimate.side);

                    if (sideFlag == 1 &&
                        surfaceMap.isHeliostat(heliostatID) &&
                        surfaceMap.isReceiver(receiverID))
                    {
                        std::string heliostatName = surfaceMap.getHeliostatName(heliostatID);
                        std::string receiverName  = surfaceMap.getReceiverName(receiverID);
                        energyMap[heliostatName][receiverName] += powerPerPhoton;
                    }
                    else
                    {
                        ++skipped;
                    }
                }
                ray.clear();

                if (rayCounter % 1000000 == 0)
                    std::cout << "Processed " << rayCounter << " rays...\n";
            }
        }
    }

    std::cout << "Finished streaming. Total photons: " << photonCounter << "\n";
    std::cout << "  - Skipped photons with invalid segments: " << skipped << "\n";

    std::ofstream out(outputCsvFile);
    if (!out) {
        std::cerr << "Error writing CSV file.\n";
        return;
    }

    const std::vector<std::string> receivers = surfaceMap.getReceiverNames();
    out << "Heliostat Label";
    for (const std::string& rec : receivers)
        out << ", Power to " << rec;
    out << ", Total Power to Receivers\n";

    for (const auto& pair : energyMap)
    {
        const std::string& heliostat = pair.first;
        const std::map<std::string, double>& recMap = pair.second;

        out << heliostat;
        double total = 0.0;
        for (const std::string& rec : receivers)
        {
            double value = recMap.count(rec) ? recMap.at(rec) : 0.0;
            out << ", " << value;
            total += value;
        }
        out << ", " << total << "\n";
    }

    std::cout << "CSV file written to: " << outputCsvFile << "\n";
    std::cout << "Finished.\n";
}