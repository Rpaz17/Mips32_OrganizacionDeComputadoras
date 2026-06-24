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

    int32_t signedExtended8(uint8_t value)
    {
        return static_cast<int8_t>(value);
    }

    int32_t signedExtendedHalf(uint16_t value)
    {
        return static_cast<int16_t>(value);
    }

    int32_t branchOffs(uint16_t imm)
    {
        return signedExtend16(imm) << 2;
    }
}

void CPU::loadProgram(const std::vector<uint32_t> &program)
{
    instructions = program;
    pc = 0x00004000;
    registers.reset();
    registers.write(29, 0x7FFFEFFC);
    dataM.reset();
}

bool CPU::canStep()
{
    static constexpr uint32_t INSTRUCTION_BASE = 0X00004000;

    if (pc < INSTRUCTION_BASE || pc % 4 != 0)
        return false;

    uint32_t index = (pc - INSTRUCTION_BASE) / 4;
    return index < instructions.size();
}

void CPU::step()
{

    static constexpr uint32_t INSTRUCTION_BASE = 0x00004000;
    uint32_t index = (pc - INSTRUCTION_BASE) / 4;

    if (pc % 4 != 0)
    {
        throw std::runtime_error("PC is not aligned to 4 bytes");
    }

    if (pc < INSTRUCTION_BASE)
    {
        throw std::runtime_error("PC below instruction memory base");
    }

    if (index >= instructions.size())
    {
        throw std::runtime_error("PC out of program");
    }

    uint32_t raw = instructions[index];

    uint32_t oldPc = pc;
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

    if (pc == oldPc)
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

void CPU::connectVGA(VGAFramebuffer *framebuff, std::mutex *mut)
{
    dataM.connectVGA(framebuff, mut);
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
    case 0x10:
        // mfhi
        registers.write(instruction.rd, registers.readHI());
        break;
    case 0x12:
        // mflo
        registers.write(instruction.rd, registers.readLO());
        break;
    case 0x18:
    {
        // mult
        HiLo r = ALU::mult(rsVal, rtVal);
        registers.writeHI(r.hi);
        registers.writeLO(r.lo);
        break;
    }
    case 0x19:
    {
        // multu
        HiLo r = ALU::multu(rsVal, rtVal);
        registers.writeHI(r.hi);
        registers.writeLO(r.lo);
    }
    case 0x1A:
    {
        // div
        HiLo r = ALU::div(rsVal, rtVal);
        registers.writeHI(r.hi);
        registers.writeLO(r.lo);
        break;
    }
    case 0x1B:
    {
        // divu
        HiLo r = ALU::divu(rsVal, rtVal);
        registers.writeHI(r.hi);
        registers.writeLO(r.lo);
        break;
    }
    case 0x09:
        // jalr
        registers.write(instruction.rd == 0 ? 31 : instruction.rd, pc + 4);
        pc = rsVal;
        break;
    case 0x11:
        // mthi
        registers.writeHI(rsVal);
        break;
    case 0x13:
        // mtlo
        registers.writeLO(rsVal);
        break;
    case 0x27:
        // nor
        registers.write(instruction.rd, ~ALU::bitwiseOr(rsVal, rtVal));
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
    case 0x04:
    {
        // beq
        uint32_t rtVal = registers.read(instruction.rt);

        if (rsVal == rtVal)
        {
            pc = pc + 4 + static_cast<uint32_t>(branchOffs(instruction.immediate));
        }

        break;
    }
    case 0x05:
    {
        // bne
        uint32_t rtVal = registers.read(instruction.rt);

        if (rsVal != rtVal)
        {
            pc = pc + 4 + static_cast<uint32_t>(branchOffs(instruction.immediate));
        }

        break;
    }
    case 0x06:
    {
        // blez
        if (static_cast<int32_t>(rsVal) <= 0)
        {
            pc = pc + 4 + static_cast<uint32_t>(branchOffs(instruction.immediate));
        }
        break;
    }
    case 0x07:
    {
        // bgtz
        if (static_cast<int32_t>(rsVal) > 0)
        {
            pc = pc + 4 + static_cast<uint32_t>(branchOffs(instruction.immediate));
        }
        break;
    }
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
        registers.write(
            instruction.rt,
            static_cast<int32_t>(rsVal) < imm ? 1 : 0);
        break;
    }
    case 0x0B:
    {
        // sltiu rt = (rs < sign_ext(imm)) ? 1 : 0 unsigned compare
        uint32_t imm = static_cast<uint32_t>(signedExtend16(instruction.immediate));
        registers.write(
            instruction.rt,
            rsVal < imm ? 1 : 0);
        break;
    }
    case 0x0C:
    {
        // andi rt = rs & zero_ext(imm)
        int32_t imm = zeroExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal & imm);
        break;
    }
    case 0x0D:
    {
        // ori rt = rs | zero_ext(immm)
        int32_t imm = zeroExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal | imm);
        break;
    }
    case 0x0E:
    {
        // xori rt = rs ^ zero_exit(imm)
        int32_t imm = zeroExtend16(instruction.immediate);
        registers.write(instruction.rt, rsVal ^ imm);
        break;
    }
    case 0x0F:
    {
        // lui rt = imm << 16
        registers.write(instruction.rt, static_cast<uint32_t>(instruction.immediate) << 16);
        break;
    }
    case 0x20:
    {
        // lb rt = signExt(MEM[rs+imm][7:0])
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint8_t value = dataM.loadByte(add);

        registers.write(
            instruction.rt,
            static_cast<uint32_t>(signedExtended8(value)));
        break;
    }
    case 0x21:
    {
        // lh rt = signExt(MEM[rs + imm][15:0])
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint16_t value = dataM.loadHalf(add);

        registers.write(
            instruction.rt,
            static_cast<uint32_t>(signedExtendedHalf(value)));

        break;
    }
    case 0x23:
    {
        // lw rt = MEM[rs + imm]
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint32_t value = dataM.loadWord(add);

        registers.write(
            instruction.rt, value);

        break;
    }
    case 0x24:
    {
        // lbu rt = zeroExt(MEM[rs + imm][7:0])
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint8_t value = dataM.loadByte(add);

        registers.write(
            instruction.rt,
            static_cast<uint32_t>(value));
        break;
    }
    case 0x25:
    {
        // lhu rt = signExt(MEM[rs + imm][15:0])
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint16_t value = dataM.loadHalf(add);

        registers.write(
            instruction.rt,
            static_cast<uint32_t>(value));

        break;
    }
    case 0x28:
    {
        // sb MEM[rs + imm][7:0] = rt [7:0]
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint32_t rtVal = registers.read(instruction.rt);

        dataM.storeByte(
            add,
            static_cast<uint8_t>(rtVal & 0xFF));
        break;
    }
    case 0x29:
    {
        // sh MEM[rs + imm][15:0] = rt [15:0]
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint32_t rtVal = registers.read(instruction.rt);

        dataM.storeHalf(
            add,
            static_cast<uint16_t>(rtVal & 0xFFFF));
        break;
    }
    case 0X2B:
    {
        // sw MEM[rs + imm] = rt
        int32_t offs = signedExtend16(instruction.immediate);
        uint32_t add = rsVal + static_cast<uint32_t>(offs);

        uint32_t rtVal = registers.read(instruction.rt);

        dataM.storeWord(add, rtVal);
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
