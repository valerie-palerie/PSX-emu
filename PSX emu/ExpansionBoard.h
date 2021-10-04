#pragma once
#include "Memory.h"
#include "MathUtils.h"

class ExpansionBoard : public IMemory
{
protected:
	virtual bool Read(uint32 address, void* out_data, size_t size) const;
	virtual bool Write(uint32 address, const void* data, size_t size);
};

