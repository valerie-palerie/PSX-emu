#pragma once
namespace MemoryMap
{
	enum MemoryMap : std::uint32_t
	{
		RAM_BASE = 0x00000000,
		RAM_SIZE = 0x200000,
		EXP1_BASE = 0x1f000000,
		EXP1_SIZE = 0x800000,
		SCRATCHPAD_BASE = 0x1f800000,
		SCRATCHPAD_SIZE = 0x400,
		MEMCTRL_BASE = 0x1F801000,
		MEMCTRL_SIZE = 0x40,
		SIO_BASE = 0x1F801050,
		SIO_SIZE = 0x10,
		MEMCTRL2_BASE = 0x1F801060,
		MEMCTRL2_SIZE = 0x10,
		INTERRUPT_CONTROLLER_BASE = 0x1F801070,
		INTERRUPT_CONTROLLER_SIZE = 0x10,
		DMA_BASE = 0x1F801080,
		DMA_SIZE = 0x80,
		TIMERS_BASE = 0x1F801100,
		TIMERS_SIZE = 0x40,
		CDROM_BASE = 0x1F801800,
		CDROM_SIZE = 0x10,
		GPU_BASE = 0x1F801810,
		GPU_SIZE = 0x10,
		MDEC_BASE = 0x1F801820,
		MDEC_SIZE = 0x10,
		SPU_BASE = 0x1F801C00,
		SPU_SIZE = 0x400,
		EXP2_BASE = 0x1f802000,
		EXP2_SIZE = 0x80,
		EXP3_BASE = 0x1fa00000,
		EXP3_SIZE = 0x200000,
		BIOS_BASE = 0x1fc00000,
		BIOS_SIZE = 0x80000,
		CACHE_CONTROL_BASE = 0xfffe0000,
		CACHE_CONTROL_SIZE = 0x200,

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

struct AddressRange
{
public:
	std::uint32_t start;
	std::uint32_t end;

	bool Contains(std::uint32_t address) const;
	bool MapAddress(std::uint32_t address, std::uint32_t& out_offset) const;

	AddressRange(std::uint32_t start, std::uint32_t end)
		: start(start)
		, end(end)
	{
	}
};

enum class MemoryAccessFlags : std::uint8_t
{
	None = 0b00,
	Read = 0b01,
	Write = 0b10,
	ReadWrite = 0b11
};
