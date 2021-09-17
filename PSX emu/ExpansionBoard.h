#pragma once
#include "Memory.h"
class ExpansionBoard : public IMemory
{
public:
	std::uint8_t Read8(std::uint32_t address) override;
	std::uint16_t Read16(std::uint32_t address) override;
	std::uint32_t Read32(std::uint32_t address) override;
	void Write8(std::uint32_t address, std::uint8_t data) override;
	void Write16(std::uint32_t address, std::uint16_t data) override;
	void Write32(std::uint32_t address, std::uint32_t data) override;
	void Write(std::uint32_t address, std::vector<std::uint8_t> data) override;
};

