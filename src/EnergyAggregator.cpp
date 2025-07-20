#include "EnergyAggregator.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>

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

void EnergyAggregator::printRanking() const
{
    std::vector<std::tuple<std::string, std::string, double>> ranking;
    for (const auto& [key, energy] : m_energyMap)
    {
        ranking.emplace_back(key.first, key.second, energy);
    }

    std::sort(ranking.begin(), ranking.end(),
              [](const auto& a, const auto& b) {
                  return std::get<2>(a) > std::get<2>(b);
              });

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Heliostat   Receiver   Energy Sent [W]\n";
    std::cout << "---------------------------------------\n";

    for (const auto& [heliostat, receiver, energy] : ranking)
    {
        std::cout << std::setw(10) << heliostat << "   "
                  << std::setw(9) << receiver << "   "
                  << std::setw(12) << energy << '\n';
    }
}