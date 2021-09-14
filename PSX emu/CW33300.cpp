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

void CW33300::Registers::R_set(std::uint8_t index, reg_t value)
{
	R(index) = value;
	zero = 0;
}

ProcessorInstruction* CW33300::MapInstruction(const Opcode& opcode)
{
	if (opcode.op == 0x00)
	{
		//clamp to bounds
		size_t op = opcode.func;
		if (specialInstructionMap.size() <= op)
			op = specialInstructionMap.size() - 1;

		return &specialInstructionMap[op];
	}
	else if (opcode.op == 0x01)
	{
		//clamp to bounds
		size_t op = opcode.rt;
		if (specialInstructionMap.size() <= op)
			op = specialInstructionMap.size() - 1;

		return &specialInstructionMap[op];
	}
	else
	{
		//clamp to bounds
		size_t op = opcode.op;
		if (specialInstructionMap.size() <= op)
			op = specialInstructionMap.size() - 1;

		return &specialInstructionMap[op];
	}
}

void CW33300::ProcessNextInstruction()
{
	std::uint32_t instruction = cpu()->memInterface()->Read32(registers.pc);
	registers.pc += 4;

	Opcode opcode(instruction);
	ProcessorInstruction* instructionRef = MapInstruction(opcode);
	std::int8_t opResult = instructionRef->instruction(opcode);
}

CW33300::CW33300(CXD8530BQ* cpu) : Processor(cpu)
{
#define INST(name) ProcessorInstruction(#name, [this](Opcode opcode)->std::int8_t { return op_##name(opcode); })
	instructionMap =
	{
		/*  00h=SPECIAL 08h=ADDI  10h=COP0 18h=N/A   20h=LB   28h=SB   30h=LWC0 38h=SWC0
			01h=BcondZ  09h=ADDIU 11h=COP1 19h=N/A   21h=LH   29h=SH   31h=LWC1 39h=SWC1
			02h=J       0Ah=SLTI  12h=COP2 1Ah=N/A   22h=LWL  2Ah=SWL  32h=LWC2 3Ah=SWC2
			03h=JAL     0Bh=SLTIU 13h=COP3 1Bh=N/A   23h=LW   2Bh=SW   33h=LWC3 3Bh=SWC3
			04h=BEQ     0Ch=ANDI  14h=N/A  1Ch=N/A   24h=LBU  2Ch=N/A  34h=N/A  3Ch=N/A
			05h=BNE     0Dh=ORI   15h=N/A  1Dh=N/A   25h=LHU  2Dh=N/A  35h=N/A  3Dh=N/A
			06h=BLEZ    0Eh=XORI  16h=N/A  1Eh=N/A   26h=LWR  2Eh=SWR  36h=N/A  3Eh=N/A
			07h=BGTZ    0Fh=LUI   17h=N/A  1Fh=N/A   27h=N/A  2Fh=N/A  37h=N/A  3Fh=N/A    */
		INST(invalid), INST(invalid), INST(j), INST(jal), INST(beq), INST(bne), INST(blez), INST(bgtz),
		INST(addi), INST(addiu), INST(slti), INST(sltiu), INST(andi), INST(ori), INST(xori), INST(lui),
		INST(copn), INST(copn), INST(copn), INST(copn), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(lb), INST(lh), INST(lwl), INST(lw), INST(lbu), INST(lhu), INST(lwr), INST(invalid),
		INST(sb), INST(sh), INST(swl), INST(sw), INST(invalid), INST(invalid), INST(swr), INST(invalid),
		INST(lwcn), INST(lwcn), INST(lwcn), INST(lwcn), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(swcn), INST(swcn), INST(swcn), INST(swcn), INST(invalid), INST(invalid), INST(invalid), INST(invalid)

	};

	branchInstructionMap =
	{
		/*  000001 | rs   | 00000| <--immediate16bit--> | bltz
			000001 | rs   | 00001| <--immediate16bit--> | bgez
			000001 | rs   | 10000| <--immediate16bit--> | bltzal
			000001 | rs   | 10001| <--immediate16bit--> | bgezal  */
		INST(bltz), INST(bgez), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(bltzal), INST(bgezal), INST(invalid), INST(invalid)
		//if crash, fill with invalid up to index 31 or clamp to bounds
	};

	specialInstructionMap =
	{
		/*  00h=SLL   08h=JR      10h=MFHI 18h=MULT  20h=ADD  28h=N/A  30h=N/A  38h=N/A
			01h=N/A   09h=JALR    11h=MTHI 19h=MULTU 21h=ADDU 29h=N/A  31h=N/A  39h=N/A
			02h=SRL   0Ah=N/A     12h=MFLO 1Ah=DIV   22h=SUB  2Ah=SLT  32h=N/A  3Ah=N/A
			03h=SRA   0Bh=N/A     13h=MTLO 1Bh=DIVU  23h=SUBU 2Bh=SLTU 33h=N/A  3Bh=N/A
			04h=SLLV  0Ch=SYSCALL 14h=N/A  1Ch=N/A   24h=AND  2Ch=N/A  34h=N/A  3Ch=N/A
			05h=N/A   0Dh=BREAK   15h=N/A  1Dh=N/A   25h=OR   2Dh=N/A  35h=N/A  3Dh=N/A
			06h=SRLV  0Eh=N/A     16h=N/A  1Eh=N/A   26h=XOR  2Eh=N/A  36h=N/A  3Eh=N/A
			07h=SRAV  0Fh=N/A     17h=N/A  1Fh=N/A   27h=NOR  2Fh=N/A  37h=N/A  3Fh=N/A   */
		INST(sll), INST(invalid), INST(srl), INST(sra), INST(sllv), INST(invalid), INST(srlv), INST(srav),
		INST(jr), INST(jalr), INST(invalid), INST(invalid), INST(syscall), INST(break), INST(invalid), INST(invalid),
		INST(mfhi), INST(mthi), INST(mflo), INST(mtlo), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(mult), INST(multu), INST(div), INST(divu), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(add), INST(addu), INST(sub), INST(subu), INST(and), INST(or ), INST(xor), INST(nor),
		INST(invalid), INST(invalid), INST(slt), INST(sltu), INST(invalid)
	};
#undef INST
}

//****** Instructions ******//
//Get/Set functions for the register pointed to by the opcode part.
#define R_GET(name) auto (name) = registers.R_get(op.name)
#define R_SET(name, val) registers.R_set(op.name, val)
std::int8_t CW33300::op_add(const Opcode& op)
{
	auto (rs) = registers.R_get(op.rs);
	R_GET(rt);

	//check for overflow or underflow
		//throw exception?

	R_SET(rd, rs + rt);
	return 0;
}

std::int8_t CW33300::op_addu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, rs + rt);
	return 0;
}

std::int8_t CW33300::op_sub(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	//check for overflow or underflow
		//throw exception?

	R_SET(rd, rs - rt);
	return 0;
}

std::int8_t CW33300::op_subu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, rs - rt);
	return 0;
}

std::int8_t CW33300::op_addi(const Opcode& op)
{
	R_GET(rs);

	// check for overflow or underflow
		//throw exception?

	R_SET(rt, rs + op.imm);
	return 0;
}

std::int8_t CW33300::op_addiu(const Opcode& op)
{
	R_GET(rs);
	R_SET(rt, rs + op.imm);
	return 0;
}

std::int8_t CW33300::op_slt(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	auto rs_signed = std::int32_t(rs);
	auto rt_signed = std::int32_t(rt);

	R_SET(rd, (rs_signed < rt_signed) ? 1 : 0);
	
	return 0;
}

std::int8_t CW33300::op_sltu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, (rs < rt) ? 1 : 0);

	return 0;
}

std::int8_t CW33300::op_slti(const Opcode& op)
{
	R_GET(rs);

	auto rs_signed = std::int32_t(rs);

	R_SET(rt, (rs_signed < std::int16_t(op.imm) ? 1 : 0));

	return 0;
}

std::int8_t CW33300::op_sltiu(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, (rs < op.imm) ? 1 : 0);
	
	return 0;
}

std::int8_t CW33300::op_and(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, rs & rt);
	return 0;
}

std::int8_t CW33300::op_or(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, rs | rt);
	return 0;
}

std::int8_t CW33300::op_xor(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, rs ^ rt);
	return 0;
}

std::int8_t CW33300::op_nor(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, 0xFFFFFFFF ^ (rs | rt));
	return 0;
}

std::int8_t CW33300::op_andi(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, rs & op.imm);
	return 0;
}

std::int8_t CW33300::op_ori(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, rs | op.imm);
	return 0;
}

std::int8_t CW33300::op_xori(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, rs ^ op.imm);
	return 0;
}

std::int8_t CW33300::op_sllv(const Opcode& op)
{
	R_GET(rt);
	R_GET(rs);

	R_SET(rd, rt << (rs & 0x1F));

	return 0;
}

std::int8_t CW33300::op_srlv(const Opcode& op)
{
	R_GET(rt);
	R_GET(rs);

	R_SET(rd, rt >> (rs & 0x1F));

	return 0;
}

std::int8_t CW33300::op_srav(const Opcode& op)
{
	R_GET(rt);
	R_GET(rs);

	R_SET(rd, std::int32_t(rt) >> (rs & 0x1F));
	return 0;
}

std::int8_t CW33300::op_sll(const Opcode& op)
{
	R_GET(rt);

	R_SET(rd, rt << op.shift);
	return 0;
}

std::int8_t CW33300::op_srl(const Opcode& op)
{
	R_GET(rt);

	R_SET(rd, rt >> op.shift);
	return 0;
}

std::int8_t CW33300::op_sra(const Opcode& op)
{
	R_GET(rt);

	R_SET(rd, std::int32_t(rt) >> op.shift);
	return 0;
}

std::int8_t CW33300::op_lui(const Opcode& op)
{
	R_SET(rt, op.imm << 16);

	return 0;
}

std::int8_t CW33300::op_mult(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	std::int32_t rss = std::int32_t(rs);
	std::int32_t rts = std::int32_t(rt);

	std::int64_t result = rss * rts;

	registers.lo = result & 0x7FFFFFFF;
	registers.hi = result >> 32;

	return 0;
}

std::int8_t CW33300::op_multu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);
	
	std::uint64_t result = rs * rt;

	registers.lo = result & 0x7FFFFFFF;
	registers.hi = result >> 32;

	return 0;
}

std::int8_t CW33300::op_div(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	std::int32_t rss = std::int32_t(rs);
	std::int32_t rts = std::int32_t(rt);

	if(rts == 0)
	{
		registers.lo = (rss >= 0) ? 0xffffffff : 1;
		registers.hi = rss;
	}
	else if(rs == 0x80000000 && rts == -1)
	{
		registers.lo = 0x80000000;
		registers.hi = 0;
	}
	else
	{
		registers.lo = rss / rts;
		registers.hi = rs % rt;
	}

	return 0;
}

std::int8_t CW33300::op_divu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	if (rs == 0)
	{
		registers.lo = 0xffffffff;
		registers.hi = rs;
	}
	else
	{
		registers.lo = rs / rt;
		registers.hi = rs % rt;
	}

	return 0;
}

std::int8_t CW33300::op_mfhi(const Opcode& op)
{
	R_SET(rd, registers.hi);

	return 0;
}

std::int8_t CW33300::op_mflo(const Opcode& op)
{
	R_SET(rd, registers.lo);

	return 0;
}

std::int8_t CW33300::op_mthi(const Opcode& op)
{
	R_GET(rs);
	registers.hi = rs;

	return 0;
}

std::int8_t CW33300::op_mtlo(const Opcode& op)
{
	R_GET(rs);
	registers.lo = rs;

	return 0;
}

std::int8_t CW33300::op_lb(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_lbu(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_lh(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_lhu(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_lw(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_lwl(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_lwr(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_sb(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_sh(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_sw(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_swr(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_swl(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_j(const Opcode& op)
{
	ProcessNextInstruction();
	return 1;
}

std::int8_t CW33300::op_jal(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_jr(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_jalr(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_beq(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_bne(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_bltz(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_bgez(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_bgtz(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_blez(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_bltzal(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_bgezal(const Opcode& op)
{

	return 1;
}

std::int8_t CW33300::op_syscall(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_break(const Opcode& op)
{

	return 0;
}

std::int8_t CW33300::op_copn(const Opcode& op)
{
	switch (op.op & 0b011)
	{
	default:
	case 0: //cop0
		break;
	case 1: //cop1
		break;
	case 2: //cop2
		break;
	case 3: //cop3
		break;
	}
	return 0;
}

std::int8_t CW33300::op_lwcn(const Opcode& op)
{
	switch (op.op & 0b011)
	{
	default:
	case 0: //lwc0
		break;
	case 1: //lwc1
		break;
	case 2: //lwc2
		break;
	case 3: //lwc3
		break;
	}
	return 0;
}

std::int8_t CW33300::op_swcn(const Opcode& op)
{
	switch (op.op & 0b011)
	{
	default:
	case 0: //swc0
		break;
	case 1: //swc1
		break;
	case 2: //swc2
		break;
	case 3: //swc3
		break;
	}
	return 0;
}

std::int8_t CW33300::op_invalid(const Opcode& op)
{
	//invalid instruction, do something!!!!!!
	__debugbreak();
	return 0;
}

#undef R_GET
#undef R_SET
//****** Instructions ******//