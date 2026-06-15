#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <vector>

#include "register_file.h"
#include "instruction_decoder.h"

class CPU
{
public:
    CPU() = default;

    void loadProgram(const std::vector<uint32_t> &program);
    void step();

    void setReg(uint8_t index, uint32_t value);
    uint32_t getReg(uint8_t index) const;

private:
    uint32_t pc = 0;
    std::vector<uint32_t> instructions;

    RegisterFile registers;

    void execRType(const RFormat &instruction);
};

#endif