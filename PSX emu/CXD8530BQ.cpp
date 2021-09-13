#include "CXD8530BQ.h"
#include "Playstation.h"

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: _cw3300(this)
	, _cp0(this)
	, _gte(this)
	, _playstation(playstation)
{
	_memInterface.AddComponent(
		MemoryMappedComponent(_playstation->bios(),
			AddressRange(0xbfc00000, 0xbfc00000 + (512 * 1024))
		)
	);

}
