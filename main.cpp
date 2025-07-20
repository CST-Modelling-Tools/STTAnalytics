#include "ParametersFileReader.h"
#include "BinaryPhotonReader.h"
#include "SurfaceMap.h"
#include "EnergyAggregator.h"
#include "PhotonProcessor.h"

#include <iostream>
#include <string>

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
        ParametersFileReader paramReader(folderPath);
        paramReader.read();

        double powerPerPhoton = paramReader.getPowerPerPhoton();
        SurfaceMap surfaceMap(paramReader.getSurfaceMap());
        EnergyAggregator aggregator(surfaceMap, powerPerPhoton);

        PhotonProcessor processor(aggregator);
        BinaryPhotonReader photonReader(folderPath);
        photonReader.streamPhotons(processor);

        aggregator.printRanking();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Fatal error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}