#ifndef ALU_H
#define ALU_H

#include <cstdint>

class ALU
{
public:
    static uint32_t add(uint32_t a, uint32_t b);
    static uint32_t addu(uint32_t a, uint32_t b);

    static uint32_t sub(uint32_t a, uint32_t b);
    static uint32_t subu(uint32_t a, uint32_t b);

    static uint32_t bitwiseAnd(uint32_t a, uint32_t b);
    static uint32_t bitwiseOr(uint32_t a, uint32_t b);
    static uint32_t bitwiseXor(uint32_t a, uint32_t b);

    static uint32_t slt(uint32_t a, uint32_t b);
    static uint32_t sltu(uint32_t a, uint32_t b);

    static uint32_t sll(uint32_t value, uint8_t shamt);
    static uint32_t srl(uint32_t value, uint8_t shamt);
    static uint32_t sra(uint32_t value, uint8_t shamt);
};

#endif