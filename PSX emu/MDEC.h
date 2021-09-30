#pragma once
#include "Memory.h"
#include "MemoryTypes.h"
#include "PlaystationComponent.h"

class MDEC : public PlaystationComponent, public MemoryChip
{
public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	MDEC(Playstation* playstation)
		: PlaystationComponent(playstation)
		, MemoryChip(MemoryMap::SIO_SIZE)
	{
	}
};
