#pragma once
#include "CW33300.h"
#include "COP0.h"
#include "GTE.h"
#include "MemoryInterface.h"

class Playstation;

class CXD8530BQ
{
private:
	CW33300 _cw33300;
	COP0 _cop0;
	GTE _gte;

	MemoryInterface _memInterface;

	Playstation* _playstation;
public:
	CW33300* cw33300() { return &_cw33300; }
	COP0* cop0() { return &_cop0; }
	GTE* gte() { return &_gte; }
	
	MemoryInterface* memInterface() { return &_memInterface; }
	Playstation* playstation() { return _playstation; }

	void Init();

	CXD8530BQ(Playstation* playstation);
};

