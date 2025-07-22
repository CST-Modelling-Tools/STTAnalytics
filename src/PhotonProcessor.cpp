#include "PhotonProcessor.h"
#include "Photon.h"

#include <fstream>
#include <iostream>
#include <set>
#include <iomanip>
#include <locale>

PhotonProcessor::PhotonProcessor(const std::string& folderPath, const SurfaceMap& surfaceMap, double powerPerPhoton)
    : folderPath(folderPath), surfaceMap(surfaceMap), powerPerPhoton(powerPerPhoton), totalPhotons(0)
{}

void PhotonProcessor::processPhotons(const std::string& outputCsvFile)
{
    std::unordered_map<std::string, std::unordered_map<std::string, double>> powerMap;
    std::set<std::string> receiverNamesSet;

    TonatiuhReader reader(folderPath);
    PhotonInfo info;

    while (reader.ReadPhotonInfo(info))
    {
        Photon photon;
        photon.id         = static_cast<uint64_t>(info.id);
        photon.x          = static_cast<float>(info.x);
        photon.y          = static_cast<float>(info.y);
        photon.z          = static_cast<float>(info.z);
        photon.side       = static_cast<uint32_t>(info.side);
        photon.previousId = static_cast<uint64_t>(info.previous_id);
        photon.nextId     = static_cast<uint64_t>(info.next_id);
        photon.surfaceId  = static_cast<uint64_t>(info.surface_id);

        ++totalPhotons;
        if (totalPhotons % 50000000 == 0)
        {
            std::cout.imbue(std::locale(""));
            std::cout << "Processed " << totalPhotons << " photons..." << std::endl;
        }

        if (photon.nextId == 0 && surfaceMap.isReceiver(photon.surfaceId))
        {
            if (photon.side != 1)
                throw std::runtime_error("Photon reached a receiver but side != 1");

            auto it = photonById.find(photon.previousId);
            if (it == photonById.end())
                continue;

            const Photon& prev = it->second;
            if (!surfaceMap.isHeliostat(prev.surfaceId))
                continue;

            std::string receiverName = surfaceMap.getReceiverName(photon.surfaceId);
            std::string heliostatName = surfaceMap.getHeliostatName(prev.surfaceId);
            powerMap[heliostatName][receiverName] += powerPerPhoton;
            receiverNamesSet.insert(receiverName);
        }

        photonById[photon.id] = photon;
    }

    std::vector<std::string> receiverNames(receiverNamesSet.begin(), receiverNamesSet.end());
    std::sort(receiverNames.begin(), receiverNames.end());

    std::ofstream outFile(outputCsvFile);
    outFile << "Heliostat";
    for (const auto& receiver : receiverNames)
        outFile << ",Power to " << receiver;
    outFile << ",Total Power to Receivers\n";

    for (const auto& [heliostat, receiverPowers] : powerMap)
    {
        outFile << heliostat;
        double totalPower = 0.0;
        for (const auto& receiver : receiverNames)
        {
            double p = receiverPowers.count(receiver) ? receiverPowers.at(receiver) : 0.0;
            totalPower += p;
            outFile << "," << p;
        }
        outFile << "," << totalPower << "\n";
    }

    std::cout.imbue(std::locale(""));
    std::cout << "Finished streaming. Total photons: " << totalPhotons << std::endl;
    std::cout << "CSV file written to: " << outputCsvFile << std::endl;
}