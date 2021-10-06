#include "DMAController.h"
#include "MemoryTypes.h"
#include "Playstation.h"
#include "MathUtils.h"

inline bool DMAController::Channel::isReadyForTransfer() const
{ 
	if (!Math::GetBit(channelControl, 24))
		return false;

	return syncMode() != 0 || Math::GetBit(channelControl, 28);
}

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
	case Status::Chopping:
		HandleChopping();
		break;
	case Status::Active:
		HandleActiveTransfer();
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
		std::memcpy(&_registers + address, data, size);

	Channel* affectedChannel = GetChannelAtAddress(address);
	if (affectedChannel != nullptr)
	{
		if (affectedChannel->isReadyForTransfer())
		{
			SetStatus(Status::Active);
		}
	}

	return true;
}

void DMAController::SetStatus(Status status)
{
	switch (status)
	{
	case Status::Active:
	{
		_activeChannel = GetHighestPriorityActiveChannel();
		_activeChannel->channelControl = Math::ToggleBit(_activeChannel->channelControl, 28, false);

		uint syncMode = _activeChannel->syncMode();
		if (syncMode == 0)
		{
			_remainingChoppingCycles = _activeChannel->choppingWindowDMA();
		}

		_currentTransferAddress = Math::GetBits(_activeChannel->baseAddress, 0, 23);
		_transferCount = 0;
		break;
	}
	case Status::Inactive:

		break;
	case Status::Chopping:
		uint syncMode = _activeChannel->syncMode();
		if (syncMode == 0)
		{
			_remainingChoppingCycles = _activeChannel->choppingWindowCPU();
		}
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

void DMAController::HandleChopping()
{
	if (--_remainingChoppingCycles == 0)
	{
		SetStatus(Status::Active);
	}
}

void DMAController::HandleActiveTransfer()
{
	uint syncMode = _activeChannel->syncMode();
	if (syncMode >= 3)
	{
		__debugbreak();
	}

	uint32 blockSize = 0;
	uint32 blockAmount = 0;
	switch (syncMode)
	{
	case 0:
		blockSize = 1;
		blockAmount = Math::GetBits(_activeChannel->blockControl, 0, 15);
		break;
	case 1:
		blockSize = Math::GetBits(_activeChannel->blockControl, 0, 15);
		blockAmount = Math::GetBits(_activeChannel->blockControl, 16, 31);
		break;
	case 2:
		blockSize = 1;
		blockAmount = 0;
		break;
	}

	if (blockAmount == 0)
		blockAmount = 0x10000;
	if (blockSize == 0)
		blockSize = 0x10000;

	//transfer word at _currentTransferAddress
	_transferCount += 1;
	_currentTransferAddress += _activeChannel->memoryStep();

	switch (syncMode)
	{
	case 0:
		if (_activeChannel->choppingEnabled())
		{
			blockAmount -= 1;
			Math::SetBits(_activeChannel->blockControl, 0, 15, blockAmount);
			_activeChannel->baseAddress = Math::SetBits(_activeChannel->baseAddress, 23, 0, _currentTransferAddress);
			if (blockAmount == 0)
			{
				HandleTransferComplete();
			}

			if (--_remainingChoppingCycles <= 0)
			{
				SetStatus(Status::Chopping);
			}
		}
		else
		{
			if (_transferCount == blockAmount)
			{
				HandleTransferComplete();
			}
		}
		break;
	case 1:
		_activeChannel->baseAddress = Math::SetBits(_activeChannel->baseAddress, 23, 0, _currentTransferAddress);
		if (_transferCount >= blockSize)
		{
			blockAmount -= 1;
			Math::SetBits(_activeChannel->blockControl, 16, 31, blockAmount);
			if (blockAmount == 0)
			{
				HandleTransferComplete();
			}
			else
			{
				SetStatus(Status::Inactive);
			}
		}
		break;
	case 2:
		//linked list
		break;
	}
}

void DMAController::HandleTransferComplete()
{
	Math::ToggleBit(_activeChannel->channelControl, 24, false);

	_activeChannel = GetHighestPriorityActiveChannel();
	if (_activeChannel != nullptr)
	{
		SetStatus(Status::Active);
	}
	else
	{
		SetStatus(Status::Inactive);
	}
}

DMAController::Channel* DMAController::GetChannelAtAddress(uint32 address)
{
	if (address >= 0x70)
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
	Math::ToggleBit(_channels[6].channelControl, 1, true);
}