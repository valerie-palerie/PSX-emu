#pragma once
#include "CW33300.h"
#include "CP0.h"
#include "GTE.h"

class Playstation;

class CXD8530BQ
{
private:
	CW33300 cw3300;
	CP0 cp0;
	GTE gte;

	Playstation* _playstation;
public:

	Playstation* playstation() { return _playstation; }

	CXD8530BQ(Playstation* playstation);
};

