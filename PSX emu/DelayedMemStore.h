#pragma once
#include <cstdint>
#include "MemoryInterface.h"
#include "MathUtils.h"

class Processor;

struct BaseDelayedMemStore
{
protected:
	uint8 _remainingDelay;

	virtual void ApplyData() = 0;

public:
	uint8 remainingDelay() const { return _remainingDelay; }

	bool Tick();

	BaseDelayedMemStore(uint8 delay)
		: _remainingDelay(delay)
	{
	}
};

template<typename T>
struct DelayedMemStore_Ptr : public BaseDelayedMemStore
{
protected:
	T _data;
	T* _target;

	virtual void ApplyData() override
	{
		if (_target != nullptr)
		{
			*_target = _data;
			_target = nullptr;
		}
	}

public:
	DelayedMemStore_Ptr(T* target, T data, uint8 delay)
		: BaseDelayedMemStore(delay)
		, _data(data)
		, _target(target)
	{
	}
};

template<typename T>
struct DelayedMemStore_Addr : public BaseDelayedMemStore
{
protected:
	MemoryInterface* _interface;
	uint32 _address;
	T _data;

	virtual void ApplyData() override
	{
		if (_interface != nullptr)
		{
			_interface->Write<T>(_data);
			_interface = nullptr;
		}
	}

public:
	DelayedMemStore_Addr(MemoryInterface* interface, uint32 address, T data, uint8 delay)
		: BaseDelayedMemStore(delay)
		, _interface(interface)
		, _address(address)
		, _data(data)
	{
	}
};

struct DelayedMemStore_ProcessorReg : public BaseDelayedMemStore
{
protected:
	Processor* _processor;
	uint8 _register;
	uint32 _data;

	virtual void ApplyData() override;

public:
	uint8 reg() const { return _register; }

	DelayedMemStore_ProcessorReg(Processor* processor, uint8 reg, uint32 data, uint8 delay)
		: BaseDelayedMemStore(delay)
		, _processor(processor)
		, _register(reg)
		, _data(data)
	{
	}
};