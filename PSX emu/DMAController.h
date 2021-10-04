#pragma once
#include "Memory.h"
#include "PlaystationComponent.h"
#include "MathUtils.h"

class Playstation;
class MemoryChip;
class MDEC;
class GPU;
class CDROMController;
class SPU;

class DMAController : public PlaystationComponent, public IMemory
{
public:
	enum class Status
	{
		Inactive,
		Active,
		Chopping,
		Waiting
	};

	struct Channel
	{
		uint32 baseAddress;
		uint32 blockControl;
		uint32 channelControl;
		uint32 unknown0xfc;

		uint syncMode() const { return Math::GetBits(channelControl, 9, 10); }
	};

	struct Registers
	{
		uint32 dmaControl;
		uint32 dmaInterrupt;
		uint32 unknown0xf8;
		uint32 unknown0xfc;
	};

	static_assert(sizeof(Channel) == (sizeof(uint32) * 4));
	static_assert(sizeof(Registers) == (sizeof(uint32) * 4));

protected:
	MemoryChip* _dram;
	MDEC* _mdec;
	GPU* _gpu;
	CDROMController* _cdrom;
	SPU* _spu;
	
	Status _status;
	uint _remainingChoppingCycles;

	std::vector<Channel> _channels;
	Registers _registers;

public:
	Status status() const { return _status; }

	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	virtual bool Write(uint32 address, const void* data, size_t size) override;

	Channel* GetChannelAtAddress(uint32 address);
	Channel* GetHighestPriorityActiveChannel();
	void SetState(Status status);

	void HandleChannelActivated();
	void HandleIRQ();
	void HandleHalting();
	void HandleTransfers();

	DMAController(Playstation* playstation);
};

