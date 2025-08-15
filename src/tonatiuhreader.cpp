#include "tonatiuhreader.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cmath>           // std::llround, std::lrint
#include "comparefilename.h"

namespace fs = std::filesystem;

// Byte-swap helper (works for PODs like double)
template <class T>
inline void endswap(T* objp)
{
    unsigned char* memp = reinterpret_cast<unsigned char*>(objp);
    std::reverse(memp, memp + sizeof(T));
}

// Read one big-endian double from current stream into host double
static bool readBigEndianDouble(std::ifstream& ifs, double& out)
{
    ifs.read(reinterpret_cast<char*>(&out), sizeof(out));
    if (!ifs) return false; // short read or error
    endswap(&out);          // big-endian -> host
    return true;
}

TonatiuhReader::TonatiuhReader(fs::path directory_path)
    : m_directory_path{directory_path}
{
    // Collect .dat files
    for (auto& p : fs::directory_iterator(directory_path)) {
        if (p.is_regular_file() && p.path().extension() == ".dat") {
            m_directory_entry.push_back(p);
        }
    }
    std::sort(m_directory_entry.begin(), m_directory_entry.end(), CompareFilename{});

    // Prepare a reasonable read buffer (≈700 KiB)
    m_buf_size = 1024u * 700u;
    m_buf = std::unique_ptr<char[]>(new char[m_buf_size]);
}

bool TonatiuhReader::OpenNextFile()
{
    if (m_file_number >= m_directory_entry.size()) return false;

    if (m_ifs.is_open()) m_ifs.close();
    m_ifs.clear();

    // Install buffer before open
    m_ifs.rdbuf()->pubsetbuf(m_buf.get(), static_cast<std::streamsize>(m_buf_size));

    const auto& path = m_directory_entry[m_file_number].path();
    m_ifs.open(path, std::ios::binary);
    if (!m_ifs.is_open()) {
        std::cerr << "Failed to open photon file: " << path << "\n";
        return false;
    }

    std::cout << path.string() << std::endl;
    return true;
}

bool TonatiuhReader::ReadPhotonInfo(PhotonInfo& photon_info)
{
    if (m_first_photon) {
        m_first_photon = false;
        if (m_directory_entry.empty()) return false;
        if (!OpenNextFile()) return false;
    }

    while (true) {
        if (ReadPhotonInfoFromFile(photon_info)) {
            return true;
        }

        // If not EOF, it’s likely a short-read/corruption. Clear to move on.
        if (!m_ifs.eof()) {
            m_ifs.clear();
        }

        // Advance to next file if available
        if (m_file_number + 1 < m_directory_entry.size()) {
            ++m_file_number;
            if (!OpenNextFile()) return false;
            continue;
        }

        // No more files
        return false;
    }
}

bool TonatiuhReader::ReadPhotonInfoFromFile(PhotonInfo& p)
{
    // Read all fields as doubles, then convert integer-like fields
    double d_id, d_x, d_y, d_z, d_side, d_prev, d_next, d_surface;

    std::streampos before = m_ifs.tellg();

    if (!readBigEndianDouble(m_ifs, d_id))      { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_x))       { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_y))       { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_z))       { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_side))    { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_prev))    { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_next))    { m_ifs.clear(); m_ifs.seekg(before); return false; }
    if (!readBigEndianDouble(m_ifs, d_surface)) { m_ifs.clear(); m_ifs.seekg(before); return false; }

    // Floating fields
    p.x = d_x;
    p.y = d_y;
    p.z = d_z;

    // Integer-like fields
    p.id          = static_cast<std::uint64_t>(std::llround(d_id));
    p.previous_id = static_cast<std::uint64_t>(std::llround(d_prev));
    p.next_id     = static_cast<std::uint64_t>(std::llround(d_next));
    p.surface_id  = static_cast<std::uint64_t>(std::llround(d_surface));
    p.side        = static_cast<int>(std::lrint(d_side));

    return true;
}