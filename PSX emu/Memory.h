#pragma once
#include <vector>
#include <cstdint>

enum class MemoryEndianness
{
	LittleEndian,
	BigEndian
};

class IMemory
{
public:
	virtual std::uint8_t Read8(std::uint32_t address) = 0;
	virtual std::uint16_t Read16(std::uint32_t address) = 0;
	virtual std::uint32_t Read32(std::uint32_t address) = 0;

	virtual void Write(std::uint32_t address, std::vector<std::uint8_t> data) = 0;

	virtual ~IMemory() = default;
};

class MemoryChip : public IMemory
{
private:
	const std::uint64_t _size;
	const MemoryEndianness _endianness;
	std::vector<std::uint8_t> _mem;

public:
	virtual std::uint8_t Read8(std::uint32_t address) override;
	virtual std::uint16_t Read16(std::uint32_t address) override;
	virtual std::uint32_t Read32(std::uint32_t address) override;

	virtual void Write(std::uint32_t address, std::vector<std::uint8_t> data) override;

	std::uint32_t size() const { return _size; }
	MemoryEndianness endianness() const { return _endianness; }

	MemoryChip(const std::uint32_t size, const MemoryEndianness endianness = MemoryEndianness::LittleEndian);
};
