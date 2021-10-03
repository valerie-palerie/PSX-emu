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
	enum Map
	{
		DMA0_BASE = 0x0,
		DMA1_BASE = 0x10,
		DMA2_BASE = 0x20,
		DMA3_BASE = 0x30,
		DMA4_BASE = 0x40,
		DMA5_BASE = 0x50,
		DMA6_BASE =	0x60,
		DMA_CONTROL = 0x70,
		DMA_INTERRUPT = 0x74,

		BLOCK_CONTROL_OFFSET = 0x4,
		CHANNEL_CONTROL_OFFSET = 0x8,
	};

	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	void HandleIRQ();
	void HandleTransfers();
	std::vector<uint> GetSortedActiveChannels() const;

	DMAController(Playstation* playstation);
};

