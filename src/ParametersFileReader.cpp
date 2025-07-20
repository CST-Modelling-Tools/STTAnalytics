#include "ParametersFileReader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

ParametersFileReader::ParametersFileReader(const std::string& folderPath)
    : m_folderPath(folderPath)
{}

void ParametersFileReader::read()
{
    fs::path fullPath = fs::path(m_folderPath) / "photons_parameters.txt";
    std::ifstream file(fullPath);
    if (!file.is_open())
        throw std::runtime_error("Unable to open parameters file: " + fullPath.string());

    std::string line;
    while (std::getline(file, line))
    {
        if (line == "START PARAMETERS")
        {
            parseParameterBlock(file);
        }
        else if (line == "START SURFACES")
        {
            parseSurfaceBlock(file);
        }
        else if (line.rfind("END SURFACES", 0) == 0)
        {
            if (!(file >> m_powerPerPhoton))
                throw std::runtime_error("Failed to read power per photon.");
            break;
        }
    }
}

void ParametersFileReader::parseParameterBlock(std::ifstream& file)
{
    std::vector<std::string> parameterNames;
    std::string line;
    while (std::getline(file, line) && line != "END PARAMETERS")
    {
        if (!line.empty())
            parameterNames.push_back(line);
    }

    if (!matchesExpectedParameterList(parameterNames))
        throw std::runtime_error("Photon parameter list in file does not match the expected structure.");
}

bool ParametersFileReader::matchesExpectedParameterList(const std::vector<std::string>& actual)
{
    const std::vector<std::string> expected = {
        "id", "x", "y", "z", "side", "previous ID", "next ID", "surface ID"
    };

    if (actual.size() != expected.size())
        return false;

    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (actual[i] != expected[i])
            return false;
    }
    return true;
}

void ParametersFileReader::parseSurfaceBlock(std::ifstream& file)
{
    std::string line;
    while (std::getline(file, line) && line != "END SURFACES")
    {
        std::istringstream iss(line);
        uint64_t surfaceId;
        std::string surfacePath;
        if (iss >> surfaceId >> std::ws && std::getline(iss, surfacePath))
        {
            m_surfaceMap[surfaceId] = surfacePath;
        }
    }
}

const std::unordered_map<uint64_t, std::string>& ParametersFileReader::getSurfaceMap() const
{
    return m_surfaceMap;
}

double ParametersFileReader::getPowerPerPhoton() const
{
    return m_powerPerPhoton;
}