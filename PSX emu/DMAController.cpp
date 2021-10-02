#include "DMAController.h"
#include "MemoryTypes.h"
#include "Playstation.h"
#include "MathUtils.h"

void DMAController::Init()
{
	std::uint32_t dmaRegValue = 0x07654321;
	Write(0x70, &dmaRegValue, sizeof(std::uint32_t));
}

void DMAController::Tick(double deltaT)
{
	/*
	for (size_t i = 0; i < 7; ++i)
	{
		std::uint32_t channelBase = (0x10 * i);

		std::uint32_t baseAddress;
		std::uint32_t blockControl;
		std::uint32_t channelControl;
		Read(channelBase, &baseAddress, sizeof(std::uint32_t));
		Read(channelBase + 0x4, &blockControl, sizeof(std::uint32_t));
		Read(channelBase + 0x8, &channelControl, sizeof(std::uint32_t));

		std::uint32_t blockSize;
		std::uint32_t blockAmount;
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
	HandleIRQ();
}

void DMAController::HandleIRQ()
{
	//IF b15=1 OR (b23=1 AND (b16-22 AND b24-30)>0) THEN b31=1 ELSE b31=0
	std::uint32_t interruptRegister;
	Read(0x74, &interruptRegister, sizeof(std::uint32_t));

	bool b31 = Math::GetBit(interruptRegister, 31);
	bool b15= Math::GetBit(interruptRegister, 15);
	bool b23 = Math::GetBit(interruptRegister, 23);

	bool triggerIRQ 
		= b15 
		|| (b23 && (Math::GetBits(interruptRegister, 16, 22) & Math::GetBits(interruptRegister, 24, 30)));

	if (!b31 && triggerIRQ)
	{
		std::uint32_t irqStatus;
		_playstation->memInterface()->Read(MemoryMap::INTERRUPT_CONTROLLER_BASE, irqStatus);
		_playstation->memInterface()->Write<std::uint32_t>(MemoryMap::INTERRUPT_CONTROLLER_BASE, Math::ToggleBit(irqStatus, 3, true));
	}

	interruptRegister = Math::ToggleBit(interruptRegister, 31, triggerIRQ);
	Write(0x74, &interruptRegister, sizeof(std::uint32_t));
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
