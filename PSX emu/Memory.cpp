#include "Memory.h"

std::uint8_t Memory::Read8(std::uint64_t offset)
{
	if (offset >= _mem.size())
	{
		//throw some kinda exception
		return 0;
	}
	
	return _mem[offset];
}

std::uint16_t Memory::Read16(std::uint64_t offset)
{
	if (offset + 1 >= _mem.size())
	{
		//throw some kinda exception
		return 0;
	}

	std::vector<std::uint32_t> bytes;
	bytes.push_back(std::uint32_t(_mem[offset]));
	bytes.push_back(std::uint32_t(_mem[offset + 1]));

	if (_endianness == MemoryEndianness::LittleEndian)
	{
		return bytes[0] + (bytes[1] << 8);
	}
	else
	{
		return bytes[1] + (bytes[0] << 8);
	}
}

std::uint32_t Memory::Read32(std::uint64_t offset)
{
	if (offset + 3 >= _mem.size())
	{
		//throw some kinda exception
		return 0;
	}

	std::vector<std::uint32_t> bytes;
	bytes.push_back(std::uint32_t(_mem[offset]));
	bytes.push_back(std::uint32_t(_mem[offset + 1]));
	bytes.push_back(std::uint32_t(_mem[offset + 2]));
	bytes.push_back(std::uint32_t(_mem[offset + 3]));

	if (_endianness == MemoryEndianness::LittleEndian)
	{
		return bytes[0] + (bytes[1] << 8) + (bytes[2] << 16) + (bytes[3] << 24);
	}
	else
	{
		return bytes[3] + (bytes[2] << 8) + (bytes[1] << 16) + (bytes[0] << 24);
	}
}

void Memory::Write(std::uint64_t offset, std::vector<std::uint8_t> data)
{
	for (int i = 0; i < data.size(); ++i)
	{
		if (offset + i >= _mem.size())
		{
			//throw some kinda exception?
			break;
		}
		_mem[offset + i] = data[i];
	}
}

Memory::Memory(const std::uint64_t size, const MemoryEndianness endianness)
	: _size(size)
	, _endianness(endianness)
{
	_mem.resize(size, 0);
}
