#ifndef TONATIUHREADER_H
#define TONATIUHREADER_H

#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

struct PhotonInfo
{
    double id;
    double x;
    double y;
    double z;
    double side;
    double previous_id;
    double next_id;
    double surface_id;
};

class TonatiuhReader
{
public:
    TonatiuhReader(fs::path directory_path);
    ~TonatiuhReader() {delete[] m_buf;}
    std::vector<fs::directory_entry> directory_entry() {return m_directory_entry;}
    bool ReadPhotonInfo(PhotonInfo& photon_info);
    bool ReadPhotonInfoFromFile(PhotonInfo& photon_info);

private:
    fs::path m_directory_path;
    std::vector<fs::directory_entry> m_directory_entry;
    int m_file_number;
    bool m_first_photon;
    std::ifstream m_ifs;
    char* m_buf;
};

#endif // TONATIUHREADER_H