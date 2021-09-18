#pragma once
#include <cstdint>
#include <utility>
#include <functional>
#include <string>
#include <vector>

class Processor;

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
		cop = binary & 0x3ffffff;
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
	struct ProcessorDebugCondition
	{
	public:
		virtual bool EvaluateCondition(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, std::uint32_t pc) = 0;
		virtual ~ProcessorDebugCondition() = default;
	};

	struct ProcessorDebugCondition_ReachFirstOfInstruction
	{
		
	};
	void LogInstruction(const Processor* processor, const Opcode& opcode, ProcessorInstruction* instruction, std::uint32_t fetchedInstruction, std::uint32_t pc);
	void LogRegisterWrites(const std::vector<std::uint32_t>& readRegs, const std::vector<std::uint32_t>& writeRegs);

}

//Get/Set functions for the register pointed to by the opcode part.
#define R_GET(name) auto (name) = GetRegister(op.name)
#define R_SET(name, val) SetRegister(op.name, val)