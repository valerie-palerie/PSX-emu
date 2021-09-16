#include "CXD8530BQ.h"
#include "Playstation.h"

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: _cw33300(this)
	, _cop0(this)
	, _gte(this)
	, _playstation(playstation)
{
	//Map all components to physical addresses. This functionality should probably be moved to a bus.
#define COMP(name, comp) _memInterface.AddComponent(MemoryMappedComponent(#name, comp, AddressRange(MemoryMap::##name##_BASE, MemoryMap::##name##_BASE + MemoryMap::##name##_SIZE)))
	COMP(BIOS, _playstation->bios());
	COMP(RAM, _playstation->dram());
	COMP(MEMCTRL, nullptr);
	COMP(MEMCTRL2, nullptr);
	COMP(EXP1, nullptr);
	COMP(EXP2, nullptr);
	COMP(EXP3, nullptr);
	COMP(PAD, nullptr);
	COMP(SIO, nullptr);
	COMP(INTERRUPT_CONTROLLER, nullptr);
	COMP(DMA, nullptr);
	COMP(TIMERS, nullptr);
	COMP(CDROM, nullptr);
	COMP(GPU, nullptr);
	COMP(MDEC, nullptr);
	COMP(SPU, nullptr);
	COMP(CACHE_CONTROL, nullptr);
}
