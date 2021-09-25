#pragma once
#include "Memory.h"
class ExpansionBoard : public IMemory
{
protected:
	virtual bool Read(std::uint32_t address, void* out_data, size_t size) const;
	virtual bool Write(std::uint32_t address, const void* data, size_t size);

public:
	virtual bool Read(std::uint32_t address, std::vector<std::uint8_t>& out_data) const override;
	virtual bool Write(std::uint32_t address, std::vector<std::uint8_t> data) override;
};

