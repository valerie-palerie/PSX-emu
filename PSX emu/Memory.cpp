#include "Memory.h"
#include <fstream>

bool MemoryChip::Read(uint32 address, void* out_data, size_t size) const
{
	if (address + size > _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return false;
	}

	std::memcpy(out_data, &_mem[address], size);
	return true;
}

bool MemoryChip::Write(uint32 address, const void* data, size_t size)
{
	if (address + size > _mem.size())
	{
		//throw some kinda exception
		__debugbreak();
		return false;
	}

	std::memcpy(&_mem[address], data, size);
	return true;
}

bool MemoryChip::Read(uint32 address, std::vector<uint8>& out_data) const
{
	return Read(address, &out_data[0], sizeof(uint8) * out_data.size());
}

bool MemoryChip::Write(uint32 address, std::vector<uint8> data)
{
	return Write(address, &data[0], sizeof(uint8) * data.size());
}

void MemoryChip::DumpToFile(const std::string& filename)
{
	std::ofstream file(filename);
	for (size_t i = 0; i < _mem.size(); ++i)
	{
		file << std::hex << "0x" << i << ": 0x" << uint32(_mem[i]) << "\n";
	}

	file.close();
}

MemoryChip::MemoryChip(const uint32 size, uint8 initValue)
{
	_mem.resize(size, initValue);
}
