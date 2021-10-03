#include "DMAController.h"
#include "MemoryTypes.h"
#include "Playstation.h"
#include "MathUtils.h"

void DMAController::Init()
{
	uint32 dmaRegValue = 0x07654321;
	Write(0x70, &dmaRegValue, sizeof(uint32));
}

void DMAController::Tick(double deltaT)
{
	HandleIRQ();
	HandleTransfers();
}

void DMAController::HandleIRQ()
{
	uint32 interruptRegister;
	Read(DMA_INTERRUPT, &interruptRegister, sizeof(uint32));

	bool b31 = Math::GetBit(interruptRegister, 31);
	bool b15 = Math::GetBit(interruptRegister, 15);
	bool b23 = Math::GetBit(interruptRegister, 23);

	bool triggerIRQ
		= b15
		|| (b23 && (Math::GetBits(interruptRegister, 16, 22) & Math::GetBits(interruptRegister, 24, 30)));

	if (!b31 && triggerIRQ)
	{
		uint32 irqStatus;
		_playstation->memInterface()->Read(MemoryMap::INTERRUPT_CONTROLLER_BASE, irqStatus);
		_playstation->memInterface()->Write<uint32>(MemoryMap::INTERRUPT_CONTROLLER_BASE, Math::ToggleBit(irqStatus, 3, true));
	}

	interruptRegister = Math::ToggleBit(interruptRegister, 31, triggerIRQ);
	Write(0x74, &interruptRegister, sizeof(uint32));
}

void DMAController::HandleTransfers()
{
	/*
	for (uint channel : GetSortedActiveChannels())
	{
		uint32 channelBase = (0x10 * channel);

		uint32 baseAddress;
		uint32 blockControl;
		uint32 channelControl;
		Read(channelBase, &baseAddress, sizeof(uint32));
		Read(channelBase + BLOCK_CONTROL_OFFSET, &blockControl, sizeof(uint32));
		Read(channelBase + CHANNEL_CONTROL_OFFSET, &channelControl, sizeof(uint32));

		uint32 blockSize = 0;
		uint32 blockAmount = 0;
		switch (Math::GetBits(channelControl, 9, 10))
		{
		case 0:
			blockSize = 1;
			blockAmount = Math::GetBits(blockControl, 0, 15);
			break;
		case 1:
			blockSize = Math::GetBits(blockControl, 0, 15);
			blockAmount = Math::GetBits(blockControl, 16, 31);
			break;
		case 2:
			blockSize = 1;
			blockAmount = 0;
			break;
		case 3:
		default:
			__debugbreak();
			break;
		}

		if (blockAmount == 0)
			blockAmount = 0x10000;
		if (blockSize == 0)
			blockSize = 0x10000;
	}
	*/
}

std::vector<uint> DMAController::GetSortedActiveChannels() const
{
	uint32 dmaControl;
	Read(DMA_CONTROL, &dmaControl, sizeof(uint32));

	std::vector <std::pair<uint, uint>> channelPriorities;
	for (uint32 i = 0; i <= 6; ++i)
	{
		bool channelEnabled = Math::GetBit(dmaControl, 3 + 4 * i);
		if (channelEnabled)
		{
			channelPriorities.emplace_back(i, Math::GetBits(dmaControl, i * 4, i * 4 + 2));
		}
	}
	std::sort(channelPriorities.begin(), channelPriorities.end(),
		[](const std::pair<uint, uint>& a, const std::pair<uint, uint>& b) -> bool
		{
			return (a.second < b.second) || (a.first > b.first);
		});

	std::vector<uint> outVector;
	outVector.reserve(channelPriorities.size());
	for (auto& pair : channelPriorities)
		outVector.push_back(pair.first);

	return outVector;
}

DMAController::DMAController(Playstation* playstation)
	: PlaystationComponent(playstation)
	, MemoryChip(MemoryMap::DMA_SIZE)
	, _dram(playstation->dram())
	, _mdec(playstation->mdec())
	, _gpu(playstation->gpu())
	, _cdrom(playstation->cdrom())
	, _spu(playstation->spu())
{
}
