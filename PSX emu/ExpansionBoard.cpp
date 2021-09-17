#include "ExpansionBoard.h"

std::uint8_t ExpansionBoard::Read8(std::uint32_t address)
{
	return 0xff;
}

std::uint16_t ExpansionBoard::Read16(std::uint32_t address)
{
	return 0xffff;
}

std::uint32_t ExpansionBoard::Read32(std::uint32_t address)
{
	return 0xffffffff;
}

void ExpansionBoard::Write8(std::uint32_t address, std::uint8_t data)
{
}

void ExpansionBoard::Write16(std::uint32_t address, std::uint16_t data)
{
}

void ExpansionBoard::Write32(std::uint32_t address, std::uint32_t data)
{
}

void ExpansionBoard::Write(std::uint32_t address, std::vector<std::uint8_t> data)
{
}
