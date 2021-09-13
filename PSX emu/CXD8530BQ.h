#pragma once
#include "CW33300.h"
#include "CP0.h"
#include "GTE.h"
#include "MemoryInterface.h"

class Playstation;

class CXD8530BQ
{
private:
	CW33300 _cw3300;
	CP0 _cp0;
	GTE _gte;

	MemoryInterface _memInterface;

	Playstation* _playstation;
public:
	CW33300* cw3300() { return &_cw3300; }
	CP0* cp0() { return &_cp0; }
	GTE* gte() { return &_gte; }
	
	MemoryInterface* memInterface() { return &_memInterface; }

	Playstation* playstation() { return _playstation; }

	CXD8530BQ(Playstation* playstation);
};

