#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <array>
#include <vector>
#include <cstdint>

class RegisterFile
{
public:
    RegisterFile()
    {
        registers.fill(0);
    }

    uint32_t read(uint8_t index) const
    {
        return registers[index];
    }

    void write(uint8_t index, uint32_t value)
    {
        if (index == 0)
        {
            return; //$zero siempre igual a cero
        }

        registers[index] = value;
    }

    uint32_t readHI() const
    {
        return hi;
    }

    uint32_t readLO() const
    {
        return lo;
    }

    void writeHI(uint32_t value)
    {
        hi = value;
    }

    void writeLO(uint32_t value)
    {
        lo = value;
    }

    void reset()
    {
        registers.fill(0);
        hi = 0;
        lo = 0;
    }

private:
    std::array<uint32_t, 32> registers{};
    uint32_t hi = 0;
    uint32_t lo = 0;
};

#endif