#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <vector>
#include <stdexcept>
#include <mutex>

#include "../vga/VGAFramebuffer.h"

class DataMemory
{
private:
    std::vector<uint8_t> memory;

    VGAFramebuffer *vgaFrameBuffer = nullptr;
    std::mutex *vgamut = nullptr;

    uint32_t translate(uint32_t address) const
    {
        if (address < DATA_BASE || address >= DATA_BASE + DATA_SIZE)
        {
            throw std::runtime_error("Data memory address out of range");
        }

        uint32_t offset = address - DATA_BASE;

        if (offset >= memory.size())
        {
            throw std::runtime_error("Data memory offset out of range");
        }
        return offset;
    }

public:
    static constexpr uint32_t DATA_BASE = 0x10000000;
    static constexpr uint32_t DATA_SIZE = 0x2000;
    static constexpr uint32_t VGA_BASE = 0x0000B800;
    static constexpr uint32_t VGA_END = 0x0000CABF;

    void connectVGA(VGAFramebuffer *framebuff, std::mutex *mut);
    bool isVGAaddr(uint32_t addr) const;
    void storeVGAHalf(uint32_t addr, uint16_t value);

    DataMemory() : memory(DATA_SIZE, 0) {}

    void reset();
    uint8_t loadByte(uint32_t address) const;
    uint16_t loadHalf(uint32_t address) const;
    uint32_t loadWord(uint32_t address) const;
    void storeByte(uint32_t add, uint8_t value);
    void storeHalf(uint32_t add, uint16_t value);
    void storeWord(uint32_t add, uint32_t value);
};

#endif