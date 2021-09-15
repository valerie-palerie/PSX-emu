#include "MemoryInterface.h"

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

IMemory* MemoryInterface::MapAddress(std::uint32_t address, std::uint32_t& out_offset)
{
	MemorySegment seg = GetMemSegmentFromAddress(address);
	switch (seg)
	{
	case MemorySegment::KSEG0:
	case MemorySegment::KSEG1:
		address &= 0x1FFFFFFF; //set 3 msb to 0
		break;
	default:
		break;
	}

	const MemoryMappedComponent* component = nullptr;
	for (const MemoryMappedComponent& comp : _components)
	{
		if (comp.range().MapAddress(address, out_offset))
			component = &comp;
	}

	if(component == nullptr || component->component() == nullptr)
		__debugbreak();
	return component->component();
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
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		return target->Read8(offset);

	return 0;
}

std::uint16_t MemoryInterface::Read16(std::uint32_t address)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		return target->Read16(offset);

	return 0;
}

std::uint32_t MemoryInterface::Read32(std::uint32_t address)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		return target->Read32(offset);

	return 0;
}

void MemoryInterface::Write8(std::uint32_t address, std::uint8_t data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		target->Write8(offset, data);
}

void MemoryInterface::Write16(std::uint32_t address, std::uint16_t data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		target->Write16(offset, data);
}

void MemoryInterface::Write32(std::uint32_t address, std::uint32_t data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		target->Write32(offset, data);
}

void MemoryInterface::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, offset);
	if (target != nullptr)
		target->Write(offset, data);
}

void MemoryInterface::AddComponent(MemoryMappedComponent component)
{
	_components.push_back(component);
}
