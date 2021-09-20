#include "COP0.h"
#include "CXD8530BQ.h"
#include "DebugUtils.h"
#include <iostream>

#define C_GET(name) auto (c_##name) = cpu()->cw33300()->GetRegister(op.name)
#define C_SET(name, val) cpu()->cw33300()->SetRegister(op.name, val)

std::int8_t COP0::op_mfc(const Opcode& op)
{
	R_GET(rd);
	C_SET(rt, rd);
	return 0;
}

std::int8_t COP0::op_cfc(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_mtc(const Opcode& op)
{
	C_GET(rt);
	R_SET(rd, c_rt);
	return 0;
}

std::int8_t COP0::op_ctc(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_lwc(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_swc(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_bcf(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_bct(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_rfe(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::int8_t COP0::op_invalid(const Opcode& op)
{
	__debugbreak();
	return 0;
}

std::uint32_t COP0::GetRegister(std::uint8_t index) const
{
	return _registers_read[index];
}

void COP0::SetRegister(std::uint8_t index, std::uint32_t value)
{
	_registers_write[index] = value;
}

void COP0::ExecuteInstruction(Opcode opcode)
{
	ProcessorInstruction* instructionRef = DecodeInstruction(opcode);

#if DEBUG_LOG_ENABLED
	std::cout << "	[" << instructionRef->name << "]\n";
#endif

	std::int8_t opResult = instructionRef->instruction(opcode);
	Processor::ExecuteInstruction(opcode);
}

ProcessorInstruction* COP0::DecodeInstruction(const Opcode& opcode)
{
	/*	  31..26 |25..21|20..16|15..11|10..6 |  5..0  |
	   6bit  | 5bit | 5bit | 5bit | 5bit |  6bit  |
	  -------+------+------+------+------+--------+------------
	  0100nn |0|0000| rt   | rd   | N/A  | 000000 | MFCn rt,rd_dat  ;rt = dat	idx:0
	  0100nn |0|0010| rt   | rd   | N/A  | 000000 | CFCn rt,rd_cnt  ;rt = cnt	idx:1
	  0100nn |0|0100| rt   | rd   | N/A  | 000000 | MTCn rt,rd_dat  ;dat = rt	idx:2
	  0100nn |0|0110| rt   | rd   | N/A  | 000000 | CTCn rt,rd_cnt  ;cnt = rt	idx:3
	  0100nn |0|1000|00000 | <--immediate16bit--> | BCnF target ;jump if false	idx:4
	  0100nn |0|1000|00001 | <--immediate16bit--> | BCnT target ;jump if true	idx:5
	  0100nn |1| <--------immediate25bit--------> | COPn imm25					unused
	  010000 |1|0000| N/A  | N/A  | N/A  | 000001 | COP0 01h  ;=TLBR			unused
	  010000 |1|0000| N/A  | N/A  | N/A  | 000010 | COP0 02h  ;=TLBWI			unused
	  010000 |1|0000| N/A  | N/A  | N/A  | 000110 | COP0 06h  ;=TLBWR			unused
	  010000 |1|0000| N/A  | N/A  | N/A  | 001000 | COP0 08h  ;=TLBP			unused
	  010000 |1|0000| N/A  | N/A  | N/A  | 010000 | COP0 10h  ;=RFE				idx:6
	  1100nn | rs   | rt   | <--immediate16bit--> | LWCn rt_dat,[rs+imm]		handled on main processor
	  1110nn | rs   | rt   | <--immediate16bit--> | SWCn rt_dat,[rs+imm]		handled on main processor */

	if (opcode.rs & 0b10000) //tlbx and rfe
	{
		switch (opcode.func)
		{
		case 0b010000: //rfe
			return &_instructionMap[6];
		default:
			return nullptr;
		}
	}

	switch (opcode.rs)
	{
	case 0b0000: //mfc
		return &_instructionMap[0];
	case 0b0010: //cfc
		return &_instructionMap[1];
	case 0b0100: //mtc
		return &_instructionMap[2];
	case 0b0110: //ctc
		return &_instructionMap[3];
	case 0b1000: //bcf/bct
		return &_instructionMap[opcode.rt == 0 ? 4 : 5];
	default:
		return nullptr;
	}
}

COP0::COP0(CXD8530BQ* cpu) : Processor(cpu)
{
	_registers_read.resize(64, 0xbadbad);
	_registers_write.resize(64, 0xbadbad);

	/*	  31..26 |25..21|20..16|15..11|10..6 |  5..0  |
		   6bit  | 5bit | 5bit | 5bit | 5bit |  6bit  |
		  -------+------+------+------+------+--------+------------
		  0100nn |0|0000| rt   | rd   | N/A  | 000000 | MFCn rt,rd_dat  ;rt = dat	idx:0
		  0100nn |0|0010| rt   | rd   | N/A  | 000000 | CFCn rt,rd_cnt  ;rt = cnt	idx:1
		  0100nn |0|0100| rt   | rd   | N/A  | 000000 | MTCn rt,rd_dat  ;dat = rt	idx:2
		  0100nn |0|0110| rt   | rd   | N/A  | 000000 | CTCn rt,rd_cnt  ;cnt = rt	idx:3
		  0100nn |0|1000|00000 | <--immediate16bit--> | BCnF target ;jump if false	idx:4
		  0100nn |0|1000|00001 | <--immediate16bit--> | BCnT target ;jump if true	idx:5
		  0100nn |1| <--------immediate25bit--------> | COPn imm25					unused
		  010000 |1|0000| N/A  | N/A  | N/A  | 000001 | COP0 01h  ;=TLBR			unused
		  010000 |1|0000| N/A  | N/A  | N/A  | 000010 | COP0 02h  ;=TLBWI			unused
		  010000 |1|0000| N/A  | N/A  | N/A  | 000110 | COP0 06h  ;=TLBWR			unused
		  010000 |1|0000| N/A  | N/A  | N/A  | 001000 | COP0 08h  ;=TLBP			unused
		  010000 |1|0000| N/A  | N/A  | N/A  | 010000 | COP0 10h  ;=RFE				idx:6
		  1100nn | rs   | rt   | <--immediate16bit--> | LWCn rt_dat,[rs+imm]		handled on main processor
		  1110nn | rs   | rt   | <--immediate16bit--> | SWCn rt_dat,[rs+imm]		handled on main processor */
#define INST(name) ProcessorInstruction(#name, [this](Opcode opcode)->std::int8_t { return op_##name(opcode); })
	_instructionMap =
	{
		INST(mfc), INST(cfc), INST(mtc), INST(ctc), INST(bcf), INST(bct), INST(rfe),
	};
}
