#include "PhotonProcessor.h"
#include "ParametersFileReader.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static bool hasPhotonDataFiles(const fs::path& dir) {
    if (!fs::exists(dir) || !fs::is_directory(dir)) return false;
    for (const auto& de : fs::directory_iterator(dir)) {
        if (!de.is_regular_file()) continue;
        const auto name = de.path().filename().string();
        // quick check; CompareFilename will do robust ordering later
        if (name.rfind("photons_", 0) == 0 && de.path().extension() == ".dat") {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: STTAnalytics <photon_folder_path> <output_csv_file>\n";
        return 64; // EX_USAGE
    }

    const std::string folderPath    = argv[1];
    const std::string outputCsvFile = argv[2];

    try
    {
        const fs::path folder(folderPath);

        // Basic input validation
        if (!fs::exists(folder) || !fs::is_directory(folder)) {
            std::cerr << "Error: \"" << folderPath << "\" is not a directory or does not exist.\n";
            return 66; // EX_NOINPUT
        }

        const fs::path params = folder / "photons_parameters.txt";
        if (!fs::exists(params) || !fs::is_regular_file(params)) {
            std::cerr << "Error: parameters file not found at " << params << "\n";
            return 66; // EX_NOINPUT
        }

        if (!hasPhotonDataFiles(folder)) {
            std::cerr << "Error: no photon data files (photons_*.dat) found in " << folderPath << "\n";
            return 66; // EX_NOINPUT
        }

        // Construct and read parameters
        ParametersFileReader reader(folderPath);
        reader.read();

        // Get surface map and power per photon
        SurfaceMap surfaceMap(reader.getSurfaceMap());
        const double powerPerPhoton = reader.getPowerPerPhoton();

        if (powerPerPhoton <= 0.0) {
            std::cerr << "Error: invalid power per photon (" << powerPerPhoton << ").\n";
            return 65; // EX_DATAERR
        }

        std::cout << "Surfaces: " << surfaceMap.getHeliostatCount() << " heliostats, "
                  << surfaceMap.getReceiverCount()  << " receivers. Total: "
                  << (surfaceMap.getHeliostatCount() + surfaceMap.getReceiverCount())
                  << " surfaces.\n";

        // Optional: list receivers once (useful sanity check)
        const auto& receiverMap = surfaceMap.getReceiverNamesMap();
        if (receiverMap.empty()) {
            std::cerr << "Warning: No receivers detected in surface map.\n";
        } else {
            std::cout << "Receivers:\n";
            for (const auto& [id, name] : receiverMap)
                std::cout << "  - ID " << id << " : " << name << '\n';
        }

        std::cout << "Power per photon: " << powerPerPhoton << " (units from parameters file)\n";
        std::cout << "Streaming photon data from: " << folderPath << '\n';

        const auto t0 = std::chrono::steady_clock::now();

        PhotonProcessor processor(folderPath, surfaceMap, powerPerPhoton);
        processor.processPhotons(outputCsvFile);

        const auto t1 = std::chrono::steady_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();

        std::cout << "Done. Wrote CSV: " << outputCsvFile << "  (" << ms << " ms)\n";
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << '\n';
        return 1;
    }
}