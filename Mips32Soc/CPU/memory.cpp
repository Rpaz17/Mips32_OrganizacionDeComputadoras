#include "memory.h"

#include <iostream>

void DataMemory::connectVGA(VGAFramebuffer *framebuff, std::mutex *mut)
{
    vgaFrameBuffer = framebuff;
    vgamut = mut;
}

bool DataMemory::isVGAaddr(uint32_t addr) const
{
    return addr >= VGA_BASE && addr <= VGA_END;
}

void DataMemory::storeVGAHalf(uint32_t addr, uint16_t value)
{
    if (vgaFrameBuffer == nullptr)
    {
        throw std::runtime_error("Frame Buffer not connected");
    }

    uint32_t offs = addr - VGA_BASE;
    uint32_t cellI = offs / 2;

    size_t row = cellI / 80;
    size_t col = cellI % 80;

    uint8_t ch = static_cast<uint8_t>(value & 0x00FF);
    uint8_t fg = static_cast<uint8_t>((value >> 8) & 0x0F);
    uint8_t bg = static_cast<uint8_t>((value >> 12) & 0x0F);

    if (vgamut != nullptr)
    {
        vgamut->lock();
        vgaFrameBuffer->writePixel(row, col, ch, fg, bg);
        vgamut->unlock();
    }
    else
    {
        vgaFrameBuffer->writePixel(row, col, ch, fg, bg);
    }
}

void DataMemory::reset()
{
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(stackM.begin(), stackM.end(), 0);
}

uint8_t DataMemory::loadByte(uint32_t address) const
{
    if (isStackAddre(address))
        return stackAt(address);
    return memory[translate(address)];
}

uint16_t DataMemory::loadHalf(uint32_t address) const
{
    if (address % 2 != 0)
        throw std::runtime_error("lh/lhu address isn't aligned to 2 bytes.");
    if (isStackAddre(address))
        return static_cast<uint16_t>(stackAt(address) | (static_cast<uint16_t>(stackAt(address + 1)) << 8));
    uint32_t o = translate(address);
    return static_cast<uint16_t>(memory[o] | (static_cast<uint16_t>(memory[o + 1]) << 8));
}

uint32_t DataMemory::loadWord(uint32_t address) const
{
    if (address % 4 != 0)
        throw std::runtime_error("lw address isn't aligned to 4 bytes.");
    if (address == KEYPAD_ADDR)
        return keypadReg;
    if (address == TIMER_ADDR)
        return timerReg;
    if (isStackAddre(address))
        return static_cast<uint32_t>(stackAt(address)) |
               (static_cast<uint32_t>(stackAt(address + 1)) << 8) |
               (static_cast<uint32_t>(stackAt(address + 2)) << 16) |
               (static_cast<uint32_t>(stackAt(address + 3)) << 24);
    uint32_t o = translate(address);
    return static_cast<uint32_t>(memory[o]) |
           (static_cast<uint32_t>(memory[o + 1]) << 8) |
           (static_cast<uint32_t>(memory[o + 2]) << 16) |
           (static_cast<uint32_t>(memory[o + 3]) << 24);
}

void DataMemory::storeByte(uint32_t add, uint8_t value)
{
    if (isStackAddre(add))
    {
        stackAt(add) = value;
        return;
    }
    memory[translate(add)] = value;
}

void DataMemory::storeHalf(uint32_t add, uint16_t value)
{
    if (add % 2 != 0)
        throw std::runtime_error("sh address isn't aligned to 2 bytes.");
    if (isVGAaddr(add))
    {
        storeVGAHalf(add, value);
        return;
    }
    if (isStackAddre(add))
    {
        stackAt(add) = static_cast<uint8_t>(value & 0xFF);
        stackAt(add + 1) = static_cast<uint8_t>((value >> 8) & 0xFF);
        return;
    }
    uint32_t o = translate(add);
    memory[o] = static_cast<uint8_t>(value & 0xFF);
    memory[o + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

void DataMemory::storeWord(uint32_t add, uint32_t value)
{
    if (add % 4 != 0)
        throw std::runtime_error("sw address isn't aligned to 4 bytes.");
    if (isVGAaddr(add))
    {
        storeVGAHalf(add, static_cast<uint16_t>(value & 0xFFFF));
        storeVGAHalf(add + 2, static_cast<uint16_t>((value >> 16) & 0xFFFF));
        return;
    }
    if (isStackAddre(add))
    {
        stackAt(add) = static_cast<uint8_t>(value & 0xFF);
        stackAt(add + 1) = static_cast<uint8_t>((value >> 8) & 0xFF);
        stackAt(add + 2) = static_cast<uint8_t>((value >> 16) & 0xFF);
        stackAt(add + 3) = static_cast<uint8_t>((value >> 24) & 0xFF);
        return;
    }
    uint32_t o = translate(add);
    memory[o] = static_cast<uint8_t>(value & 0xFF);
    memory[o + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    memory[o + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    memory[o + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}