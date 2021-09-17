#include "CXD8530BQ.h"
#include "Playstation.h"

void CXD8530BQ::Init()
{
	_cw33300.Init();
	_cop0.Init();
	_gte.Init();
}

CXD8530BQ::CXD8530BQ(Playstation* playstation)
	: _cw33300(this)
	, _cop0(this)
	, _gte(this)
	, _playstation(playstation)
{

}
