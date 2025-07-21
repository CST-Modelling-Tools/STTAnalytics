#include "PhotonProcessor.h"
#include "Photon.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <cstring>
#include <iomanip>

namespace fs = std::filesystem;

PhotonProcessor::PhotonProcessor(const std::string& folderPath, const SurfaceMap& surfaceMap)
    : folderPath(folderPath), surfaceMap(surfaceMap), totalPhotons(0)
{}

void PhotonProcessor::processPhotons(const std::string& outputCsvFile)
{
    std::map<std::pair<std::string, std::string>, double> energyByPair;

    std::vector<std::string> photonFiles = getSortedPhotonFiles();
    Photon prevPhoton{};

    for (const auto& filePath : photonFiles)
    {
        std::ifstream file(filePath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Could not open file: " + filePath);

        while (file.peek() != EOF)
        {
            Photon photon;
            photon.id         = static_cast<uint64_t>(readBigEndianDouble(file));
            photon.x          = readBigEndianDouble(file);
            photon.y          = readBigEndianDouble(file);
            photon.z          = readBigEndianDouble(file);
            photon.side       = static_cast<int>(readBigEndianDouble(file));
            photon.previousId = static_cast<uint64_t>(readBigEndianDouble(file));
            photon.nextId     = static_cast<uint64_t>(readBigEndianDouble(file));
            photon.surfaceId  = static_cast<uint64_t>(readBigEndianDouble(file));

            ++totalPhotons;
            if (totalPhotons % 10000000 == 0)
            {
                std::cout.imbue(std::locale(""));
                std::cout << "Processed " << totalPhotons << " photons..." << std::endl;
            }

            if (photon.nextId == 0 && surfaceMap.isReceiver(photon.surfaceId))
            {
                auto it = photonById.find(photon.previousId);
                if (it != photonById.end())
                {
                    const Photon& prev = it->second;
                    if (surfaceMap.isHeliostat(prev.surfaceId))
                    {
                        std::string receiverName = surfaceMap.getReceiverName(photon.surfaceId);
                        std::string heliostatName = surfaceMap.getHeliostatName(prev.surfaceId);
                        energyByPair[{receiverName, heliostatName}] += 1.0; // Each ray has unit energy
                    }
                }
            }

            photonById[photon.id] = photon;
        }
    }

    std::ofstream outFile(outputCsvFile);
    outFile << "Receiver,Heliostat,Energy\n";
    for (const auto& [pair, energy] : energyByPair)
    {
        outFile << pair.first << "," << pair.second << "," << energy << "\n";
    }

    std::cout.imbue(std::locale(""));
    std::cout << "Finished streaming. Total photons: " << totalPhotons << std::endl;
    std::cout << "CSV file written to: " << outputCsvFile << std::endl;
}

double PhotonProcessor::readBigEndianDouble(std::ifstream& stream) const
{
    uint8_t buffer[8];
    stream.read(reinterpret_cast<char*>(buffer), 8);

    if (stream.gcount() != 8)
        throw std::runtime_error("Unexpected end of file or read error while reading a double.");

    uint64_t intVal = 0;
    for (int i = 0; i < 8; ++i)
    {
        intVal = (intVal << 8) | buffer[i];
    }

    double result;
    std::memcpy(&result, &intVal, sizeof(result));

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&result);
    std::reverse(bytes, bytes + 8);
#endif

    return result;
}

std::vector<std::string> PhotonProcessor::getSortedPhotonFiles() const
{
    std::vector<std::pair<int, std::string>> indexedFiles;
    std::regex pattern(R"(photons_(\d+)\.dat)");

    for (const auto& entry : fs::directory_iterator(folderPath))
    {
        if (!entry.is_regular_file())
            continue;

        const std::string filename = entry.path().filename().string();
        std::smatch match;
        if (std::regex_match(filename, match, pattern))
        {
            int index = std::stoi(match[1]);
            indexedFiles.emplace_back(index, entry.path().string());
        }
    }

    std::sort(indexedFiles.begin(), indexedFiles.end());

    std::vector<std::string> result;
    for (const auto& [_, path] : indexedFiles)
        result.push_back(path);

    return result;
}