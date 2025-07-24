#include "PhotonProcessor.h"
#include "ParametersFileReader.h"

#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: STTAnalytics <photon_folder_path> <output_csv_file>\n";
        return 1;
    }

    const std::string folderPath = argv[1];
    const std::string outputCsvFile = argv[2];
    const std::string surfaceFile = folderPath;

    try
    {
        // Construct and read parameters
        ParametersFileReader reader(surfaceFile);
        reader.read();

        // Get surface map and power per photon
        SurfaceMap surfaceMap(reader.getSurfaceMap());

        const auto& receiverMap = surfaceMap.getReceiverNamesMap();
        for (const auto& [id, name] : receiverMap)
        {
            std::cout << "Receiver ID: " << id << ", Name: " << name << "\n";
        }

        double powerPerPhoton = reader.getPowerPerPhoton();

        std::cout << "Surface map contains " << surfaceMap.getHeliostatCount()
                  << " heliostats and " << surfaceMap.getReceiverCount() << " receivers.\n";

        std::cout << "Surface map contains "
                  << surfaceMap.getHeliostatCount() + surfaceMap.getReceiverCount()
                  << " total surfaces (heliostats and receivers).\n";

        std::cout << "Known Heliostat Surface IDs:\n";
        for (const auto& [id, name] : surfaceMap.getHeliostatNames())
            std::cout << "  - ID: " << id << " -> " << name << "\n";

        std::cout << "Known Receiver Surface IDs:\n";
        for (const auto& [id, name] : surfaceMap.getReceiverNamesMap())
            std::cout << "  - ID: " << id << " -> " << name << "\n";

        std::cout << "Receivers detected:\n";
        for (const auto& name : surfaceMap.getReceiverNames())
            std::cout << "  - " << name << "\n";

        std::cout << "Streaming photon data...\n";

        // Process photons
        PhotonProcessor processor(folderPath, surfaceMap, powerPerPhoton);
        processor.processPhotons(outputCsvFile);

        std::cout << "Finished.\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Exception: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}