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
}

uint8_t DataMemory::loadByte(uint32_t address) const
{
    uint32_t offs = translate(address);
    return memory[offs];
}

uint16_t DataMemory::loadHalf(uint32_t address) const
{
    if (address % 2 != 0)
    {
        throw std::runtime_error("lh/lhu address isn't aligned to 2 bytes.");
    }

    uint32_t offs = translate(address);

    return static_cast<uint16_t>(memory[offs] | (static_cast<uint16_t>(memory[offs + 1]) << 8));
}

uint32_t DataMemory::loadWord(uint32_t address) const
{
    if (address % 4 != 0)
    {
        throw std::runtime_error("lh/lhu address isn't aligned to 4 bytes.");
    }

    uint32_t offs = translate(address);

    return static_cast<uint32_t>(memory[offs]) |
           (static_cast<uint32_t>(memory[offs + 1]) << 8) |
           (static_cast<uint32_t>(memory[offs + 2]) << 16) |
           (static_cast<uint32_t>(memory[offs + 3]) << 24);
}

void DataMemory::storeByte(uint32_t add, uint8_t value)
{
    uint32_t offs = translate(add);
    memory[offs] = value;
}

void DataMemory::storeHalf(uint32_t add, uint16_t value)
{
    if (add % 2 != 0)
    {
        throw std::runtime_error("sh address isn't aligned to 2 bytes.");
    }

    if (isVGAaddr(add))
    {
        storeVGAHalf(add, value);
        return;
    }

    uint32_t offs = translate(add);
    memory[offs] = static_cast<uint8_t>(value & 0xFF);
    memory[offs + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
}

void DataMemory::storeWord(uint32_t add, uint32_t value)
{
    if (add % 4 != 0)
    {
        throw std::runtime_error("sh address isn't aligned to 4 bytes.");
    }

    if (isVGAaddr(add))
    {
        storeVGAHalf(add, static_cast<uint16_t>(value & 0xFFF));
        storeVGAHalf(add + 2, static_cast<uint16_t>((value >> 16) & 0xFFF));
    }

    uint32_t offs = translate(add);
    memory[offs] = static_cast<uint8_t>(value & 0xFF);
    memory[offs + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
    memory[offs + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
    memory[offs + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
}
