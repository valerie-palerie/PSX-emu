#pragma once
#include "CW33300.h"
#include "COP0.h"
#include "GTE.h"

class Playstation;

class CXD8530BQ
{
private:
	CW33300 _cw33300;
	COP0 _cop0;
	GTE _gte;

	Playstation* _playstation;
public:
	CW33300* cw33300() { return &_cw33300; }
	COP0* cop0() { return &_cop0; }
	GTE* gte() { return &_gte; }

	Playstation* playstation() { return _playstation; }

	void Init();

	void Clock();

	void ExecuteInstruction(Opcode opcode);
	void RaiseException(ExceptionType exceptionType, std::uint32_t data = 0x0);

	CXD8530BQ(Playstation* playstation);
};

