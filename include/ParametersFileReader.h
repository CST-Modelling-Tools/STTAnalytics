#ifndef PARAMETERSFILEREADER_H
#define PARAMETERSFILEREADER_H

#include <string>
#include <unordered_map>
#include <cstdint>

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

    static bool matchesExpectedParameterList(const std::vector<std::string>& actual);
};

#endif // PARAMETERSFILEREADER_H