#include "CXD8530BQ.h"
#include "Playstation.h"

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: _cw3300(this)
	, _cp0(this)
	, _gte(this)
	, _playstation(playstation)
{
	//BIOS
	_memInterface.AddComponent(
		MemoryMappedComponent(_playstation->bios(), AddressRange(MEM_BIOS, MEM_BIOS + (512 * 1024)))
	);
	//RAM size register
	_memInterface.AddComponent(
		MemoryMappedComponent(nullptr, AddressRange(MEM_RAM_SIZE_REG, MEM_RAM_SIZE_REG + 4))
	);
	//Cache control register
	_memInterface.AddComponent(
		MemoryMappedComponent(nullptr, AddressRange(MEM_CACHE_CONTROL_REG, MEM_CACHE_CONTROL_REG + 4))
	);
	
}
