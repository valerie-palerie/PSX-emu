#pragma once
#include <cstdint>
#include <utility>
#include <functional>
#include <string>

class Processor;

namespace MemoryMap
{
	//Addresses of stuff. Taken from duckstation's source code for convenience.
	//https://github.com/stenzek/duckstation/blob/master/src/core/bus.h
	enum MemoryMap : std::uint32_t
	{
		RAM_BASE = 0x00000000,
		RAM_SIZE = 0x200000,
		RAM_2MB_SIZE = 0x200000,
		RAM_8MB_SIZE = 0x800000,
		RAM_MIRROR_END = 0x800000,
		EXP1_BASE = 0x1f000000,
		EXP1_SIZE = 0x800000,
		MEMCTRL_BASE = 0x1f801000,
		MEMCTRL_SIZE = 0x40,
		PAD_BASE = 0x1f801040,
		PAD_SIZE = 0x10,
		SIO_BASE = 0x1f801050,
		SIO_SIZE = 0x10,
		MEMCTRL2_BASE = 0x1f801060,
		MEMCTRL2_SIZE = 0x10,
		INTERRUPT_CONTROLLER_BASE = 0x1f801070,
		INTERRUPT_CONTROLLER_SIZE = 0x10,
		DMA_BASE = 0x1f801080,
		DMA_SIZE = 0x80,
		TIMERS_BASE = 0x1f801100,
		TIMERS_SIZE = 0x40,
		CDROM_BASE = 0x1f801800,
		CDROM_SIZE = 0x10,
		GPU_BASE = 0x1f801810,
		GPU_SIZE = 0x10,
		MDEC_BASE = 0x1f801820,
		MDEC_SIZE = 0x10,
		SPU_BASE = 0x1f801C00,
		SPU_SIZE = 0x400,
		EXP2_BASE = 0x1f802000,
		EXP2_SIZE = 0x2000,
		EXP3_BASE = 0x1fa00000,
		EXP3_SIZE = 0x1,
		BIOS_BASE = 0x1fc00000,
		BIOS_SIZE = 0x80000,
		CACHE_CONTROL_BASE = 0xfffe0130,
		CACHE_CONTROL_SIZE = 4,

		KUSEG_BASE = 0x00000000,
		KSEG0_BASE = 0x80000000,
		KSEG1_BASE = 0xa0000000,
		KSEG2_BASE = 0xc0000000,
	};
}

enum class MemorySegment
{
	KUSEG,
	KSEG0,
	KSEG1,
	KSEG2,
};

struct Opcode
{
public:
	std::uint8_t op : 6; //bits 31-26
	std::uint8_t rs : 5; //bits 25-21
	std::uint8_t rt : 5; //bits 20-16
	std::uint8_t rd : 5; //bits 15-11
	std::uint8_t shift : 5; //bits 10-6
	std::uint8_t func : 6; //bits 5-0

	std::uint16_t imm; //bits 15-0
	std::uint32_t imm_se; //bits 15-0 sign-extended
	std::uint32_t cop : 26; //bits 25 - 0
	std::uint32_t opcode; //bits 31 - 0

	//Helper function for debugging
	std::uint8_t GetSegment(std::uint8_t index) const
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

	explicit Opcode(std::uint32_t binary)
	{
		op = (binary >> 26);
		rs = ((binary >> 21) & 0b11111);
		rt = ((binary >> 16) & 0b11111);
		rd = ((binary >> 11) & 0b11111);
		shift = ((binary >> 6) & 0b11111);
		func = ((binary) & 0b111111);

		imm = std::uint16_t(binary & 0xffff);
		imm_se = std::uint32_t(std::int16_t(binary & 0xffff));
		cop = binary & 0x3FFFFFF;
		opcode = binary;
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
	std::uint8_t segmentMask;

	InstructionStructure(InstructionFormat format = InstructionFormat::R, std::uint8_t segmentMask = 0x00)
		: format(format)
		, segmentMask(segmentMask)
	{
	}
};

struct ProcessorInstruction
{
public:
	std::string name;
	std::function<std::int8_t(const Opcode&)> instruction;
	InstructionStructure structure;

	ProcessorInstruction(std::string name, std::function<std::int8_t(const Opcode&)> instruction, InstructionStructure structure = InstructionStructure())
		: name(std::move(name))
		, instruction(std::move(instruction))
		, structure(structure)
	{
	}
};

namespace Debug
{
	void LogInstruction(const Processor* processor, const Opcode& opcode, ProcessorInstruction* instruction, std::uint32_t pc, bool isDelaySlot);
	void LogRegisterWrites(const std::vector<std::uint32_t>& readRegs, const std::vector<std::uint32_t>& writeRegs);

}

//Get/Set functions for the register pointed to by the opcode part.
#define R_GET(name) auto (name) = GetRegister(op.name)
#define R_SET(name, val) SetRegister(op.name, val)