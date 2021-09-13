#pragma once
#include<vector>
#include<cstdint>

enum class MemoryEndianness
{
	LittleEndian,
	BigEndian
};

class Memory
{
private:
	const std::uint64_t _size;
	const MemoryEndianness _endianness;
	std::vector<std::uint8_t> _mem;

public:
	std::uint8_t Read8(std::uint64_t offset);
	std::uint16_t Read16(std::uint64_t offset);
	std::uint32_t Read32(std::uint64_t offset);

	void Write(std::uint64_t offset, std::vector<std::uint8_t> data);

	std::uint64_t size() const { return _size; }
	MemoryEndianness endianness() const { return _endianness; }

	Memory(const std::uint64_t size, const MemoryEndianness endianness = MemoryEndianness::LittleEndian);
};
