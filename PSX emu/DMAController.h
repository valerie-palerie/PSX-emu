#pragma once
#include "Memory.h"
#include "PlaystationComponent.h"

class Playstation;

class DMAController : public PlaystationComponent, public MemoryChip
{
protected:
	MemoryChip* _dram;

public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	DMAController(Playstation* playstation);
};

