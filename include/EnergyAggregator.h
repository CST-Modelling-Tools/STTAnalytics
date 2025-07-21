#ifndef ENERGYAGGREGATOR_H
#define ENERGYAGGREGATOR_H

#include "Photon.h"
#include "SurfaceMap.h"
#include <string>
#include <vector>
#include <map>

class EnergyAggregator
{
public:
    EnergyAggregator(const SurfaceMap& surfaceMap, double powerPerPhoton);

    void processPhoton(const Photon& photon);

    double getEnergy(const std::string& heliostat, const std::string& receiver) const;
    const std::map<std::pair<std::string, std::string>, double>& getEnergyMap() const;

    void writeCSV(const std::string& outputPath) const;

private:
    const SurfaceMap& m_surfaceMap;
    double m_powerPerPhoton;

    std::map<std::pair<std::string, std::string>, double> m_energyMap;
};

#endif // ENERGYAGGREGATOR_H