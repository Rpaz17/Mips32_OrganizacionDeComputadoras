#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <vector>

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
    std::array<uint32_t, 32> registers{};

    void execRType(uint32_t instruction);
};

#endif