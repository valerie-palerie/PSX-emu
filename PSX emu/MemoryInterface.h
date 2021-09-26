#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <iostream>
#include "DebugUtils.h"
#include "Memory.h"
#include "ProcessorTypes.h"

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
class MemoryInterface
{
private:
	Playstation* _playstation;
	std::vector<MemoryMappedComponent> _components;

#if _DEBUG
	std::vector<std::unique_ptr<Debug::MemoryDebugCondition_MemoryAccess>> _debugConditions;
#endif

	IMemory* MapAddress(std::uint32_t address, MemoryAccessFlags accessFlags, std::uint32_t& out_offset);

public:
	MemorySegment GetMemSegmentFromAddress(std::uint32_t address);

	template<typename T>
	bool Read(std::uint32_t address, T& out_data);
	template<typename T>
	bool Write(std::uint32_t address, T data);

	void AddComponent(MemoryMappedComponent component);

	void NotifyException(ExceptionType type, std::uint32_t address);

	void MapAddresses(Playstation* playstation);
};

template<typename T>
inline bool MemoryInterface::Read(std::uint32_t address, T& outData)
{
	if (address % sizeof(T) != 0)
	{
		NotifyException(ExceptionType::AdEL, address);
		return false;
	}

	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Read, offset);

	if (target == nullptr)
	{
		NotifyException(ExceptionType::AdEL, address);
		return false;
	}

	target->Read(offset, &outData, sizeof(T));
	return true;
}

template<typename T>
inline bool MemoryInterface::Write(std::uint32_t address, T data)
{
	if (address % sizeof(T) != 0)
	{
		NotifyException(ExceptionType::AdES, address);
		return false;
	}

	std::uint32_t offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Write, offset);

	if (target == nullptr)
	{
		NotifyException(ExceptionType::AdES, address);
		return false;
	}

	target->Write(offset, &data, sizeof(T));
	return true;
}
