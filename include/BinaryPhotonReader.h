#ifndef BINARYPHOTONREADER_H
#define BINARYPHOTONREADER_H

#include "Photon.h"
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <cstdint>
#include <algorithm>

class BinaryPhotonReader
{
public:
    explicit BinaryPhotonReader(const std::string& folderPath)
        : m_folderPath(folderPath) {}

    template <typename PhotonCallback>
    void streamPhotons(PhotonCallback&& callback) const
    {
        std::vector<std::string> photonFiles = getPhotonFiles();

        for (const auto& filepath : photonFiles)
        {
            readPhotonsFromFile(filepath, std::forward<PhotonCallback>(callback));
        }
    }

private:
    std::string m_folderPath;

    std::vector<std::string> getPhotonFiles() const
    {
        std::vector<std::pair<int, std::string>> indexedFiles;
        std::regex pattern(R"(photons_(\d+)\.dat)");

        for (const auto& entry : std::filesystem::directory_iterator(m_folderPath))
        {
            if (!entry.is_regular_file()) continue;

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

    template <typename PhotonCallback>
    void readPhotonsFromFile(const std::string& filepath, PhotonCallback&& callback) const
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Unable to open binary file: " + filepath);

        while (file.peek() != EOF)
        {
            Photon p;
            p.m_id         = static_cast<uint64_t>(readBigEndianDouble(file));
            p.m_x          = readBigEndianDouble(file);
            p.m_y          = readBigEndianDouble(file);
            p.m_z          = readBigEndianDouble(file);
            p.m_side       = static_cast<int>(readBigEndianDouble(file));
            p.m_previousId = static_cast<uint64_t>(readBigEndianDouble(file));
            p.m_nextId     = static_cast<uint64_t>(readBigEndianDouble(file));
            p.m_surfaceId  = static_cast<uint64_t>(readBigEndianDouble(file));

            callback(p);
        }
    }

    double readBigEndianDouble(std::ifstream& stream) const
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
};

#endif // BINARYPHOTONREADER_H