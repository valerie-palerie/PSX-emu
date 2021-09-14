#pragma once
#include <cstdint>
#include "Memory.h"

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

struct MemoryMappedComponent
{
private:
	IMemory* _component;
	AddressRange _range;

public:
	IMemory* component() const { return _component; }
	const AddressRange& range() const { return _range; }

	MemoryMappedComponent(IMemory* component, AddressRange range)
		: _component(component)
		, _range(range)
	{
	}
};

class MemoryInterface : public IMemory
{
private:
	std::vector<MemoryMappedComponent> _components;

	IMemory* MapAddress(std::uint32_t address, std::uint32_t& out_offset);
public:

	// Inherited via IMemory
	virtual std::uint8_t Read8(std::uint32_t address) override;
	virtual std::uint16_t Read16(std::uint32_t address) override;
	virtual std::uint32_t Read32(std::uint32_t address) override;
	virtual void Write(std::uint32_t address, std::vector<std::uint8_t> data) override;

	void AddComponent(MemoryMappedComponent component);
};

