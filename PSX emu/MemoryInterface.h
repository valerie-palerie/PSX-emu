#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include "Utils.h"
#include "Memory.h"

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
	None		= 0b00,
	Read		= 0b01,
	Write		= 0b10,
	ReadWrite	= 0b11
};

struct MemoryMappedComponent
{
private:
	std::string _name;
	IMemory* _component;
	AddressRange _range;

public:
	MemoryAccessFlags accessFlags;

	const std::string& name() const { return _name; }
	IMemory* component() const { return _component; }
	const AddressRange& range() const { return _range; }

	MemoryMappedComponent(std::string name, IMemory* component, AddressRange range, MemoryAccessFlags accessFlags = MemoryAccessFlags::ReadWrite)
		: _name(std::move(name))
		, _component(component)
		, _range(range)
		, accessFlags(accessFlags)
	{
	}
};

//The memory interface is here to map the addressable memory space into the devices that occupy it.
class MemoryInterface : public IMemory
{
private:
	std::vector<MemoryMappedComponent> _components;

	IMemory* MapAddress(std::uint32_t address, MemoryAccessFlags accessFlags, std::uint32_t& out_offset);

public:
	MemorySegment GetMemSegmentFromAddress(std::uint32_t address);

	// Inherited via IMemory
	virtual std::uint8_t Read8(std::uint32_t address) override;
	virtual std::uint16_t Read16(std::uint32_t address) override;
	virtual std::uint32_t Read32(std::uint32_t address) override;

	virtual void Write8(std::uint32_t address, std::uint8_t data) override;
	virtual void Write16(std::uint32_t address, std::uint16_t data) override;
	virtual void Write32(std::uint32_t address, std::uint32_t data) override;
	virtual void Write(std::uint32_t address, std::vector<std::uint8_t> data) override;

	void AddComponent(MemoryMappedComponent component);
};

