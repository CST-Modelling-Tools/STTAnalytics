#include "ParametersFileReader.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// --- helpers ---

std::string ParametersFileReader::trim(const std::string& s)
{
    std::size_t b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

// normalize: lowercase + remove spaces/underscores to compare ids tolerant of formatting
std::string ParametersFileReader::normalizeId(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for (unsigned char ch : s) {
        if (ch == ' ' || ch == '_') continue;
        out.push_back(static_cast<char>(std::tolower(ch)));
    }
    return out;
}

// --- public ---

ParametersFileReader::ParametersFileReader(const std::string& folderPath)
    : m_folderPath(folderPath)
{}

void ParametersFileReader::read()
{
    m_surfaceMap.clear();
    m_powerPerPhoton = 0.0;

    fs::path fullPath = fs::path(m_folderPath) / "photons_parameters.txt";
    std::ifstream file(fullPath);
    if (!file.is_open())
        throw std::runtime_error("Unable to open parameters file: " + fullPath.string());

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line.rfind("#", 0) == 0) continue; // skip blank/comments

        if (line == "START PARAMETERS")
        {
            parseParameterBlock(file);
        }
        else if (line == "START SURFACES")
        {
            parseSurfaceBlock(file);
            // After END SURFACES, search remaining lines for the last numeric token (power per photon)
            parsePowerAfterSurfaces(file);
            break; // we’re done
        }
        // else: ignore other lines before first block
    }

    if (m_surfaceMap.empty())
        std::cerr << "Warning: no surfaces parsed from photons_parameters.txt\n";
    if (m_powerPerPhoton <= 0.0)
        std::cerr << "Warning: power per photon not found or non-positive.\n";
}

// --- blocks ---

void ParametersFileReader::parseParameterBlock(std::ifstream& file)
{
    std::vector<std::string> parameterNames;
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line == "END PARAMETERS") break;
        if (!line.empty() && line[0] != '#')
            parameterNames.push_back(line);
    }

    if (!matchesExpectedParameterList(parameterNames))
        throw std::runtime_error("Photon parameter list in file does not match the expected structure.");
}

bool ParametersFileReader::matchesExpectedParameterList(const std::vector<std::string>& actual)
{
    // Expected order in Tonatiuh++ photon .dat
    const std::vector<std::string> expected = {
        "id", "x", "y", "z", "side", "previous ID", "next ID", "surface ID"
    };

    if (actual.size() != expected.size())
        return false;

    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        if (normalizeId(actual[i]) != normalizeId(expected[i]))
            return false;
    }
    return true;
}

void ParametersFileReader::parseSurfaceBlock(std::ifstream& file)
{
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line == "END SURFACES") break;
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        uint64_t surfaceId;
        std::string surfacePath;
        if (iss >> surfaceId >> std::ws && std::getline(iss, surfacePath))
        {
            m_surfaceMap[surfaceId] = trim(surfacePath);
        }
    }
}

void ParametersFileReader::parsePowerAfterSurfaces(std::ifstream& file)
{
    // After END SURFACES, Tonatiuh++ often writes the power per photon on the last line.
    // Be tolerant: skip blanks/comments; if the line has labels, extract the last numeric token.
    std::string line, lastNonEmpty;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        lastNonEmpty = line;
    }
    if (lastNonEmpty.empty())
        throw std::runtime_error("Failed to read power per photon: no data after END SURFACES.");

    // Extract the last numeric token from the line
    std::istringstream iss(lastNonEmpty);
    std::string token, lastToken;
    while (iss >> token) lastToken = token;

    try {
        // stod tolerates leading/trailing spaces; token should be numeric (possibly with exponent)
        m_powerPerPhoton = std::stod(lastToken);
    } catch (...) {
        // If the entire line is just a number with locale commas, try to replace ',' with '.'
        std::string canon = lastToken;
        std::replace(canon.begin(), canon.end(), ',', '.');
        try {
            m_powerPerPhoton = std::stod(canon);
        } catch (...) {
            throw std::runtime_error("Failed to parse power per photon from line: \"" + lastNonEmpty + "\"");
        }
    }
}

// --- getters ---

const std::unordered_map<uint64_t, std::string>& ParametersFileReader::getSurfaceMap() const
{
    return m_surfaceMap;
}

double ParametersFileReader::getPowerPerPhoton() const
{
    return m_powerPerPhoton;
}