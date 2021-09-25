#include "ExpansionBoard.h"

bool ExpansionBoard::Read(std::uint32_t address, void* out_data, size_t size) const
{
	std::uint32_t val = 0xffffffff;
	std::memcpy(out_data, &val, size);
	return true;
}

bool ExpansionBoard::Write(std::uint32_t address, const void* data, size_t size)
{
	return true;
}

bool ExpansionBoard::Read(std::uint32_t address, std::vector<std::uint8_t>& out_data) const
{
	for (std::uint8_t& val : out_data)
		val = 0xff;
	return true;
}

bool ExpansionBoard::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
	return true;
}
