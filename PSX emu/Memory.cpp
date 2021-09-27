#include "Memory.h"
#include <fstream>

bool MemoryChip::Read(std::uint32_t address, void* out_data, size_t size) const
{
	if (address + size > _mem.size())
		return false;

	std::memcpy(out_data, &_mem[address], size);
	return true;
}

bool MemoryChip::Write(std::uint32_t address, const void* data, size_t size)
{
	if (address + size > _mem.size())
		return false;

	std::memcpy(&_mem[address], data, size);
	return true;
}

bool MemoryChip::Read(std::uint32_t address, std::vector<std::uint8_t>& out_data) const
{
	if (size_t(address + out_data.size()) > _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return false;
	}

	std::memcpy(&out_data[0], &_mem[address], sizeof(std::uint8_t) * out_data.size());
	return true;
}

bool MemoryChip::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	if (size_t(address + data.size()) > _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return false;
	}

	std::memcpy(&_mem[address], &data[0], sizeof(std::uint8_t) * data.size());
	return true;
}

void MemoryChip::DumpToFile(const std::string& filename)
{
	std::ofstream file(filename);
	for (size_t i = 0; i < _mem.size(); ++i)
	{
		file << std::hex << "0x" << i << ": 0x" << std::uint32_t(_mem[i]) << "\n";
	}

	file.close();
}

MemoryChip::MemoryChip(const std::uint32_t size, std::uint8_t initValue)
{
	_mem.resize(size, initValue);
}
