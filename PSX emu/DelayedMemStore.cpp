#include "DelayedMemStore.h"
#include "Processor.h"

bool BaseDelayedMemStore::Tick()
{
	if (_remainingDelay-- == 0)
	{
		ApplyData();
		return true;
	}

	return false;
}

void DelayedMemStore_ProcessorReg::ApplyData()
{
	if (_processor != nullptr)
	{
		_processor->SetRegisterImmediate(_register, _data);
		_processor = nullptr;
	}
}
