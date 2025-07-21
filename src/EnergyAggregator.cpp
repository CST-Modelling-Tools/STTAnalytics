#include "EnergyAggregator.h"
#include <fstream>
#include <iomanip>
#include <stdexcept>

EnergyAggregator::EnergyAggregator(const SurfaceMap& surfaceMap, double powerPerPhoton)
    : m_surfaceMap(surfaceMap), m_powerPerPhoton(powerPerPhoton)
{}

void EnergyAggregator::processPhoton(const Photon& photon)
{
    uint64_t surfaceId = photon.m_surfaceId;
    uint64_t nextId = photon.m_nextId;

    if (!m_surfaceMap.isHeliostat(surfaceId)) return;
    if (!m_surfaceMap.isReceiver(nextId)) return;

    std::string heliostat = m_surfaceMap.getHeliostatName(surfaceId);
    std::string receiver  = m_surfaceMap.getReceiverName(nextId);

    std::pair<std::string, std::string> key = {heliostat, receiver};
    m_energyMap[key] += m_powerPerPhoton;
}

double EnergyAggregator::getEnergy(const std::string& heliostat, const std::string& receiver) const
{
    auto it = m_energyMap.find({heliostat, receiver});
    return (it != m_energyMap.end()) ? it->second : 0.0;
}

const std::map<std::pair<std::string, std::string>, double>& EnergyAggregator::getEnergyMap() const
{
    return m_energyMap;
}

void EnergyAggregator::writeCSV(const std::string& outputPath) const
{
    std::ofstream outFile(outputPath);
    if (!outFile)
        throw std::runtime_error("Cannot write CSV file: " + outputPath);

    outFile << "Heliostat,Receiver,Energy_W\n";
    for (const auto& [key, energy] : m_energyMap)
    {
        outFile << key.first << "," << key.second << "," << std::fixed << std::setprecision(6) << energy << "\n";
    }

    outFile.close();
}