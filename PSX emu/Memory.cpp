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

	std::vector<std::uint8_t> bytes;
	bytes.push_back(std::uint8_t(_mem[size_t(address)]));
	bytes.push_back(std::uint8_t(_mem[size_t(address) + 1]));

	if (_endianness == MemoryEndianness::LittleEndian)
	{
		return bytes[0] + std::uint16_t(bytes[1] << 8);
	}
	else
	{
		return bytes[1] + std::uint16_t(bytes[0] << 8);
	}
}

std::uint32_t MemoryChip::Read32(std::uint32_t address)
{
	if (size_t(address) + 3 >= _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return 0;
	}

	std::vector<std::uint8_t> bytes;
	bytes.push_back(std::uint8_t(_mem[size_t(address)]));
	bytes.push_back(std::uint8_t(_mem[size_t(address) + 1]));
	bytes.push_back(std::uint8_t(_mem[size_t(address) + 2]));
	bytes.push_back(std::uint8_t(_mem[size_t(address) + 3]));

	if (_endianness == MemoryEndianness::LittleEndian)
	{
		return bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
	}
	else
	{
		return bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24);
	}
}

void MemoryChip::Write8(std::uint32_t address, std::uint8_t data)
{
	_mem[size_t(address)] = data;
}

void MemoryChip::Write16(std::uint32_t address, std::uint16_t data)
{
	_mem[size_t(address)] = std::uint8_t(data & 0xff);
	_mem[size_t(address) + 1] = std::uint8_t((data >> 8) & 0xff);
}

void MemoryChip::Write32(std::uint32_t address, std::uint32_t data)
{
	_mem[size_t(address)] = std::uint8_t(data & 0xff);
	_mem[size_t(address) + 1] = std::uint8_t((data >> 8) & 0xff);
	_mem[size_t(address) + 2] = std::uint8_t((data >> 16) & 0xff);
	_mem[size_t(address) + 3] = std::uint8_t((data >> 24) & 0xff);
}

void MemoryChip::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	for (std::uint32_t i = 0; i < data.size(); ++i)
	{
		if ((size_t(address) + i) >= _mem.size())
		{
			//throw some kinda exception?
			__debugbreak();
			break;
		}
		_mem[size_t(address) + i] = data[i];
	}
}

MemoryChip::MemoryChip(const std::uint32_t size, const MemoryEndianness endianness)
	: _size(size)
	, _endianness(endianness)
{
	_mem.resize(size, 0x3f);
}
