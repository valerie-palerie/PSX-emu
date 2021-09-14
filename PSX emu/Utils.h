#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

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
	std::uint32_t cop : 26; //bits 25 - 0


	explicit Opcode(std::uint32_t binary)
	{
		op = (binary >> 26);
		rs = ((binary >> 21) & 0b11111);
		rt = ((binary >> 16) & 0b11111);
		rd = ((binary >> 11) & 0b11111);
		shift = ((binary >> 6) & 0b11111);
		func = ((binary) & 0b11111);

		imm = std::uint16_t(binary);
		cop = binary & 0x3FFFFFF;
	}

	Opcode(const Opcode& opcode) = default;
	Opcode() = default;
};

struct ProcessorInstruction
{
public:
	std::string name;
	std::function<std::int8_t(const Opcode&)> instruction;

	ProcessorInstruction(std::string name, std::function<std::int8_t(const Opcode&)> instruction)
		: name(name)
		, instruction(instruction)
	{
	}
};