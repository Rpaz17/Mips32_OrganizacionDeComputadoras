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
