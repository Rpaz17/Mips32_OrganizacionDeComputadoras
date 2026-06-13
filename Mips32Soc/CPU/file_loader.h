#ifndef FILE_LOADER_H
#define FILE_LOADER_H

#include <cstdint>
#include <string>
#include <vector>

class FileLoader
{
public:
    static std::vector<uint32_t> loadInstructions(const std::string &filename);
};

#endif // FILE_LOADER_H