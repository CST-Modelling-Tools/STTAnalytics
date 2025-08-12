#ifndef COMPAREFILENAME_H
#define COMPAREFILENAME_H

#include <filesystem>

namespace fs = std::filesystem;

class CompareFilename
{
public:
    CompareFilename();
    bool operator ()(const fs::directory_entry& entry1, const fs::directory_entry& entry2);
    int TonatiuhFileNumber(std::string filename);
};

#endif // COMPAREFILENAME_H