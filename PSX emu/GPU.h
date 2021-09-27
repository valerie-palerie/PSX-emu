#pragma once
#include "Memory.h"
#include "PlaystationComponent.h"

class GPU : public PlaystationComponent, public MemoryChip
{
public:
	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	GPU(Playstation* playstation);
};

