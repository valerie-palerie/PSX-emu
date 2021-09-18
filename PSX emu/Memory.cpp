#include "Memory.h"

std::uint8_t MemoryChip::Read8(std::uint32_t address)
{
	if (address >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return 0;
	}

	return _mem[address];
}

std::uint16_t MemoryChip::Read16(std::uint32_t address)
{
	if (size_t(address) + 1 >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return 0;
	}

	std::uint16_t val;
	std::memcpy(&val, &_mem[address], sizeof(std::uint16_t));
	return val;
}

std::uint32_t MemoryChip::Read32(std::uint32_t address)
{
	if (size_t(address) + 3 >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return 0;
	}

	std::uint32_t val;
	std::memcpy(&val, &_mem[address], sizeof(std::uint32_t));
	return val;
}

void MemoryChip::Write8(std::uint32_t address, std::uint8_t data)
{
	if (size_t(address) >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return;
	}

	_mem[size_t(address)] = data;
}

void MemoryChip::Write16(std::uint32_t address, std::uint16_t data)
{
	if (size_t(address + 1) >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return;
	}
	std::memcpy(&_mem[address], &data, sizeof(std::uint16_t));
}

void MemoryChip::Write32(std::uint32_t address, std::uint32_t data)
{
	if (size_t(address + 3) >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return;
	}
	std::memcpy(&_mem[address], &data, sizeof(std::uint32_t));
}

void MemoryChip::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	for (std::uint32_t i = 0; i < data.size(); ++i)
	{
		Write8(address + i, data[i]);
	}
}

MemoryChip::MemoryChip(const std::uint32_t size, std::uint8_t initValue)
	: _size(size)
{
	_mem.resize(size, initValue);
}
