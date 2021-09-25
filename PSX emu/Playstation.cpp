#include "Playstation.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstdint>
#include "CXD8530BQ.h"
#include "CW33300.h"
#include "DebugUtils.h"

void Playstation::Init()
{
	_cpu.Init();
}

void Playstation::Tick(float deltaTime)
{
	_cpu.Clock();
}

Playstation::Playstation()
	: _dram(MemoryMap::RAM_SIZE)
	, _scratchPad(MemoryMap::SCRATCHPAD_SIZE)
	, _io(MemoryMap::IO_SIZE)
	, _controlRegs(MemoryMap::CONTROL_REGS_SIZE)
	, _bios(MemoryMap::BIOS_SIZE)
	, _cpu(this)
{
	std::basic_ifstream<std::uint8_t> input("SCPH1001.bin", std::ios::binary);
	std::vector<std::uint8_t> buffer(
		(std::istreambuf_iterator<std::uint8_t>(input)),
		(std::istreambuf_iterator<std::uint8_t>()));
	input.close();

	_bios.Write(0, std::move(buffer));

	_memInterface.MapAddresses(this);
}
