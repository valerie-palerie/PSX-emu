#pragma once
#include <functional>
#include <string>
#include <vector>

class CXD8530BQ;

struct Opcode
{
public:
	std::uint8_t op; //bits 31-26
	std::uint8_t rs; //bits 25-21
	std::uint8_t rt; //bits 20-16
	std::uint8_t rd; //bits 15-11
	std::uint8_t shift; //bits 10-6
	std::uint8_t func; //bits 5-0

	std::uint16_t imm; //bits 15-0
	std::uint16_t imm_ze; //bits 15-0 zero extended
	std::uint32_t imm_se; //bits 15-0 sign-extended
	std::uint32_t cop; //bits 25 - 0
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
		op = (binary >> 26) & 0b111111;
		rs = ((binary >> 21) & 0b11111);
		rt = ((binary >> 16) & 0b11111);
		rd = ((binary >> 11) & 0b11111);
		shift = ((binary >> 6) & 0b11111);
		func = ((binary) & 0b111111);

		imm = binary & 0xffff;
		imm_se = std::uint32_t(std::int16_t(binary & 0xffff));
		imm_ze = std::uint16_t(binary & 0xffff);
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

class Processor
{
private:
	CXD8530BQ* _cpu;

protected:
	//Two sets of registers, loading a value from memory has a delay and we emulate it by reading and writing on different sets of registers, then syncing them after the instruction has finished.
	std::vector<std::uint32_t> _registers_read;
	std::vector<std::uint32_t> _registers_write;

	std::vector<ProcessorInstruction> _instructionMap;

public:
	CXD8530BQ* cpu() const { return _cpu; }

	virtual void Init() {}

	virtual std::uint32_t GetRegister(std::uint8_t index) const;
	virtual void SetRegister(std::uint8_t index, std::uint32_t value);

	virtual void ExecuteInstruction(Opcode opcode);

	explicit Processor(CXD8530BQ* cpu);
	virtual ~Processor() = default;
};