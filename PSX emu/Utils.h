#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

/* https://github.com/stenzek/duckstation/blob/master/src/core/bus.h
  RAM_BASE = 0x00000000,
  RAM_2MB_SIZE = 0x200000,
  RAM_2MB_MASK = RAM_2MB_SIZE - 1,
  RAM_8MB_SIZE = 0x800000,
  RAM_8MB_MASK = RAM_8MB_SIZE - 1,
  RAM_MIRROR_END = 0x800000,
  EXP1_BASE = 0x1F000000,
  EXP1_SIZE = 0x800000,
  EXP1_MASK = EXP1_SIZE - 1,
  MEMCTRL_BASE = 0x1F801000,
  MEMCTRL_SIZE = 0x40,
  MEMCTRL_MASK = MEMCTRL_SIZE - 1,
  PAD_BASE = 0x1F801040,
  PAD_SIZE = 0x10,
  PAD_MASK = PAD_SIZE - 1,
  SIO_BASE = 0x1F801050,
  SIO_SIZE = 0x10,
  SIO_MASK = SIO_SIZE - 1,
  MEMCTRL2_BASE = 0x1F801060,
  MEMCTRL2_SIZE = 0x10,
  MEMCTRL2_MASK = MEMCTRL2_SIZE - 1,
  INTERRUPT_CONTROLLER_BASE = 0x1F801070,
  INTERRUPT_CONTROLLER_SIZE = 0x10,
  INTERRUPT_CONTROLLER_MASK = INTERRUPT_CONTROLLER_SIZE - 1,
  DMA_BASE = 0x1F801080,
  DMA_SIZE = 0x80,
  DMA_MASK = DMA_SIZE - 1,
  TIMERS_BASE = 0x1F801100,
  TIMERS_SIZE = 0x40,
  TIMERS_MASK = TIMERS_SIZE - 1,
  CDROM_BASE = 0x1F801800,
  CDROM_SIZE = 0x10,
  CDROM_MASK = CDROM_SIZE - 1,
  GPU_BASE = 0x1F801810,
  GPU_SIZE = 0x10,
  GPU_MASK = GPU_SIZE - 1,
  MDEC_BASE = 0x1F801820,
  MDEC_SIZE = 0x10,
  MDEC_MASK = MDEC_SIZE - 1,
  SPU_BASE = 0x1F801C00,
  SPU_SIZE = 0x400,
  SPU_MASK = 0x3FF,
  EXP2_BASE = 0x1F802000,
  EXP2_SIZE = 0x2000,
  EXP2_MASK = EXP2_SIZE - 1,
  EXP3_BASE = 0x1FA00000,
  EXP3_SIZE = 0x1,
  EXP3_MASK = EXP3_SIZE - 1,
  BIOS_BASE = 0x1FC00000,
  BIOS_SIZE = 0x80000,
  BIOS_MASK = 0x7FFFF,
 */

#define MEM_BIOS 0xbfc00000
#define MEM_BIOS_RANGE 0xbfc00000, 0xbfc00000 + (512 * 1024)
#define MEM_RAM 0xa0000000
#define MEM_RAM_RANGE 0xa0000000, 0xa0000000 + (2048 * 1024)
#define MEM_RAM_SIZE_REG 0x1f801060
#define MEM_RAM_SIZE_REG_RANGE 0x1f801060
#define MEM_CACHE_CONTROL_REG 0xfffe0130
#define MEM_MEM_CONTROL_REG 0x1f801000
#define MEM_MEM_CONTROL_REG_RANGE 0x1f801000, 0x1f801000 + 36

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

//Get/Set functions for the register pointed to by the opcode part.
#define R_GET(name) auto (name) = GetRegister(op.name)
#define R_SET(name, val) SetRegister(op.name, val)