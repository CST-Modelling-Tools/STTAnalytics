#ifndef COMPAREFILENAME_H
#define COMPAREFILENAME_H

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class CompareFilename
{
public:
    CompareFilename() = default;

    // strict-weak-order comparator; const so it can be reused freely
    bool operator()(const fs::directory_entry& entry1,
                    const fs::directory_entry& entry2) const;

    // Extract numeric suffix after the last '_' and before the last '.'
    // Returns -1 if no numeric suffix is found.
    int TonatiuhFileNumber(const std::string& filename) const;
};

#endif // COMPAREFILENAME_H