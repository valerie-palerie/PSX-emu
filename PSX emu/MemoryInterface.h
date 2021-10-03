#pragma once
#include "PlaystationComponent.h"
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
class MemoryInterface : public PlaystationComponent
{
private:
	std::vector<MemoryMappedComponent> _components;

#if _DEBUG
	std::vector<std::unique_ptr<Debug::MemoryDebugCondition_MemoryAccess>> _debugConditions;
#endif

	IMemory* MapAddress(uint32 address, MemoryAccessFlags accessFlags, uint32& out_offset);

public:
	virtual void Init();
	virtual void Tick(double deltaT) override {};

	MemorySegment GetMemSegmentFromAddress(uint32 address);

	template<typename T>
	bool Read(uint32 address, T& out_data);
	template<typename T>
	bool Write(uint32 address, T data);

	void AddComponent(MemoryMappedComponent component);

	void NotifyException(ExceptionType type, uint32 address);

	MemoryInterface(Playstation* playstation)
		:PlaystationComponent(playstation)
	{ }
};

template<typename T>
inline bool MemoryInterface::Read(uint32 address, T& outData)
{
	if (address % sizeof(T) != 0)
	{
		NotifyException(ExceptionType::AdEL, address);
		return false;
	}

	uint32 offset = 0;
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
inline bool MemoryInterface::Write(uint32 address, T data)
{
	if (address % sizeof(T) != 0)
	{
		NotifyException(ExceptionType::AdES, address);
		return false;
	}

	uint32 offset = 0;
	IMemory* target = MapAddress(address, MemoryAccessFlags::Write, offset);

	if (target == nullptr)
	{
		NotifyException(ExceptionType::AdES, address);
		return false;
	}

	target->Write(offset, &data, sizeof(T));
	return true;
}
