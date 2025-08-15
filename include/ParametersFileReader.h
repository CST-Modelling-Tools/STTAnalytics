#ifndef PARAMETERSFILEREADER_H
#define PARAMETERSFILEREADER_H

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>  // needed for std::ifstream in declarations

class ParametersFileReader
{
public:
    explicit ParametersFileReader(const std::string& folderPath);

    void read();

    const std::unordered_map<uint64_t, std::string>& getSurfaceMap() const;
    double getPowerPerPhoton() const;

private:
    std::string m_folderPath;
    std::unordered_map<uint64_t, std::string> m_surfaceMap;
    double m_powerPerPhoton = 0.0;

    void parseParameterBlock(std::ifstream& file);
    void parseSurfaceBlock(std::ifstream& file);
    void parsePowerAfterSurfaces(std::ifstream& file);

    static bool matchesExpectedParameterList(const std::vector<std::string>& actual);

    // helpers
    static std::string trim(const std::string& s);
    static std::string normalizeId(const std::string& s); // for parameter-name comparison
};

#endif // PARAMETERSFILEREADER_H