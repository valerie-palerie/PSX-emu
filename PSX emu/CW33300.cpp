#include "CW33300.h"

#include <bitset>
#include <iostream>

#include "DebugUtils.h"
#include "MathUtils.h"
#include "CXD8530BQ.h"
#include "Playstation.h"
#include "Memory.h"
#include "MemoryInterface.h"


//****** Instructions ******//
//Get/Set functions for the register pointed to by the opcode part.
#define CR_GET(cop, name) auto (c##cop##_##name) = GetCoprocessor(cop)->GetRegister(op.name)
#define CR_SET(cop, name, val) GetCoprocessor(cop)->SetRegister(op.name, val)
std::int8_t CW33300::op_add(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);
	std::uint32_t result = rs + rt;
	if (Math::DetectOverflowAdd(rs, rt, result))
	{
		RaiseException(op, ExceptionType::Overflow);
	}
	else
	{
		R_SET(rd, result);
	}

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
	std::uint32_t result = rs - rt;
	if (Math::DetectOverflowSubtract(rs, rt, result))
	{
		RaiseException(op, ExceptionType::Overflow);
	}
	else
	{
		R_SET(rd, result);
	}

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

	std::uint32_t result = rs + op.imm_se;
	if (Math::DetectOverflowAdd(rs, op.imm_se, result))
	{
		RaiseException(op, ExceptionType::Overflow);
	}
	else
	{
		R_SET(rt, result);
	}

	return 0;
}

std::int8_t CW33300::op_addiu(const Opcode& op)
{
	R_GET(rs);
	R_SET(rt, rs + op.imm_se);
	return 0;
}

std::int8_t CW33300::op_slt(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	R_SET(rd, (std::int32_t(rs) < std::int32_t(rt)) ? 1 : 0);

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

	R_SET(rt, (std::int32_t(rs) < std::int32_t(op.imm_se)) ? 1 : 0);

	return 0;
}

std::int8_t CW33300::op_sltiu(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, (rs < op.imm_se) ? 1 : 0);

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

	R_SET(rd, ~(rs | rt));
	return 0;
}

std::int8_t CW33300::op_andi(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, rs & op.imm_ze);
	return 0;
}

std::int8_t CW33300::op_ori(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, rs | op.imm_ze);
	return 0;
}

std::int8_t CW33300::op_xori(const Opcode& op)
{
	R_GET(rs);

	R_SET(rt, rs ^ op.imm_ze);
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

	R_SET(rd, std::uint32_t(std::int32_t(rt) >> op.shift));
	return 0;
}

std::int8_t CW33300::op_sllv(const Opcode& op)
{
	R_GET(rt);
	R_GET(rs);

	R_SET(rd, rt << (rs & 0x1f));

	return 0;
}

std::int8_t CW33300::op_srlv(const Opcode& op)
{
	R_GET(rt);
	R_GET(rs);

	R_SET(rd, rt >> (rs & 0x1f));

	return 0;
}

std::int8_t CW33300::op_srav(const Opcode& op)
{
	R_GET(rt);
	R_GET(rs);

	R_SET(rd, std::uint32_t(std::int32_t(rt) >> (rs & 0x1F)));
	return 0;
}

std::int8_t CW33300::op_lui(const Opcode& op)
{
	R_SET(rt, op.imm_ze << 16);

	return 0;
}

std::int8_t CW33300::op_mult(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	std::uint64_t result = std::uint64_t(std::int64_t(std::int32_t(rs)) * std::int64_t(std::int32_t(rt)));

	_r_lo = std::uint32_t(result);
	_r_hi = std::uint32_t(result >> 32);

	return 0;
}

std::int8_t CW33300::op_multu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	std::uint64_t result = std::uint64_t(rs) * std::uint64_t(rt);

	_r_lo = std::uint32_t(result);
	_r_hi = std::uint32_t(result >> 32);

	return 0;
}

std::int8_t CW33300::op_div(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	auto rss = std::int32_t(rs);
	auto rts = std::int32_t(rt);

	if (rts == 0)
	{
		_r_lo = (rss >= 0) ? 0xffffffff : 1;
		_r_hi = rs;
	}
	else if (rs == 0x80000000 && rts == -1)
	{
		_r_lo = 0x80000000;
		_r_hi = 0;
	}
	else
	{
		_r_lo = rss / rts;
		_r_hi = rs % rt;
	}

	return 0;
}

std::int8_t CW33300::op_divu(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);

	if (rs == 0)
	{
		_r_lo = 0xffffffff;
		_r_hi = rs;
	}
	else
	{
		_r_lo = rs / rt;
		_r_hi = rs % rt;
	}

	return 0;
}

std::int8_t CW33300::op_mfhi(const Opcode& op)
{
	R_SET(rd, _r_hi);

	return 0;
}

std::int8_t CW33300::op_mflo(const Opcode& op)
{
	R_SET(rd, _r_lo);

	return 0;
}

std::int8_t CW33300::op_mthi(const Opcode& op)
{
	R_GET(rs);
	_r_hi = rs;

	return 0;
}

std::int8_t CW33300::op_mtlo(const Opcode& op)
{
	R_GET(rs);
	_r_lo = rs;

	return 0;
}

std::int8_t CW33300::op_lb(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	R_GET(rs);
	std::uint8_t val = cpu()->playstation()->memInterface()->Read8(op.imm_se + rs);
	R_SET(rt, std::int32_t(std::int8_t(val)));

	return 0;
}

std::int8_t CW33300::op_lbu(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	R_GET(rs);
	R_SET(rt, cpu()->playstation()->memInterface()->Read8(op.imm_se + rs));

	return 0;
}

std::int8_t CW33300::op_lh(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	R_GET(rs);
	std::uint16_t val = cpu()->playstation()->memInterface()->Read16(op.imm_se + rs);
	R_SET(rt, std::int32_t(std::int16_t(val)));

	return 0;
}

std::int8_t CW33300::op_lhu(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	R_GET(rs);
	R_SET(rt, cpu()->playstation()->memInterface()->Read16(op.imm_se + rs));

	return 0;
}

std::int8_t CW33300::op_lw(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	R_GET(rs);
	R_SET(rt, cpu()->playstation()->memInterface()->Read32(op.imm_se + rs));

	return 0;
}

std::int8_t CW33300::op_lwl(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();

	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	return 0;
}

std::int8_t CW33300::op_lwr(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache read
		return 0;
	}

	return 0;
}

std::int8_t CW33300::op_sb(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache write
		return 0;
	}

	R_GET(rs);
	R_GET(rt);
	cpu()->playstation()->memInterface()->Write8(op.imm_se + rs, std::uint8_t(rt & 0xff));
	return 0;
}

std::int8_t CW33300::op_sh(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache write
		return 0;
	}

	R_GET(rs);
	R_GET(rt);
	cpu()->playstation()->memInterface()->Write16(op.imm_se + rs, std::uint16_t(rt & 0xffff));
	return 0;
}

std::int8_t CW33300::op_sw(const Opcode& op)
{
	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache write
		return 0;
	}

	R_GET(rs);
	R_GET(rt);
	cpu()->playstation()->memInterface()->Write32(op.imm_se + rs, rt);
	return 0;
}

std::int8_t CW33300::op_swr(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();

	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache write
		return 0;
	}

	return 0;
}

std::int8_t CW33300::op_swl(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();

	if (GetCoprocessor(0)->GetRegister(12) & 0x10000)
	{
		//cache write
		return 0;
	}
	return 0;
}

std::int8_t CW33300::op_j(const Opcode& op)
{
	Jump((_r_pc & 0xf0000000) | (op.cop << 2));
	return 0;
}

std::int8_t CW33300::op_jal(const Opcode& op)
{
	SetRegister(31, _r_pc + 4);
	return op_j(op);
}

std::int8_t CW33300::op_jr(const Opcode& op)
{
	R_GET(rs);
	Jump(rs);
	return 0;
}

std::int8_t CW33300::op_jalr(const Opcode& op)
{
	R_SET(rd, _r_pc + 4);
	R_GET(rs);
	Jump(rs);
	return 0;
}

std::int8_t CW33300::op_beq(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);
	if (rs == rt)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}
	return 0;
}

std::int8_t CW33300::op_bne(const Opcode& op)
{
	R_GET(rs);
	R_GET(rt);
	if (rs != rt)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}
	return 0;
}

std::int8_t CW33300::op_bltz(const Opcode& op)
{
	R_GET(rs);
	if (std::int32_t(rs) < 0)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}

	return 0;
}

std::int8_t CW33300::op_bgez(const Opcode& op)
{
	R_GET(rs);
	if (std::int32_t(rs) >= 0)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}

	return 0;
}

std::int8_t CW33300::op_bgtz(const Opcode& op)
{
	R_GET(rs);
	if (std::int32_t(rs) > 0)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}

	return 0;
}

std::int8_t CW33300::op_blez(const Opcode& op)
{
	R_GET(rs);
	if (std::int32_t(rs) <= 0)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}

	return 0;
}

std::int8_t CW33300::op_bltzal(const Opcode& op)
{
	R_GET(rs);
	SetRegister(31, _r_pc + 4);
	if (std::int32_t(rs) < 0)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}

	return 0;
}

std::int8_t CW33300::op_bgezal(const Opcode& op)
{
	R_GET(rs);
	SetRegister(31, _r_pc + 4);
	if (std::int32_t(rs) >= 0)
	{
		Jump(_r_pc + (op.imm_se << 2));
	}

	return 0;
}

std::int8_t CW33300::op_syscall(const Opcode& op)
{
	RaiseException(op, ExceptionType::Syscall);
	return 0;
}

std::int8_t CW33300::op_break(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();
	return 0;
}

std::int8_t CW33300::op_copn(const Opcode& op)
{
	Processor* coprocessor = GetCoprocessor(op.op & 0b011);
	if (coprocessor != nullptr)
		coprocessor->ExecuteInstruction(op);
	else
		__debugbreak();

	return 0;
}

//LWCn and SWCn might need to be implemented on the coprocessor, i'll get back to this when i know more.
std::int8_t CW33300::op_lwcn(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();
	//Processor* coprocessor = GetCoprocessor(op.op & 0b011);
	//if (coprocessor != nullptr)
	//	coprocessor->ExecuteInstruction(op);

	return 0;
}

std::int8_t CW33300::op_swcn(const Opcode& op)
{
	//-UNIMPLEMENTED
	__debugbreak();
	//Processor* coprocessor = GetCoprocessor(op.op & 0b011);
	//if (coprocessor != nullptr)
	//	coprocessor->ExecuteInstruction(op);

	return 0;
}

std::int8_t CW33300::op_invalid(const Opcode& op)
{
	RaiseException(op, ExceptionType::RI);
	return 0;
}

void CW33300::Init()
{
}


std::uint32_t CW33300::GetRegister(std::uint8_t index) const
{
	return _registers_read[index];
}

void CW33300::SetRegister(std::uint8_t index, std::uint32_t value)
{
	_registers_write[index] = value;
	_registers_write[0] = 0;
}


void CW33300::ExecuteInstruction(Opcode opcode)
{
	ProcessorInstruction* instructionRef = DecodeInstruction(opcode);

#if DEBUG_LOG_ENABLED
	Debug::LogInstruction(this, opcode, instructionRef, _currentInstructionAddress);
	bool didDebugBreak = false;
#endif

#if _DEBUG
	for (const auto& condition : _debugConditions)
	{
		if (condition->EvaluateCondition(this, opcode, instructionRef, _currentInstructionAddress))
		{
#if DEBUG_LOG_ENABLED
			didDebugBreak = true;
#endif
			//cpu()->playstation()->dram()->DumpToFile("ramdump.txt");
			__debugbreak();
			break;
		}
	}
#endif

	std::int8_t opResult = instructionRef->instruction(opcode);

#if DEBUG_LOG_ENABLED
	Debug::LogRegisterWrites(_registers_read, _registers_write);
	if (didDebugBreak)
	{
		std::cout << "------------ COND BREAK ------------\n\n";
	}
#endif

	Processor::ExecuteInstruction(opcode);
}

Processor* CW33300::GetCoprocessor(std::uint8_t idx) const
{
	switch (idx)
	{
	case 0:
		return cpu()->cop0();
	case 2:
		return cpu()->gte();
	default:
		return nullptr;
	}
}

void CW33300::MoveExecutionTo(std::uint32_t address)
{
	_r_pc = address;
	_r_npc = address + 4;
}

ProcessorInstruction* CW33300::DecodeInstruction(const Opcode& opcode)
{
	//Bits 31-26 being 0x0 suggests we should look at bits 5-0 where a special instruction is encoded.
	if (opcode.op == 0x00)
	{
		//Clamp map lookup to the end of map, handles invalid instructions at the end of the mapping that i don't wanna manually add.
		return &_specialInstructionMap[std::min(size_t(opcode.func), _specialInstructionMap.size() - 1)];
	}

	//Branching instructions that are marked with 0x1 on bits 31-26 and are differentiated with bits 20-16
	if (opcode.op == 0x01)
	{
		return &_branchInstructionMap[std::min(size_t(opcode.rt), _branchInstructionMap.size() - 1)];
	}

	//All other instructions are mapped normally in bits 31-26
	return &_instructionMap[std::min(size_t(opcode.op), _instructionMap.size() - 1)];
}

void CW33300::Jump(std::uint32_t address)
{
	_delaySlotAddress = _r_pc;
	_r_npc = address;
}

void CW33300::RaiseException(const Opcode& opcode, ExceptionType exceptionType)
{
	COP0* cop0 = cpu()->cop0();
	bool isBranchDelay = isExecutingDelaySlot();

	//If we're executing a branch delay slot, we have to return to the branch instruction before it
	cop0->SetRegister(14, isBranchDelay ? (_currentInstructionAddress - 4) : _currentInstructionAddress);

	std::uint32_t causeReg = cop0->GetRegister(13);
	causeReg = Math::SetBits(causeReg, 6, 2, std::uint32_t(exceptionType));
	causeReg = Math::SetBit(causeReg, 31, isBranchDelay);
	cop0->SetRegister(13, causeReg);

	std::uint32_t handler =
		Math::IsBitSet(cop0->GetRegister(12), 22)
		? 0xbfc00180
		: 0x80000080;

	MoveExecutionTo(handler);
}

void CW33300::ProcessNextInstruction()
{
	_currentInstructionAddress = _r_pc;
	Opcode opcode(cpu()->playstation()->memInterface()->Read32(_currentInstructionAddress));
	_r_pc = _r_npc;
	_r_npc += 4;

	ExecuteInstruction(opcode);

	if (isExecutingDelaySlot())
		_delaySlotAddress = 0x0;
}


CW33300::CW33300(CXD8530BQ* cpu) : Processor(cpu)
{
	//Initialize with 0xbadbad to make it clear what's uninitialized memory and what isn't.
	_registers_read.resize(32, 0xbadbad);
	_registers_write.resize(32, 0xbadbad);
	_registers_read[0] = 0;
	_registers_write[0] = 0;
	_r_pc = MemoryMap::BIOS_BASE;
	_r_npc = MemoryMap::BIOS_BASE + 4;

#if _DEBUG
	/*
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("lui", 0x3c080013, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("ori", 0x3508243f, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sw", 0xac281010, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sll", 0x00000000, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("addiu", 0x24080b88, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("j", 0x0bf00054, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("or", 0x00000825, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("copn", 0x408c6000, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("bne", 0x154bfff7, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("addi", 0x214a0080, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("lw", 0x8d090000, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sltu", 0x0043082b, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("addu", 0x03a0f021, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sh", 0xa5200180, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("jal", 0x0ff00698, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("andi", 0x308400ff, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sb", 0xa1c42041, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("jr", 0x03e00008, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("lb", 0x81efe288, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("beq", 0x11e0000c, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("copn", 0x40026000, 1));//won't trigger
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("and", 0x00412024, 1));

	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("add", 0x01094020, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("bgtz", 0x1ca00003, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("blez", 0x18a00005, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("lbu", 0x90ae0000, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("jalr", 0x0100f809, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("slti", 0x28810010, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("subu", 0x01c47023, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sra", 0x00042603, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("div", 0x0061001a, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("mflo", 0x00001812, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("srl", 0x00057082, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("sltiu", 0x2c410045, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("divu", 0x0064001b, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("mfhi", 0x0000c810, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("slt", 0x0338082a, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("syscall", 0x0000000c, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("mtlo", 0x00400013, 1));
	_debugConditions.push_back(std::make_unique<Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature>("mthi", 0x00400011, 1));
	*/
#endif

#define INST(name) ProcessorInstruction(#name, [this](Opcode opcode)->std::int8_t { return op_##name(opcode); })
#define INST_F(name, format, mask) ProcessorInstruction(#name, [this](Opcode opcode)->std::int8_t { return op_##name(opcode); }, InstructionStructure(InstructionFormat::##format, mask))
	//Since operations are encoded in 5 or 6 bit values, just put them in a vector and retrieve by index.

	_instructionMap =
	{
		/*  00h=SPECIAL 08h=ADDI  10h=COP0 18h=N/A   20h=LB   28h=SB   30h=LWC0 38h=SWC0
			01h=BcondZ  09h=ADDIU 11h=COP1 19h=N/A   21h=LH   29h=SH   31h=LWC1 39h=SWC1
			02h=J       0Ah=SLTI  12h=COP2 1Ah=N/A   22h=LWL  2Ah=SWL  32h=LWC2 3Ah=SWC2
			03h=JAL     0Bh=SLTIU 13h=COP3 1Bh=N/A   23h=LW   2Bh=SW   33h=LWC3 3Bh=SWC3
			04h=BEQ     0Ch=ANDI  14h=N/A  1Ch=N/A   24h=LBU  2Ch=N/A  34h=N/A  3Ch=N/A
			05h=BNE     0Dh=ORI   15h=N/A  1Dh=N/A   25h=LHU  2Dh=N/A  35h=N/A  3Dh=N/A
			06h=BLEZ    0Eh=XORI  16h=N/A  1Eh=N/A   26h=LWR  2Eh=SWR  36h=N/A  3Eh=N/A
			07h=BGTZ    0Fh=LUI   17h=N/A  1Fh=N/A   27h=N/A  2Fh=N/A  37h=N/A  3Fh=N/A    */
		INST(invalid), INST(invalid), INST_F(j, J, 0xff), INST_F(jal, J, 0xff), INST_F(beq, I, 0xff), INST_F(bne, I, 0xff), INST_F(blez, I, 0b110111), INST_F(bgtz, I, 0b110111),
		INST_F(addi, I, 0xff), INST_F(addiu, I, 0xff), INST_F(slti, I, 0xff), INST_F(sltiu, I, 0xff), INST_F(andi, I, 0xff), INST_F(ori, I, 0xff), INST_F(xori, I, 0xff), INST_F(lui, I, 0b101111),
		INST_F(copn, J, 0x0), INST_F(copn, J, 0x0), INST_F(copn, J, 0x0), INST_F(copn, J, 0x0), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST_F(lb, I, 0xff), INST_F(lh, I, 0xff), INST_F(lwl, I, 0xff), INST_F(lw, I, 0xff), INST_F(lbu, I, 0xff), INST_F(lhu, I, 0xff), INST_F(lwr, I, 0xff), INST(invalid),
		INST_F(sb, I, 0xff), INST_F(sh, I, 0xff), INST_F(swl, I, 0xff), INST_F(sw, I, 0xff), INST(invalid), INST(invalid), INST(swr), INST(invalid),
		INST(lwcn), INST(lwcn), INST(lwcn), INST(lwcn), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(swcn), INST(swcn), INST(swcn), INST(swcn), INST(invalid), INST(invalid), INST(invalid), INST(invalid)

	};

	_branchInstructionMap =
	{
		/*  000001 | rs   | 00000| <--immediate16bit--> | bltz
			000001 | rs   | 00001| <--immediate16bit--> | bgez
			000001 | rs   | 10000| <--immediate16bit--> | bltzal
			000001 | rs   | 10001| <--immediate16bit--> | bgezal  */
		INST_F(bltz, I, 0b110111), INST_F(bgez, I, 0b110111), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST_F(bltzal, I, 0b110111), INST_F(bgezal, I, 0b110111), INST(invalid), INST(invalid)
	};

	_specialInstructionMap =
	{
		/*  00h=SLL   08h=JR      10h=MFHI 18h=MULT  20h=ADD  28h=N/A  30h=N/A  38h=N/A
			01h=N/A   09h=JALR    11h=MTHI 19h=MULTU 21h=ADDU 29h=N/A  31h=N/A  39h=N/A
			02h=SRL   0Ah=N/A     12h=MFLO 1Ah=DIV   22h=SUB  2Ah=SLT  32h=N/A  3Ah=N/A
			03h=SRA   0Bh=N/A     13h=MTLO 1Bh=DIVU  23h=SUBU 2Bh=SLTU 33h=N/A  3Bh=N/A
			04h=SLLV  0Ch=SYSCALL 14h=N/A  1Ch=N/A   24h=AND  2Ch=N/A  34h=N/A  3Ch=N/A
			05h=N/A   0Dh=BREAK   15h=N/A  1Dh=N/A   25h=OR   2Dh=N/A  35h=N/A  3Dh=N/A
			06h=SRLV  0Eh=N/A     16h=N/A  1Eh=N/A   26h=XOR  2Eh=N/A  36h=N/A  3Eh=N/A
			07h=SRAV  0Fh=N/A     17h=N/A  1Fh=N/A   27h=NOR  2Fh=N/A  37h=N/A  3Fh=N/A   */
		INST_F(sll, R, 0b101111), INST(invalid), INST_F(srl, R, 0b101111), INST_F(sra, R, 0b101111), INST_F(sllv, R, 0b111101), INST(invalid), INST_F(srlv, R, 0b111101), INST_F(srav, R, 0b111101),
		INST_F(jr, R, 0b110001), INST_F(jalr, R, 0b110101), INST(invalid), INST(invalid), INST_F(syscall, R, 0x0), INST_F(break, R, 0x0), INST(invalid), INST(invalid),
		INST_F(mfhi, R, 0b100101), INST_F(mthi, R, 0b110001), INST_F(mflo, R, 0b100101), INST_F(mtlo, R, 0b110001), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST_F(mult, R, 0b111001), INST_F(multu, R, 0b111001), INST_F(div, R, 0b111001), INST_F(divu, R, 0b111001), INST(invalid), INST(invalid), INST(invalid), INST(invalid),
		INST_F(add, R, 0b111101), INST_F(addu, R, 0b111101), INST_F(sub, R, 0b111101), INST_F(subu, R, 0b111101), INST_F(and, R, 0b111101), INST_F(or , R, 0b111101), INST_F(xor, R, 0b111101), INST_F(nor, R, 0b111101),
		INST(invalid), INST(invalid), INST_F(slt, R, 0b111101), INST_F(sltu, R, 0b111101), INST(invalid)
	};
#undef INST
}
#undef CR_GET
#undef CR_SET
//****** Instructions ******//