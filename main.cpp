#include "ParametersFileReader.h"
#include "BinaryPhotonReader.h"
#include "SurfaceMap.h"
#include "EnergyAggregator.h"
#include "PhotonProcessor.h"

#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: STTAnalytics <path_to_simulation_folder>\n";
        return 1;
    }

    std::string folderPath = argv[1];

    try
    {
        std::cout << "Reading parameters...\n";
        ParametersFileReader paramReader(folderPath);
        paramReader.read();

        double powerPerPhoton = paramReader.getPowerPerPhoton();
        SurfaceMap surfaceMap(paramReader.getSurfaceMap());
        EnergyAggregator aggregator(surfaceMap, powerPerPhoton);
        PhotonProcessor processor(aggregator);

        std::cout << "Streaming photon data...\n";
        BinaryPhotonReader photonReader(folderPath);
        photonReader.streamPhotons(processor);

        std::cout << "Finished processing " << processor.getTotalCount() << " photons.\n";

        std::string csvPath = (std::filesystem::path(folderPath) / "energy_report.csv").string();
        aggregator.writeCSV(csvPath);

        std::cout << "Results written to " << csvPath << "\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}