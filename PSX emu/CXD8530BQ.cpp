#include "CXD8530BQ.h"

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: _cw3300(this)
	, _cp0(this)
	, _gte(this)
	, _playstation(playstation)
{
}
