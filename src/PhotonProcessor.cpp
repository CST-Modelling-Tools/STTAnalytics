#include "PhotonProcessor.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <vector>

PhotonProcessor::PhotonProcessor(const std::string& folderPath_,
                                 const SurfaceMap& surfaceMap_,
                                 double powerPerPhoton_)
    : folderPath(folderPath_), surfaceMap(surfaceMap_), powerPerPhoton(powerPerPhoton_)
{
}

void PhotonProcessor::processPhotons(const std::string& outputCsvFile)
{
    // Stream photons using TonatiuhReader (handles file ordering, buffering, and endianness)
    TonatiuhReader reader(folderPath);

    // Accumulator: HeliostatName -> (ReceiverName -> Power)
    std::map<std::string, std::map<std::string, double>> energyMap;

    std::uint64_t rayCounter    = 0;
    std::uint64_t countedRays   = 0;
    std::uint64_t skippedRays   = 0;

    std::vector<PhotonInfo> ray;
    ray.reserve(32);

    PhotonInfo p{};
    while (reader.ReadPhotonInfo(p))
    {
        ++totalPhotons;
        ray.push_back(p);

        // A ray ends when the current photon has next_id == 0
        if (p.next_id == 0)
        {
            ++rayCounter;

            if (ray.size() >= 2)
            {
                const PhotonInfo& pen  = ray[ray.size() - 2]; // penultimate
                const PhotonInfo& last = ray.back();          // receiver hit

                const std::uint64_t heliostatID = pen.surface_id;
                const std::uint64_t receiverID  = last.surface_id;
                const int arrivalSide = last.side;            // check arrival at receiver

                if (arrivalSide == 1 &&
                    surfaceMap.isHeliostat(heliostatID) &&
                    surfaceMap.isReceiver(receiverID))
                {
                    const std::string heliostatName = surfaceMap.getHeliostatName(heliostatID);
                    const std::string receiverName  = surfaceMap.getReceiverName(receiverID);
                    energyMap[heliostatName][receiverName] += powerPerPhoton;
                    ++countedRays;
                }
                else
                {
                    ++skippedRays;
                }
            }

            ray.clear();

            if (rayCounter % 1000000 == 0)
                std::cout << "Processed " << rayCounter << " rays...\n";
        }
    }

    std::cout << "Finished streaming.\n";
    std::cout << "  - Total photons read: " << totalPhotons << "\n";
    std::cout << "  - Rays processed: " << rayCounter << "\n";
    std::cout << "  - Counted heliostat→receiver rays (side==1): " << countedRays << "\n";
    std::cout << "  - Skipped rays: " << skippedRays << "\n";

    // -----------------------
    // Build receiver list sorted by numeric suffix (Receiver1, Receiver2, ...)
    // -----------------------
    const auto& receiverNameMap = surfaceMap.getReceiverNamesMap(); // id -> name
    std::vector<std::string> receivers;
    receivers.reserve(receiverNameMap.size());
    for (const auto& kv : receiverNameMap) receivers.push_back(kv.second);

    auto trailingNumber = [](const std::string& s) -> long long {
        if (s.empty()) return -1;
        std::size_t i = s.size(), end = i;
        while (i > 0 && std::isdigit(static_cast<unsigned char>(s[i - 1]))) --i;
        if (i < end) {
            try { return std::stoll(s.substr(i, end - i)); }
            catch (...) { return -1; }
        }
        return -1;
    };

    std::sort(receivers.begin(), receivers.end(),
              [&](const std::string& a, const std::string& b){
                  long long na = trailingNumber(a);
                  long long nb = trailingNumber(b);
                  if (na >= 0 && nb >= 0) return na < nb;  // numeric first, increasing
                  if (na >= 0) return true;                // numeric before non-numeric
                  if (nb >= 0) return false;
                  return a < b;                            // both non-numeric: lexicographic
              });

    // -----------------------
    // Write CSV
    // -----------------------
    std::ofstream out(outputCsvFile);
    if (!out) {
        std::cerr << "Error writing CSV file: " << outputCsvFile << "\n";
        return;
    }

    out << "Heliostat Label";
    for (const std::string& rec : receivers)
        out << ", Power to " << rec;
    out << ", Total Power to Receivers\n";

    for (const auto& pair : energyMap)
    {
        const std::string& heliostat = pair.first;
        const std::map<std::string, double>& recMap = pair.second;

        out << heliostat;
        double total = 0.0;

        for (const std::string& rec : receivers)
        {
            const auto it = recMap.find(rec);
            const double value = (it != recMap.end()) ? it->second : 0.0;
            out << ", " << value;
            total += value;
        }

        out << ", " << total << "\n";
    }

    std::cout << "CSV file written to: " << outputCsvFile << "\n";
    std::cout << "Finished.\n";
}