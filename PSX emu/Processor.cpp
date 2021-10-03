#include "Processor.h"
#include "CXD8530BQ.h"
#include "DebugUtils.h"

uint32 Processor::GetRegister(uint8 index) const
{
	return _registers[index];
}

void Processor::SetRegister(uint8 index, uint32 value, uint8 delay)
{
	_cpu->QueueDelayMemStore<DelayedMemStore_ProcessorReg>(this, index, value, delay);
}

void Processor::SetRegisterImmediate(uint8 index, uint32 value)
{
	_registers[index] = value;
}

Processor::Processor(CXD8530BQ* cpu) :
	_cpu(cpu)
{
}
