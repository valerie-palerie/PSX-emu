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
	, _interruptController(MemoryMap::INTERRUPT_CONTROLLER_SIZE)
	, _cpu(this)

{
	std::basic_ifstream<std::uint8_t> input("SCPH1001.bin", std::ios::binary);

	std::vector<std::uint8_t> buffer(
		(std::istreambuf_iterator<std::uint8_t>(input)),
		(std::istreambuf_iterator<std::uint8_t>()));

	input.close();

	_bios.Write(0, std::move(buffer));

	//Map all components to physical addresses. This functionality should probably be moved to a bus.
#define COMP(name, comp, ...) _memInterface.AddComponent(MemoryMappedComponent(#name, comp, AddressRange(MemoryMap::##name##_BASE, MemoryMap::##name##_BASE + MemoryMap::##name##_SIZE), __VA_ARGS__))
	COMP(BIOS, &_bios, MemoryAccessFlags::Read);
	COMP(RAM, &_dram);
	COMP(MEMCTRL, nullptr);
	COMP(MEMCTRL2, nullptr);
	COMP(EXP1, &_exp1);
	COMP(EXP2, &_exp2);
	COMP(EXP3, &_exp3);
	COMP(PAD, nullptr);
	COMP(SIO, nullptr);
	COMP(INTERRUPT_CONTROLLER, &_interruptController);
	COMP(DMA, nullptr);
	COMP(TIMERS, nullptr);
	COMP(CDROM, nullptr);
	COMP(GPU, nullptr);
	COMP(MDEC, nullptr);
	COMP(SPU, nullptr);
	COMP(CACHE_CONTROL, nullptr);
}
