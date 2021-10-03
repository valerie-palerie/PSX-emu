#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "MathUtils.h"

class IMemory
{
protected:
	friend class MemoryInterface;
	virtual bool Read(uint32 address, void* out_data, size_t size) const = 0;
	virtual bool Write(uint32 address, const void* data, size_t size) = 0;

public:
	virtual bool Read(uint32 address, std::vector<uint8>& out_data) const = 0;
	virtual bool Write(uint32 address, std::vector<uint8> data) = 0;

	virtual ~IMemory() = default;
};

class MemoryChip : public IMemory
{
private:
	std::vector<uint8> _mem;

protected:
	virtual bool Read(uint32 address, void* out_data, size_t size) const;
	virtual bool Write(uint32 address, const void* data, size_t size);

public:
	virtual bool Read(uint32 address, std::vector<uint8>& out_data) const override;
	virtual bool Write(uint32 address, std::vector<uint8> data) override;

	uint32 size() const { return _mem.size(); }

	void DumpToFile(const std::string& filename);
	MemoryChip(const uint32 size, uint8 initValue = 0x0);
};

using HardwareRegs = MemoryChip;