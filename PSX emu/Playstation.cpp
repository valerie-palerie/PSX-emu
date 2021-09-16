#include "Playstation.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstdint>
#include "CXD8530BQ.h"
#include "CW33300.h"
#include "Utils.h"

void Playstation::Init()
{
	_cpu.Init();
}

void Playstation::Tick(float deltaTime)
{
	_cpu.cw33300()->ProcessNextInstruction();
}

Playstation::Playstation()
	: _bios(MemoryMap::BIOS_SIZE)
	, _dram(MemoryMap::RAM_2MB_SIZE)
	, _cpu(this)

{
	std::basic_ifstream<std::uint8_t> input("SCPH1001.bin", std::ios::binary);

	std::vector<std::uint8_t> buffer(
		(std::istreambuf_iterator<std::uint8_t>(input)),
		(std::istreambuf_iterator<std::uint8_t>()));

	input.close();

	_bios.Write(0, std::move(buffer));
}
