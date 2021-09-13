#include "CW33300.h"
#include "Utils.h"
#include "CXD8530BQ.h"
#include "Playstation.h"
#include "Memory.h"

CW33300::Registers::reg_t& CW33300::Registers::R(std::uint8_t index)
{
    switch (index) 
    {
    case 0:
        zero = 0;
        return zero;
    case 1:
        return at;
    case 2:
    case 3:
        return v[index - 2];
    case 4:
    case 5:
    case 6:
    case 7:
        return a[index - 4];
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        return t[index - 8];
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
        return s[index - 16];
    case 24:
    case 25:
        return t[8 + index - 24];
    case 26:
    case 27:
        return k[index - 26];
    case 28:
        return gp;
    case 29:
        return sp;
    case 30:
        return fp;
    case 31:
        return ra;
    default:
        throw;
    }
}

CW33300::Registers::reg_t CW33300::Registers::R_get(std::uint8_t index)
{
    return R(index);
}

void CW33300::Registers::R_set(std::uint8_t index, CW33300::Registers::reg_t value)
{
    R(index) = value;
    zero = 0;
}

void CW33300::ProcessNextInstruction()
{
    std::uint32_t instruction = cpu()->playstation()->bios()->Read32(0);
    Opcode opcode(instruction);
    registers.pc += 4;
    
    if (opcode.op == 0x00)
    {
        //special
    }
    else if(opcode.op & 0b010000)
    {
        //coprocessor
    }
    else
    {
        //map to thing
    }


}

//****** Instructions ******//
#define R_GET(name) auto name = registers.R_get(op.name)
#define R_SET(name, val) registers.R_set(op.name, val);
void CW33300::op_add(const Opcode& op)
{
    R_GET(rs);
    R_GET(rt);

    //check for overflow or underflow
        //throw exception?

    R_SET(rd, rs + rt);
}

void CW33300::op_addu(const Opcode& op)
{
    R_GET(rs);
    R_GET(rt);

    R_SET(rd, rs + rt);
}

void CW33300::op_sub(const Opcode& op)
{
    R_GET(rs);
    R_GET(rt);

    //check for overflow or underflow
        //throw exception?

    R_SET(rd, rs - rt);
}

void CW33300::op_subu(const Opcode& op)
{
    R_GET(rs);
    R_GET(rt);

    R_SET(rd, rs - rt);
}

void CW33300::op_addi(const Opcode& op)
{
    R_GET(rs);

    // check for overflow or underflow
        //throw exception?

    R_SET(rt, rs + op.imm);
}

void CW33300::op_addiu(const Opcode& op)
{
    R_GET(rs);

    R_SET(rt, rs + op.imm);
}

#undef R_GET
#undef R_SET
//****** Instructions ******//