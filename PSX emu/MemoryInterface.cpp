#include "MemoryInterface.h"

#include <iostream>

#include "Playstation.h"

bool AddressRange::Contains(std::uint32_t address) const
{
	return (address >= start && address < end);
}

bool AddressRange::MapAddress(std::uint32_t address, std::uint32_t& out_offset) const
{
	if (!Contains(address))
		return false;

	out_offset = address - start;
	return true;
}

IMemory* MemoryInterface::MapAddress(std::uint32_t address, MemoryAccessFlags accessFlags, std::uint32_t& out_offset)
{
	//The PS1 mirrors its addressable memory in 3 separate places
	//But KSEG2 maps to some hardware registers, handle it later
	MemorySegment seg = GetMemSegmentFromAddress(address);
	switch (seg)
	{
	case MemorySegment::KSEG0:
	case MemorySegment::KSEG1:
		address &= 0x1FFFFFFF; //set 3 msb to 0
		break;
	case MemorySegment::KUSEG:
	case MemorySegment::KSEG2:
		break;
	}

	const MemoryMappedComponent* component = nullptr;
	for (const MemoryMappedComponent& comp : _components)
	{
		if (comp.range().MapAddress(address, out_offset))
		{
			component = &comp;
			break;
		}
	}

	//I think accessing past KUSEG + 0x20000000 (512MB) is meant to throw an exception? (https://psx-spx.consoledev.net/memorymap/)
	//if(seg == MemorySegment::KUSEG && offset >= 0x20000000)
	//	__debugbreak();

#if DEBUG_LOG_ENABLED
	if (component == nullptr)
	{
		std::cout << std::hex << "		Mem " << (accessFlags == MemoryAccessFlags::Read ? "Read: 0x" : "Write: 0x") << address << std::dec;
		switch (seg)
		{
		case MemorySegment::KUSEG:
			std::cout << " (KUSEG)";
			break;
		case MemorySegment::KSEG0:
			std::cout << " (KSEG0)";
			break;
		case MemorySegment::KSEG1:
			std::cout << " (KSEG1)";
			break;
		case MemorySegment::KSEG2:
			std::cout << " (KSEG2)";
			break;
		}
		
		std::cout << " [" << (component == nullptr ? "invalid" : component->name()) << "]\n";
	}
#endif

	if (component == nullptr)// || component->component() == nullptr)
	{
		//__debugbreak();
		return nullptr;
	}

	if (static_cast<std::uint8_t>(component->accessFlags) & static_cast<std::uint8_t>(accessFlags))
		return component->component();

	__debugbreak();
	return nullptr;
}

MemorySegment MemoryInterface::GetMemSegmentFromAddress(std::uint32_t address)
{
	if (address >= MemoryMap::KSEG2_BASE)
		return MemorySegment::KSEG2;
	if (address >= MemoryMap::KSEG1_BASE)
		return MemorySegment::KSEG1;
	if (address >= MemoryMap::KSEG0_BASE)
		return MemorySegment::KSEG0;

	return MemorySegment::KUSEG;
}

std::uint8_t MemoryInterface::Read8(std::uint32_t address)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Read, offset);
	if (target != nullptr)
		return target->Read8(offset);

	return 0;
}

std::uint16_t MemoryInterface::Read16(std::uint32_t address)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Read, offset);
	if (target != nullptr)
		return target->Read16(offset);

	return 0;
}

std::uint32_t MemoryInterface::Read32(std::uint32_t address)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Read, offset);
	if (target != nullptr)
		return target->Read32(offset);

	return 0;
}

void MemoryInterface::Write8(std::uint32_t address, std::uint8_t data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Write, offset);
	if (target != nullptr)
		target->Write8(offset, data);
}

void MemoryInterface::Write16(std::uint32_t address, std::uint16_t data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Write, offset);
	if (target != nullptr)
		target->Write16(offset, data);
}

void MemoryInterface::Write32(std::uint32_t address, std::uint32_t data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Write, offset);
	if (target != nullptr)
		target->Write32(offset, data);
}

void MemoryInterface::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Write, offset);
	if (target != nullptr)
		target->Write(offset, data);
}

void MemoryInterface::AddComponent(MemoryMappedComponent component)
{
	_components.emplace_back(std::move(component));
}

void MemoryInterface::MapAddresses(Playstation* playstation)
{
	/*	TODO: Handle memory mirrors (https://psx-spx.consoledev.net/memorymap/)
	 *	2MB RAM can be mirrored to the first 8MB (strangely, enabled by default)
	 *	512K BIOS ROM can be mirrored to the last 4MB(disabled by default)
	 *	Expansion hardware(if any) may be mirrored within expansion region
	 *	The seven DMA Control Registers at 1F8010x8h are mirrored to 1F8010xCh
	 */

	 //Map all components to physical addresses.
#define COMP(name, comp, ...) AddComponent(MemoryMappedComponent(#name, comp, AddressRange(MemoryMap::##name##_BASE, MemoryMap::##name##_BASE + MemoryMap::##name##_SIZE), __VA_ARGS__))
#define COMP_MIRROR(name, comp, offset, ...) AddComponent(MemoryMappedComponent(#name, comp, AddressRange(MemoryMap::##name##_BASE + offset, MemoryMap::##name##_BASE + MemoryMap::##name##_SIZE + offset), __VA_ARGS__))

	COMP(RAM, playstation->dram(), MemoryAccessFlags::ReadWrite);
	COMP_MIRROR(RAM, playstation->dram(), MemoryMap::RAM_SIZE, MemoryAccessFlags::ReadWrite);
	COMP_MIRROR(RAM, playstation->dram(), MemoryMap::RAM_SIZE * 2, MemoryAccessFlags::ReadWrite);
	COMP_MIRROR(RAM, playstation->dram(), MemoryMap::RAM_SIZE * 3, MemoryAccessFlags::ReadWrite);
	COMP(EXP1, playstation->exp1(), MemoryAccessFlags::ReadWrite);
	COMP(SCRATCHPAD, playstation->scratchPad(), MemoryAccessFlags::ReadWrite);
	COMP(IO, playstation->io(), MemoryAccessFlags::ReadWrite);
	COMP(EXP2, playstation->exp2(), MemoryAccessFlags::ReadWrite);
	COMP(EXP3, playstation->exp3(), MemoryAccessFlags::ReadWrite);
	COMP(BIOS, playstation->bios(), MemoryAccessFlags::Read);
	COMP(CONTROL_REGS, playstation->controlRegs(), MemoryAccessFlags::ReadWrite);
}
