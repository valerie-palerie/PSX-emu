#pragma once
#include <cstdint>
#include "MemoryInterface.h"

class Processor;

struct BaseDelayedMemStore
{
protected:
	std::uint8_t _remainingDelay;

	virtual void ApplyData() = 0;

public:
	std::uint8_t remainingDelay() const { return _remainingDelay; }

	bool Tick();

	BaseDelayedMemStore(std::uint8_t delay)
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
	DelayedMemStore_Ptr(T* target, T data, std::uint8_t delay)
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
	std::uint32_t _address;
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
	DelayedMemStore_Addr(MemoryInterface* interface, std::uint32_t address, T data, std::uint8_t delay)
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
	std::uint8_t _register;
	std::uint32_t _data;

	virtual void ApplyData() override;

public:
	std::uint8_t reg() const { return _register; }

	DelayedMemStore_ProcessorReg(Processor* processor, std::uint8_t reg, std::uint32_t data, std::uint8_t delay)
		: BaseDelayedMemStore(delay)
		, _processor(processor)
		, _register(reg)
		, _data(data)
	{
	}
};