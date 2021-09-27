#pragma once
#include "Memory.h"
#include "MemoryTypes.h"
#include "PlaystationComponent.h"

class Playstation;

class SPU : public PlaystationComponent, public MemoryChip
{
public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	SPU(Playstation* playstation)
		: PlaystationComponent(playstation)
		, MemoryChip(MemoryMap::SPU_SIZE)
	{
	}
};