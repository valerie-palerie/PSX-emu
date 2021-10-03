#include "ExpansionBoard.h"

bool ExpansionBoard::Read(uint32 address, void* out_data, size_t size) const
{
	uint32 val = 0xffffffff;
	std::memcpy(out_data, &val, size);
	return true;
}

bool ExpansionBoard::Write(uint32 address, const void* data, size_t size)
{
	return true;
}

bool ExpansionBoard::Read(uint32 address, std::vector<uint8>& out_data) const
{
	for (uint8& val : out_data)
		val = 0xff;
	return true;
}

bool ExpansionBoard::Write(uint32 address, std::vector<uint8> data)
{
	return true;
}
