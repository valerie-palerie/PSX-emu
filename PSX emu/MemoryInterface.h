#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include "DebugUtils.h"
#include "MemoryTypes.h"
#include "Memory.h"

class Playstation;

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

#if _DEBUG
	std::vector<std::unique_ptr<Debug::MemoryDebugCondition_MemoryAccess>> _debugConditions;
#endif

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

	void MapAddresses(Playstation* playstation);
};

