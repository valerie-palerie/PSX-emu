#pragma once
#include "Memory.h"
#include "MemoryTypes.h"
#include "PlaystationComponent.h"

class SIO : public PlaystationComponent, public MemoryChip
{
public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	SIO(Playstation* playstation)
		: PlaystationComponent(playstation)
		, MemoryChip(MemoryMap::SIO_SIZE)
	{
	}
};
