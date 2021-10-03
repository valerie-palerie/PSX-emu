#pragma once
#include "Memory.h"
#include "MathUtils.h"

class ExpansionBoard : public IMemory
{
protected:
	virtual bool Read(uint32 address, void* out_data, size_t size) const;
	virtual bool Write(uint32 address, const void* data, size_t size);

public:
	virtual bool Read(uint32 address, std::vector<uint8>& out_data) const override;
	virtual bool Write(uint32 address, std::vector<uint8> data) override;
};

