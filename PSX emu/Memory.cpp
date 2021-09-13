#include "Memory.h"

std::uint8_t MemoryChip::Read8(std::uint32_t address)
{
	if (address >= _mem.size())
	{
		//throw some kinda exception
		return 0;
	}
	
	return _mem[address];
}

std::uint16_t MemoryChip::Read16(std::uint32_t address)
{
	if (address + 1 >= _mem.size())
	{
		//throw some kinda exception
		return 0;
	}

	std::vector<std::uint32_t> bytes;
	bytes.push_back(std::uint32_t(_mem[address]));
	bytes.push_back(std::uint32_t(_mem[address + 1]));

	if (_endianness == MemoryEndianness::LittleEndian)
	{
		return bytes[0] + (bytes[1] << 8);
	}
	else
	{
		return bytes[1] + (bytes[0] << 8);
	}
}

std::uint32_t MemoryChip::Read32(std::uint32_t address)
{
	if (address + 3 >= _mem.size())
	{
		//throw some kinda exception
		return 0;
	}

	std::vector<std::uint32_t> bytes;
	bytes.push_back(std::uint32_t(_mem[address]));
	bytes.push_back(std::uint32_t(_mem[address + 1]));
	bytes.push_back(std::uint32_t(_mem[address + 2]));
	bytes.push_back(std::uint32_t(_mem[address + 3]));

	if (_endianness == MemoryEndianness::LittleEndian)
	{
		return bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
	}
	else
	{
		return bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24);
	}
}

void MemoryChip::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	for (std::uint32_t i = 0; i < data.size(); ++i)
	{
		if ((address + i) >= _mem.size())
		{
			//throw some kinda exception?
			break;
		}
		_mem[address + i] = data[i];
	}
}

MemoryChip::MemoryChip(const std::uint32_t size, const MemoryEndianness endianness)
	: _size(size)
	, _endianness(endianness)
{
	_mem.resize(size, 0);
}
