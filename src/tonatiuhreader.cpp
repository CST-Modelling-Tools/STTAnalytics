#include "tonatiuhreader.h"
#include "comparefilename.h"
#include <iostream>

TonatiuhReader::TonatiuhReader(fs::path directory_path)
    : m_directory_path {directory_path}, m_file_number {0}, m_first_photon {true}
{
    for(auto& p: fs::directory_iterator(directory_path))
        if(p.is_regular_file() && p.path().extension() == ".dat") m_directory_entry.push_back(p);
    std::sort(std::begin(m_directory_entry), std::end(m_directory_entry), CompareFilename::CompareFilename());

    m_buf = new char[1024 * 700];
}

template <class T>
void endswap(T *objp)
{
  unsigned char *memp = reinterpret_cast<unsigned char*>(objp);
  std::reverse(memp, memp + sizeof(T));
}

bool TonatiuhReader::ReadPhotonInfo(PhotonInfo& photon_info)
{
    if(m_first_photon)
    {
        if( m_directory_entry.empty()) return false;
        else
        {
            m_first_photon = false;
            m_ifs.rdbuf()->pubsetbuf(m_buf, sizeof m_buf);
            m_ifs.open( m_directory_entry[m_file_number].path(), std::ios::binary );
            std::cout << m_directory_entry[m_file_number].path().string() << std::endl;
        }
    }

    if(ReadPhotonInfoFromFile(photon_info)) return true;
    else if(m_ifs.eof())
    {
        m_ifs.close();
        m_file_number++;
        if( m_directory_entry.size() > m_file_number)
        {
            m_ifs.rdbuf()->pubsetbuf(m_buf, sizeof m_buf);
            m_ifs.open( m_directory_entry[m_file_number].path(), std::ios::binary );
            std::cout << m_directory_entry[m_file_number].path().string() << std::endl;
        }
        else return false;
    }

    return ReadPhotonInfoFromFile(photon_info);
}

bool TonatiuhReader::ReadPhotonInfoFromFile(PhotonInfo& photon_info)
{
   m_ifs.read( reinterpret_cast<char*>( &photon_info.id ), sizeof photon_info.id );
   endswap( &photon_info.id );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.x ), sizeof photon_info.x );
   endswap( &photon_info.x );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.y ), sizeof photon_info.y );
   endswap( &photon_info.y );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.z ), sizeof photon_info.z );
   endswap( &photon_info.z );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.side ), sizeof photon_info.side );
   endswap( &photon_info.side );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.previous_id ), sizeof photon_info.previous_id );
   endswap( &photon_info.previous_id );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.next_id ), sizeof photon_info.next_id );
   endswap( &photon_info.next_id );
   m_ifs.read( reinterpret_cast<char*>( &photon_info.surface_id ), sizeof photon_info.surface_id );
   endswap( &photon_info.surface_id );

   return m_ifs.good();
}
