#pragma once
#include <vector>
#include <cstdint>
#include <string>

enum class MemoryEndianness
{
	LittleEndian,
	BigEndian
};

class IMemory
{
protected:
	friend class MemoryInterface;
	virtual bool Read(std::uint32_t address, void* out_data, size_t size) const = 0;
	virtual bool Write(std::uint32_t address, const void* data, size_t size) = 0;

public:
	virtual bool Read(std::uint32_t address, std::vector<std::uint8_t>& out_data) const = 0;
	virtual bool Write(std::uint32_t address, std::vector<std::uint8_t> data) = 0;

	virtual ~IMemory() = default;
};

class MemoryChip : public IMemory
{
private:
	const std::uint32_t _size;
	std::vector<std::uint8_t> _mem;

protected:
	virtual bool Read(std::uint32_t address, void* out_data, size_t size) const;
	virtual bool Write(std::uint32_t address, const void* data, size_t size);

public:
	virtual bool Read(std::uint32_t address, std::vector<std::uint8_t>& out_data) const override;
	virtual bool Write(std::uint32_t address, std::vector<std::uint8_t> data) override;

	std::uint32_t size() const { return _size; }

	void DumpToFile(const std::string& filename);
	MemoryChip(const std::uint32_t size, std::uint8_t initValue = 0xba);
};
