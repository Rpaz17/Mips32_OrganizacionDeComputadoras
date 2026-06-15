#include "cpu.h"

#include <stdexcept>

#include "alu.h"

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
        throw std::runtime_error("IFormat not applied yet...");
    case InstFormat::J_TYPE:
        throw std::runtime_error("JFormat not applied yet...");
    default:
        throw std::runtime_error("Unknown format ^ 3 ^");
    }

    pc += 4;
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
        throw std::runtime_error("The instruction isn't RFormt!");

    uint32_t rsVal = registers.read(instruction.rs);
    uint32_t rtVal = registers.read(instruction.rt);

    switch (instruction.funct)
    {
    case 0x00:
        // nop
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
        // slt
        registers.write(instruction.rd, ALU::slt(rsVal, rtVal));
        break;
    case 0x2B:
        // sltu
        registers.write(instruction.rd, ALU::sltu(rsVal, rtVal));
        break;
    default:
        throw std::runtime_error("RFormat function not created yet");
    }
}
