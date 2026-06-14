#ifndef INSTRUCTION_DECODER_H
#define INSTRUCTION_DECODER_H

#include <cstdint>
#include <string>

struct RFormat
{
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint8_t funct;
};

struct IFormat
{
    uint8_t opcode;
    uint8_t rs;
    uint8_t rt;
    uint16_t immediate;
};

struct JFormat
{
    uint8_t opcode;
    uint32_t address;
};

enum class InstFormat
{
    R_TYPE,
    I_TYPE,
    J_TYPE,
    UNKNOWN
};

class Instruction_Decoder
{
public:
    static uint8_t opcode(uint32_t instruction)
    {
        return static_cast<uint8_t>((instruction >> 26) & 0x3F);
    }

    static RFormat decoderR(uint32_t instruction)
    {
        return RFormat{
            .opcode = opcode(instruction),
            .rs = static_cast<uint8_t>((instruction >> 21) & 0x1F),
            .rt = static_cast<uint8_t>((instruction >> 16) & 0x1F),
            .rd = static_cast<uint8_t>((instruction >> 11) & 0x1F),
            .shamt = static_cast<uint8_t>((instruction >> 6) & 0x1F),
            .funct = static_cast<uint8_t>(instruction & 0x3F)};
    }

    static IFormat decoderI(uint32_t instruction)
    {
        return IFormat{
            .opcode = opcode(instruction),
            .rs = static_cast<uint8_t>((instruction >> 21) & 0x1F),
            .rt = static_cast<uint8_t>((instruction >> 16) & 0x1F),
            .immediate = static_cast<uint16_t>(instruction & 0xFFFF)};
    }

    static JFormat decoderJ(uint32_t instruction)
    {
        return JFormat{
            .opcode = opcode(instruction),
            .address = instruction & 0x3FFFFFF};
    }

    static InstFormat getFormat(uint32_t instruction)
    {
        uint8_t op = opcode(instruction);
        if (op == 0x00)
            return InstFormat::R_TYPE;
        else if (op == 0x02 || op == 0x03)
            return InstFormat::J_TYPE;

        return InstFormat::I_TYPE;
    }
};

#endif