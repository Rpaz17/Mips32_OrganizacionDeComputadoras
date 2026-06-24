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
    std::vector<uint8_t> stackM;

    static constexpr uint32_t STACK_TOP = 0x7FFFEFFC;
    static constexpr uint32_t STACK_SIZE = 0X2000;
    static constexpr uint32_t STACK_BASE = STACK_TOP - STACK_SIZE + 4;

    uint32_t keypadReg = 0;
    uint32_t timerReg = 0;

    VGAFramebuffer *vgaFrameBuffer = nullptr;
    std::mutex *vgamut = nullptr;

    bool isDataAddr(uint32_t addr) const
    {
        return addr >= DATA_BASE && addr < DATA_BASE + DATA_SIZE;
    }

    bool isStackAddre(uint32_t addr) const
    {
        return addr >= STACK_BASE && addr <= STACK_TOP;
    }

    uint8_t &stackAt(uint32_t a)
    {
        return stackM[a - STACK_BASE];
    }

    const uint8_t &stackAt(uint32_t a) const
    {
        return stackM[a - STACK_BASE];
    }

    uint32_t translate(uint32_t address) const
    {
        if (isDataAddr(address))
            return address - DATA_BASE;

        throw std::runtime_error("Data memory address out of range!");
    }

public:
    static constexpr uint32_t DATA_BASE = 0x10000000;
    static constexpr uint32_t DATA_SIZE = 0x2000;
    static constexpr uint32_t VGA_BASE = 0x0000B800;
    static constexpr uint32_t VGA_END = 0x0000CABF;

    static constexpr uint32_t KEYPAD_ADDR = 0xFFFF0004;
    static constexpr uint32_t TIMER_ADDR = 0xFFFF0008;

    void connectVGA(VGAFramebuffer *framebuff, std::mutex *mut);
    bool isVGAaddr(uint32_t addr) const;
    void storeVGAHalf(uint32_t addr, uint16_t value);

    DataMemory() : memory(DATA_SIZE, 0), stackM(STACK_SIZE, 0) {}

    void reset();
    uint8_t loadByte(uint32_t address) const;
    uint16_t loadHalf(uint32_t address) const;
    uint32_t loadWord(uint32_t address) const;
    void storeByte(uint32_t add, uint8_t value);
    void storeHalf(uint32_t add, uint16_t value);
    void storeWord(uint32_t add, uint32_t value);

    void setKeypad(uint32_t v) { keypadReg = v; }
    uint32_t getTimer() const { return timerReg; }
    void setTimer(uint32_t ms) { timerReg = ms; }

    void loadInitialData(const std::vector<uint8_t> &data)
    {
        size_t count = std::min(data.size(), memory.size());
        std::copy(data.begin(), data.begin() + count, memory.begin());
    }
};

#endif