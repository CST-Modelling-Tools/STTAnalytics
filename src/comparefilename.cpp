#include <iostream>
#include <sstream>
#include "comparefilename.h"

CompareFilename::CompareFilename()
{

}

bool CompareFilename::operator ()(const fs::directory_entry& entry1, const fs::directory_entry& entry2)
{
    return TonatiuhFileNumber(entry1.path().filename().string()) < TonatiuhFileNumber(entry2.path().filename().string());
}

int CompareFilename::TonatiuhFileNumber(std::string filename)
{
    std::size_t start = filename.rfind("_") + 1;
    std::size_t end = filename.find(".") - 1;
    int tonatiuh_file_number;
    std::string tfn_as_string = filename.substr(start, end-start+1);
    std::istringstream iss(tfn_as_string);
    iss >> tonatiuh_file_number;
    return tonatiuh_file_number;
}