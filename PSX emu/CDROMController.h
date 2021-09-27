#pragma once
#include "Memory.h"
#include "MemoryTypes.h"
#include "PlaystationComponent.h"

class Playstation;

class CDROMController : public PlaystationComponent, public MemoryChip
{
public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	CDROMController(Playstation* playstation)
		: PlaystationComponent(playstation)
		, MemoryChip(MemoryMap::CDROM_SIZE)
	{
	}
};