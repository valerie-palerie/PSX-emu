#pragma once
#include "Memory.h"
#include "PlaystationComponent.h"

class Playstation;
class MemoryChip;
class MDEC;
class GPU;
class CDROMController;
class SPU;

class DMAController : public PlaystationComponent, public MemoryChip
{
protected:
	MemoryChip* _dram;
	MDEC* _mdec;
	GPU* _gpu;
	CDROMController* _cdrom;
	SPU* _spu;
	
public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	DMAController(Playstation* playstation);
};

