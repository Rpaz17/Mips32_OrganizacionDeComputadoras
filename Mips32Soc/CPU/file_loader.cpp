#include "file_loader.h"

#include <fstream>
#include <stdexcept>

std::vector<uint32_t> FileLoader::loadInstructions(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (bytes.size() % 4 != 0)
    {
        throw std::runtime_error("File size is not a multiple of 4 bytes: " + filename);
    }

    std::vector<uint32_t> instructions;
    instructions.reserve(bytes.size() / 4);

    for (size_t i = 0; i < bytes.size(); i += 4)
    {
        uint32_t instruction = (static_cast<uint32_t>(bytes[i])) |
                               (static_cast<uint32_t>(bytes[i + 1]) << 8) |
                               (static_cast<uint32_t>(bytes[i + 2]) << 16) |
                               (static_cast<uint32_t>(bytes[i + 3]) << 24);

        instructions.push_back(instruction);
    }

    return instructions;
}
