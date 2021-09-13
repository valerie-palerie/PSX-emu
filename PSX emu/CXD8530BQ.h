#pragma once
#include "CW33300.h"
#include "CP0.h"
#include "GTE.h"

class Playstation;

class CXD8530BQ
{
private:
	CW33300 _cw3300;
	CP0 _cp0;
	GTE _gte;

	Playstation* _playstation;
public:
	CW33300* cw3300() { return &_cw3300; }
	CP0* cp0() { return &_cp0; }
	GTE* gte() { return &_gte; }
	

	Playstation* playstation() { return _playstation; }

	CXD8530BQ(Playstation* playstation);
};

