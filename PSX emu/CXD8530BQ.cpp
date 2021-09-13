#include "CXD8530BQ.h"

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: cw3300(this)
	, cp0(this)
	, gte(this)
	, _playstation(playstation)
{
}
