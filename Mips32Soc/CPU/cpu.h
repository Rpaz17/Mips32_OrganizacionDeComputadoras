#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <vector>
#include <mutex>
#include "memory.h"

#include "register_file.h"
#include "instruction_decoder.h"
#include "../vga/VGAFramebuffer.h"

class CPU
{
public:
    CPU() = default;

    void loadProgram(const std::vector<uint32_t> &program);
    bool canStep();
    void step();

    void setReg(uint8_t index, uint32_t value);
    uint32_t getReg(uint8_t index) const;

    void connectVGA(VGAFramebuffer *framebuff, std::mutex *mut);

private:
    uint32_t pc = 0;
    std::vector<uint32_t> instructions;

    RegisterFile registers;
    DataMemory dataM;

    void execRType(const RFormat &instruction);
    void execIType(const IFormat &instruction);
    void execJType(const JFormat &instruction);
};

#endif