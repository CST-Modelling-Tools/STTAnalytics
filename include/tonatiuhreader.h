#ifndef TONATIUHREADER_H
#define TONATIUHREADER_H

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>
#include <cstdint>   // for std::uint64_t

namespace fs = std::filesystem;

struct PhotonInfo
{
    std::uint64_t id;          // integer-like fields as integers
    double        x;
    double        y;
    double        z;
    int           side;        // arrival side
    std::uint64_t previous_id;
    std::uint64_t next_id;
    std::uint64_t surface_id;
};

class TonatiuhReader
{
public:
    explicit TonatiuhReader(fs::path directory_path);
    ~TonatiuhReader() = default;

    TonatiuhReader(const TonatiuhReader&) = delete;
    TonatiuhReader& operator=(const TonatiuhReader&) = delete;
    TonatiuhReader(TonatiuhReader&&) noexcept = default;
    TonatiuhReader& operator=(TonatiuhReader&&) noexcept = default;

    const std::vector<fs::directory_entry>& directory_entry() const { return m_directory_entry; }

    // Reads the next photon across files; returns false when no more photons.
    bool ReadPhotonInfo(PhotonInfo& photon_info);

private:
    // Reads one photon from the current file only; returns false on EOF/short read.
    bool ReadPhotonInfoFromFile(PhotonInfo& photon_info);

    // Try to advance to next file; returns true if a new file is open and ready.
    bool OpenNextFile();

private:
    fs::path m_directory_path;
    std::vector<fs::directory_entry> m_directory_entry;
    std::size_t m_file_number = 0;
    bool m_first_photon = true;

    std::ifstream m_ifs;

    // Buffered I/O
    std::unique_ptr<char[]> m_buf;
    std::size_t m_buf_size = 0;
};

#endif // TONATIUHREADER_H