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
		Chopping
	};

	struct Channel
	{
		uint32 baseAddress;
		uint32 blockControl;
		uint32 channelControl;
		uint32 unknown0xc;

		bool transferFromRAM() const { return Math::GetBit(channelControl, 0); }
		int memoryStep() const { return Math::GetBit(channelControl, 1) ? -4 : 4; }
		bool choppingEnabled() const { return Math::GetBit(channelControl, 8); }
		uint syncMode() const { return Math::GetBits(channelControl, 9, 10); }
		uint choppingWindowDMA() const { return 1 << Math::GetBits(channelControl, 16, 18); }
		uint choppingWindowCPU() const { return 1 << Math::GetBits(channelControl, 20, 22); }

		bool isReadyForTransfer() const;
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

	std::vector<Channel> _channels;
	Registers _registers;

	Status _status;
	Channel* _activeChannel;
	uint32 _currentTransferAddress;
	uint _transferCount;
	uint _remainingChoppingCycles;

public:
	Status status() const { return _status; }

	virtual void Init() override;
	virtual void Tick(double deltaT) override;

	virtual bool Write(uint32 address, const void* data, size_t size) override;

	Channel* GetChannelAtAddress(uint32 address);
	Channel* GetHighestPriorityActiveChannel();
	void SetStatus(Status status);

	void HandleIRQ();
	void HandleChopping();
	void HandleActiveTransfer();

	void HandleTransferComplete();

	DMAController(Playstation* playstation);
};

