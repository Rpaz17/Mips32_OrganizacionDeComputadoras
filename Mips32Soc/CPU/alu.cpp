#include "alu.h"

uint32_t ALU::add(uint32_t a, uint32_t b)
{
    return static_cast<uint32_t>(static_cast<int32_t>(a) + static_cast<int32_t>(b));
}

uint32_t ALU::addu(uint32_t a, uint32_t b)
{
    return a + b;
}

uint32_t ALU::sub(uint32_t a, uint32_t b)
{
    return static_cast<uint32_t>(static_cast<int32_t>(a) - static_cast<int32_t>(b));
}

uint32_t ALU::subu(uint32_t a, uint32_t b)
{
    return a - b;
}

uint32_t ALU::bitwiseAnd(uint32_t a, uint32_t b)
{
    return a & b;
}

uint32_t ALU::bitwiseOr(uint32_t a, uint32_t b)
{
    return a | b;
}

uint32_t ALU::bitwiseXor(uint32_t a, uint32_t b)
{
    return a ^ b;
}

uint32_t ALU::slt(uint32_t a, uint32_t b)
{
    return static_cast<int32_t>(a) < static_cast<int32_t>(b) ? 1 : 0;
}

uint32_t ALU::sltu(uint32_t a, uint32_t b)
{
    return a < b ? 1 : 0;
}

uint32_t ALU::sll(uint32_t value, uint8_t shamt)
{
    return value << shamt;
}

uint32_t ALU::srl(uint32_t value, uint8_t shamt)
{
    return value >> shamt;
}

uint32_t ALU::sra(uint32_t value, uint8_t shamt)
{
    return static_cast<uint32_t>(static_cast<int32_t>(value) >> shamt);
}

HiLo ALU::mult(uint32_t a, uint32_t b)
{
    int64_t result = static_cast<int64_t>(static_cast<int32_t>(a)) * static_cast<int64_t>(static_cast<int32_t>(b));
    return HiLo{
        .hi = static_cast<uint32_t>((static_cast<int64_t>(result) >> 32) & 0xFFFFFFFF),
        .lo = static_cast<uint32_t>(static_cast<int64_t>(result) & 0xFFFFFFFF)};
}

HiLo ALU::multu(uint32_t a, uint32_t b)
{
    int64_t result = static_cast<int64_t>(a) * static_cast<int64_t>(b);
    return HiLo{
        .hi = static_cast<uint32_t>((result >> 32) & 0xFFFFFFFF),
        .lo = static_cast<uint32_t>(result & 0xFFFFFFFF)};
    return HiLo();
}

HiLo ALU::div(uint32_t a, uint32_t b)
{
    if (b == 0)
    {
        return HiLo{.hi = 0, .lo = 0};
    }

    int32_t sa = static_cast<int32_t>(a);
    int32_t sb = static_cast<int32_t>(b);
    return HiLo{
        .hi = static_cast<uint32_t>(sa % sb),
        .lo = static_cast<uint32_t>(sa / sb)};
}

HiLo ALU::divu(uint32_t a, uint32_t b)
{
    if (b == 0)
    {
        return HiLo{.hi = 0, .lo = 0};
    }
    return HiLo{
        .hi = a % b,
        .lo = a / b};
}
