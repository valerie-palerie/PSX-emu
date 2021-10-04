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
	switch (_status)
	{
	case Status::Inactive:

		break;
	case Status::Waiting:
		break;
	case Status::Chopping:
		HandleHalting();
		break;
	case Status::Active:
		HandleTransfers();
		break;
	}
}

bool DMAController::Write(uint32 address, const void* data, size_t size)
{
	if (address + size > 0x80)
		return false;

	if (address < 0x70)
		std::memcpy(&_channels[0] + address, data, size);
	else
		std::memcpy(&_registers[0] + address, data, size);

	Channel* affectedChannel = GetChannelAtAddress(address);
	if (affectedChannel != nullptr)
	{
		if (Math::GetBit(affectedChannel->channelControl, 24))
		{
			HandleChannelActivated();
		}
	}

	return true;
}

void DMAController::SetState(Status status)
{
	switch (status)
	{
	case Status::Active:
	{
		Channel* activeChannel = GetHighestPriorityActiveChannel();
		activeChannel->channelControl = Math::ToggleBit(activeChannel->channelControl, 28, false);
		break;
	}
	case Status::Inactive:

		break;
	case Status::Chopping:

		break;
	case Status::Waiting:

		break;
	}	

	_status = status;
}

void DMAController::HandleIRQ()
{
	bool b31 = Math::GetBit(_registers.dmaInterrupt, 31);
	bool b15 = Math::GetBit(_registers.dmaInterrupt, 15);
	bool b23 = Math::GetBit(_registers.dmaInterrupt, 23);

	bool triggerIRQ
		= b15
		|| (b23 && (Math::GetBits(_registers.dmaInterrupt, 16, 22) & Math::GetBits(_registers.dmaInterrupt, 24, 30)));

	if (!b31 && triggerIRQ)
	{
		uint32 irqStatus;
		_playstation->memInterface()->Read(MemoryMap::INTERRUPT_CONTROLLER_BASE, irqStatus);
		_playstation->memInterface()->Write<uint32>(MemoryMap::INTERRUPT_CONTROLLER_BASE, Math::ToggleBit(irqStatus, 3, true));
	}

	_registers.dmaInterrupt = Math::ToggleBit(_registers.dmaInterrupt, 31, triggerIRQ);
}

void DMAController::HandleHalting()
{

}

void DMAController::HandleTransfers()
{
	Channel* activeChannel = GetHighestPriorityActiveChannel();
	if (activeChannel == nullptr)
	{
		SetState(Status::Inactive);
		return;
	}

	uint syncMode = activeChannel->syncMode();

	uint32 blockSize = 0;
	uint32 blockAmount = 0;
	switch (syncMode)
	{
	case 0:
		blockSize = 1;
		blockAmount = Math::GetBits(activeChannel->blockControl, 0, 15);
		break;
	case 1:
		blockSize = Math::GetBits(activeChannel->blockControl, 0, 15);
		blockAmount = Math::GetBits(activeChannel->blockControl, 16, 31);
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

DMAController::Channel* DMAController::GetChannelAtAddress(uint32 address)
{
	if(address >= 0x70)
	return nullptr;

	return &_channels[address / 0x10];
}

DMAController::Channel* DMAController::GetHighestPriorityActiveChannel()
{
	Channel* validChannel = nullptr;
	uint validChannelPriority = 8;
	for (int i = 0; i <= 6; ++i)
	{
		bool channelEnabled = Math::GetBit(_registers.dmaControl, 3 + 4 * i);
		if (!channelEnabled)
		{
			continue;
		}

		if (!Math::GetBit(_channels[i].channelControl, 1))
		{
			continue;
		}

		int channelPriority = Math::GetBits(_registers.dmaControl, i * 4, i * 4 + 2);
		if (channelPriority <= validChannelPriority)
		{
			validChannel = &_channels[i];
			validChannelPriority = channelPriority;
		}
	}

	return validChannel;
}

DMAController::DMAController(Playstation* playstation)
	: PlaystationComponent(playstation)
	, _dram(playstation->dram())
	, _mdec(playstation->mdec())
	, _gpu(playstation->gpu())
	, _cdrom(playstation->cdrom())
	, _spu(playstation->spu())
	, _status(Status::Inactive)
	, _remainingChoppingCycles(0)
	, _channels(7)
	, _registers()
{
}
