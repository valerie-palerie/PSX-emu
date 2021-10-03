#pragma once
#include <cstdint>
#include <functional>
#include "MathUtils.h"

struct Opcode
{
public:
	uint8 op; //bits 31-26
	uint8 rs; //bits 25-21
	uint8 rt; //bits 20-16
	uint8 rd; //bits 15-11
	uint8 shift; //bits 10-6
	uint8 func; //bits 5-0

	uint16 imm; //bits 15-0
	uint32 imm_ze; //bits 15-0 zero extended
	uint32 imm_se; //bits 15-0 sign-extended
	uint32 cop; //bits 25 - 0
	uint32 bits; //bits 31 - 0

	//Helper function for debugging
	uint8 GetSegment(uint8 index) const
	{
		switch (index)
		{
		case 0:
			return func;
		case 1:
			return shift;
		case 2:
			return rd;
		case 3:
			return rt;
		case 4:
			return rs;
		default:
		case 5:
			return op;
		}
	}

	explicit Opcode(uint32 binary)
	{
		op = (binary >> 26) & 0b111111;
		rs = ((binary >> 21) & 0b11111);
		rt = ((binary >> 16) & 0b11111);
		rd = ((binary >> 11) & 0b11111);
		shift = ((binary >> 6) & 0b11111);
		func = ((binary) & 0b111111);

		imm = binary & 0xffff;
		imm_se = uint32(int16(imm));
		imm_ze = uint32(imm);
		cop = binary & 0x3ffffff;
		bits = binary;
	}

	Opcode(const Opcode& opcode) = default;
	Opcode() = default;
};


enum class InstructionFormat
{
	R,
	I,
	J
};

struct InstructionStructure
{
public:
	InstructionFormat format;
	uint8 segmentMask;

	InstructionStructure(InstructionFormat format = InstructionFormat::R, uint8 segmentMask = 0x00)
		: format(format)
		, segmentMask(segmentMask)
	{
	}
};

struct ProcessorInstruction
{
public:
	std::string name;
	std::function<int8(const Opcode&)> instruction;
	InstructionStructure structure;

	ProcessorInstruction(std::string name, std::function<int8(const Opcode&)> instruction, InstructionStructure structure = InstructionStructure())
		: name(std::move(name))
		, instruction(std::move(instruction))
		, structure(structure)
	{
	}
};

enum class ExceptionType
{
	INT = 0x00,			//Interrupt
	TLB_Mod = 0x01,		//Unused
	TLB_Load = 0x02,	//Unused
	TLB_Store = 0x03,	//Unused
	AdEL = 0x04,		//Address error, Data load or Instruction fetch (Read outside of KUSEG in user mode or misaligned address)
	AdES = 0x05,		//Address error, Data store (Store outside of KUSEG in user mode or misaligned address)
	IBE = 0x06,			//Bus error on Instruction fetch
	DBE = 0x07,			//Bus error on Data load / store
	Syscall = 0x08,		//Syscall instruction
	BP = 0x09,			//Breakpoint (break instruction)
	RI = 0x0A,			//Reserved instruction
	CopU = 0x0B,		//Coprocessor unusable
	Overflow = 0x0C,	//Arithmetic overflow
};
