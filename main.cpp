#include "PhotonProcessor.h"
#include "ParametersFileReader.h"

#include <iostream>
#include <iomanip>
#include <locale>

class CommaNumpunct : public std::numpunct<char> {
protected:
    char do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};

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
        std::locale commaLocale(std::cout.getloc(), new CommaNumpunct);
        std::cout.imbue(commaLocale);

        // Read parameters
        ParametersFileReader reader(surfaceFile);
        reader.read();

        const SurfaceMap& surfaceMap = reader.getSurfaceMap();
        double powerPerPhoton = reader.getPowerPerPhoton();

        std::cout << "Surface map contains " << surfaceMap.getHeliostatCount()
                  << " heliostats and " << surfaceMap.getReceiverCount() << " receivers.\n";

        std::cout << "Surface map contains "
                  << surfaceMap.getHeliostatCount() + surfaceMap.getReceiverCount()
                  << " total surfaces (heliostats and receivers).\n";

        std::cout << "Receivers detected:\n";
        for (const auto& name : surfaceMap.getReceiverNames())
            std::cout << "  - " << name << "\n";

        std::cout << "Streaming photon data...\n";

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