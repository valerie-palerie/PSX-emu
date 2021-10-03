#include "MemoryInterface.h"

#include <iostream>

#include "Playstation.h"

bool AddressRange::Contains(uint32 address) const
{
	return (address >= start && address < end);
}

bool AddressRange::MapAddress(uint32 address, uint32& out_offset) const
{
	if (!Contains(address))
		return false;

	out_offset = address - start;
	return true;
}

IMemory* MemoryInterface::MapAddress(uint32 address, MemoryAccessFlags accessFlags, uint32& out_offset)
{
	//The PS1 mirrors its addressable memory in 2 separate places
	//KSEG2 maps to some hardware registers, handle it later
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
	if (component != nullptr)
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

#if _DEBUG
	for (auto& cond : _debugConditions)
	{
		if (cond->EvaluateCondition(address, accessFlags))
		{
			__debugbreak();
			break;
		}
	}
#endif

	if (component == nullptr)// || component->component() == nullptr)
	{
		__debugbreak();
		return nullptr;
	}

	if (static_cast<uint8>(component->accessFlags) & static_cast<uint8>(accessFlags))
		return component->component();

	__debugbreak();
	return nullptr;
}

MemorySegment MemoryInterface::GetMemSegmentFromAddress(uint32 address)
{
	if (address >= MemoryMap::KSEG2_BASE)
		return MemorySegment::KSEG2;
	if (address >= MemoryMap::KSEG1_BASE)
		return MemorySegment::KSEG1;
	if (address >= MemoryMap::KSEG0_BASE)
		return MemorySegment::KSEG0;

	return MemorySegment::KUSEG;
}

void MemoryInterface::AddComponent(MemoryMappedComponent component)
{
	_components.emplace_back(std::move(component));
}

void MemoryInterface::NotifyException(ExceptionType type, uint32 address)
{
	_playstation->cpu()->RaiseException(type, address);
}

void MemoryInterface::Init()
{
#if _DEBUG
	//_debugConditions.push_back(std::make_unique<Debug::MemoryDebugCondition_MemoryAccess>(AddressRange(0x0, 0x1000), MemoryAccessFlags::Write, -1));
#endif


	/*	TODO: Handle memory mirrors (https://psx-spx.consoledev.net/memorymap/)
	 *	2MB RAM can be mirrored to the first 8MB (strangely, enabled by default)		DONE
	 *	512K BIOS ROM can be mirrored to the last 4MB(disabled by default)				TODO
	 *	Expansion hardware(if any) may be mirrored within expansion region				TODO
	 *	The seven DMA Control Registers at 1F8010x8h are mirrored to 1F8010xCh			TODO
	 */

	 //Map all components to physical addresses.
#define COMP(name, comp, ...) AddComponent(MemoryMappedComponent(#name, comp, AddressRange(MemoryMap::##name##_BASE, MemoryMap::##name##_BASE + MemoryMap::##name##_SIZE), __VA_ARGS__))
#define COMP_MIRROR(name, comp, offset, ...) AddComponent(MemoryMappedComponent(#name, comp, AddressRange(MemoryMap::##name##_BASE + offset, MemoryMap::##name##_BASE + MemoryMap::##name##_SIZE + offset), __VA_ARGS__))

	COMP(RAM, _playstation->dram(), MemoryAccessFlags::ReadWrite);
	COMP_MIRROR(RAM, _playstation->dram(), MemoryMap::RAM_SIZE, MemoryAccessFlags::ReadWrite);
	COMP_MIRROR(RAM, _playstation->dram(), MemoryMap::RAM_SIZE * 2, MemoryAccessFlags::ReadWrite);
	COMP_MIRROR(RAM, _playstation->dram(), MemoryMap::RAM_SIZE * 3, MemoryAccessFlags::ReadWrite);
	COMP(EXP1, _playstation->exp1(), MemoryAccessFlags::ReadWrite);
	COMP(SCRATCHPAD, _playstation->scratchPad(), MemoryAccessFlags::ReadWrite);
	COMP(MEMCTRL, _playstation->memCtrl(), MemoryAccessFlags::ReadWrite);
	COMP(SIO, _playstation->sio(), MemoryAccessFlags::ReadWrite);
	COMP(MEMCTRL2, _playstation->memCtrl2(), MemoryAccessFlags::ReadWrite);
	COMP(INTERRUPT_CONTROLLER, _playstation->interruptCtrl(), MemoryAccessFlags::ReadWrite);
	COMP(DMA, _playstation->dma(), MemoryAccessFlags::ReadWrite);
	COMP(TIMERS, _playstation->timers(), MemoryAccessFlags::ReadWrite);
	COMP(CDROM, _playstation->cdrom(), MemoryAccessFlags::ReadWrite);
	COMP(GPU, _playstation->gpu(), MemoryAccessFlags::ReadWrite);
	COMP(MDEC, _playstation->mdec(), MemoryAccessFlags::ReadWrite);
	COMP(SPU, _playstation->spu(), MemoryAccessFlags::ReadWrite);
	COMP(EXP2, _playstation->exp2(), MemoryAccessFlags::ReadWrite);
	COMP(EXP3, _playstation->exp3(), MemoryAccessFlags::ReadWrite);
	COMP(BIOS, _playstation->bios(), MemoryAccessFlags::Read);
	COMP(CACHE_CONTROL, _playstation->cacheCtrl(), MemoryAccessFlags::ReadWrite);
}
