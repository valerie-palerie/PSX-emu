#include "Playstation.h"
#include <fstream>
#include <iterator>
#include <vector>
#include <cstdint>
#include "CXD8530BQ.h"
#include "CW33300.h"
#include "DebugUtils.h"
#include "PlaystationComponent.h"

void Playstation::Init()
{
	for(PlaystationComponent* comp : _components)
		comp->Init();
}

void Playstation::Tick(double deltaT)
{
	for (PlaystationComponent* comp : _components)
		comp->Tick(deltaT);
}

Playstation::Playstation()
	: _dram(MemoryMap::RAM_SIZE, 0xba)
	, _exp1()//(MemoryMap::EXP1_SIZE)
	, _scratchPad(MemoryMap::SCRATCHPAD_SIZE)
	, _memCtrl(MemoryMap::MEMCTRL_SIZE)
	, _memCtrl2(MemoryMap::MEMCTRL2_SIZE)
	, _timers(MemoryMap::TIMERS_SIZE)
	, _interruptCtrl(MemoryMap::INTERRUPT_CONTROLLER_SIZE)
	, _exp2()//(MemoryMap::EXP2_SIZE)
	, _exp3()//(MemoryMap::EXP3_SIZE)
	, _bios(MemoryMap::BIOS_SIZE)
	, _cacheCtrl(MemoryMap::CACHE_CONTROL_SIZE)
	, _gpu(this)
	, _spu(this)
	, _sio(this)
	, _mdec(this)
	, _cdrom(this)
	, _memInterface(this)
	, _dma(this)
	, _cpu(this)
{
	_components.push_back(&_gpu);
	_components.push_back(&_spu);
	_components.push_back(&_sio);
	_components.push_back(&_mdec);
	_components.push_back(&_cdrom);
	_components.push_back(&_memInterface);
	_components.push_back(&_dma);
	_components.push_back(&_cpu);

	std::basic_ifstream<uint8> input("SCPH1001.bin", std::ios::binary);
	std::vector<uint8> buffer(
		(std::istreambuf_iterator<uint8>(input)),
		(std::istreambuf_iterator<uint8>()));
	input.close();

	_bios.Write(0, std::move(buffer));
}
