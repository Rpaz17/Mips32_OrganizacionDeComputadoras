#include "cpu.h"

#include <stdexcept>
#include <cstdint>

#include "alu.h"

namespace
{
    int32_t signedExtend16(uint16_t value)
    {
        return static_cast<int16_t>(value);
    }

    uint32_t zeroExtend16(uint16_t value)
    {
        return static_cast<uint32_t>(value);
    }
}

void CPU::loadProgram(const std::vector<uint32_t> &program)
{
    instructions = program;
    pc = 0;
    registers.reset();
}

void CPU::step()
{
    if (pc % 4 != 0)
    {
        throw std::runtime_error("PC is not aligned to 4 bytes");
    }

    uint32_t index = pc / 4;

    if (index >= instructions.size())
    {
        throw std::runtime_error("PC out of program");
    }

    uint32_t raw = instructions[index];

    uint32_t nextPc = pc + 4;

    InstFormat format = Instruction_Decoder::getFormat(raw);

    switch (format)
    {
    case InstFormat::R_TYPE:
    {
        RFormat decoded = Instruction_Decoder::decoderR(raw);
        execRType(decoded);
        break;
    }
    case InstFormat::I_TYPE:
    {
        IFormat decoded = Instruction_Decoder::decoderI(raw);
        execIType(decoded);
        break;
    }
    case InstFormat::J_TYPE:
    {
        JFormat decoded = Instruction_Decoder::decoderJ(raw);
        execJType(decoded);
        break;
    }
    default:
        throw std::runtime_error("Unknown format ^ 3 ^");
    }

    if (pc == index * 4)
    {
        pc = nextPc;
    }
}

void CPU::setReg(uint8_t index, uint32_t value)
{
    registers.write(index, value);
}

uint32_t CPU::getReg(uint8_t index) const
{
    return registers.read(index);
}

void CPU::execRType(const RFormat &instruction)
{
    if (instruction.opcode != 0x00)
        throw std::runtime_error("The instruction isn't RFormat!");

    uint32_t rsVal = registers.read(instruction.rs);
    uint32_t rtVal = registers.read(instruction.rt);

    switch (instruction.funct)
    {
    case 0x00:
        // nop
        registers.write(instruction.rd, ALU::sll(rtVal, instruction.shamt));
        break;
    case 0x02:
        // srl
        registers.write(instruction.rd, ALU::srl(rtVal, instruction.shamt));
        break;
    case 0x03:
        // sra
        registers.write(instruction.rd, ALU::sra(rtVal, instruction.shamt));
        break;
    case 0x04:
        // sllv
        registers.write(instruction.rd, ALU::sll(rtVal, rsVal & 0x1F));
        break;
    case 0x06:
        // srlv
        registers.write(instruction.rd, ALU::srl(rtVal, rsVal & 0x1F));
        break;
    case 0x07:
        // srav
        registers.write(instruction.rd, ALU::sra(rtVal, rsVal & 0x1F));
        break;
    case 0x08:
        // jr
        pc = rsVal;
        break;
    case 0x20:
        // add
        registers.write(instruction.rd, ALU::add(rsVal, rtVal));
        break;
    case 0x21:
        // addu
        registers.write(instruction.rd, ALU::addu(rsVal, rtVal));
        break;
    case 0x22:
        // sub
        registers.write(instruction.rd, ALU::sub(rsVal, rtVal));
        break;
    case 0x23:
        // subu
        registers.write(instruction.rd, ALU::subu(rsVal, rtVal));
        break;
    case 0x24:
        // bitwiseAnd
        registers.write(instruction.rd, ALU::bitwiseAnd(rsVal, rtVal));
        break;
    case 0x25:
        // bitwiseOr
        registers.write(instruction.rd, ALU::bitwiseOr(rsVal, rtVal));
        break;
    case 0x26:
        // bitwiseXor
        registers.write(instruction.rd, ALU::bitwiseXor(rsVal, rtVal));
        break;
    case 0x2A:
        // slt signed
        registers.write(instruction.rd, ALU::slt(rsVal, rtVal));
        break;
    case 0x2B:
        // sltu unsigned
        registers.write(instruction.rd, ALU::sltu(rsVal, rtVal));
        break;
    default:
        throw std::runtime_error("RFormat function not created yet");
    }
}

void CPU::execIType(const IFormat &instruction)
{
    uint32_t rsVal = registers.read(instruction.rs);

    switch (instruction.opcode)
    {
    case 0x08:
    {
        // addi rt = rs + sign_ext(imm)
        int32_t imm = signedExtend16(instruction.immediate);
        uint32_t result = static_cast<uint32_t>(static_cast<int32_t>(rsVal) + imm);
        registers.write(instruction.rt, result);
        break;
    }
    case 0x09:
    {
        // addiu rt = rs + sign_ext(imm)
        int32_t imm = signedExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal + static_cast<uint32_t>(imm));
        break;
    }
    case 0x0A:
    {
        // slti rt = (rs < sign_ext(imm)) ? 1 : 0
        int32_t imm = signedExtend16(instruction.immediate);
        registers.write(instruction.rt, static_cast<int32_t>(rsVal < imm ? 1 : 0));
        break;
    }
    case 0x0B:
    {
        // sltiu rt = (rs < sign_ext(imm)) ? 1 : 0 unsigned compare
        int32_t imm = signedExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal < imm ? 1 : 0);
        break;
    }
    case 0x0C:
    {
        // andi rt = rs & zero_ext(imm)
        int32_t imm = signedExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal & imm);
        break;
    }
    case 0x0D:
    {
        // ori rt = rs | zero_ext(immm)
        int32_t imm = signedExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal | imm);
        break;
    }
    case 0x0E:
    {
        // xori rt = rs ^ zero_exit(imm)
        int32_t imm = signedExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal ^ imm);
        break;
    }
    case 0x0F:
    {
        // lui rt = imm << 16
        registers.write(instruction.rt, static_cast<uint32_t>(instruction.immediate) << 16);
        break;
    }
    default:
        throw std::runtime_error("IFormat opcode not created yet ^ _ ^");
        break;
    }
}

void CPU::execJType(const JFormat &instruction)
{
    switch (instruction.opcode)
    {
    case 0x02:
        // j
        pc = (pc & 0xF0000000) | (instruction.address << 2);
        break;
    case 0x03:
        registers.write(31, pc + 4);
        pc = (pc & 0xF0000000) | (instruction.address << 2);
        break;
    default:
        throw std::runtime_error("JFormat not done yet ^ 6 ^");
    }
}
