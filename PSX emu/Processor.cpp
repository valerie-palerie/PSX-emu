#include "Processor.h"
#include "CXD8530BQ.h"
#include "DebugUtils.h"

std::uint32_t Processor::GetRegister(std::uint8_t index) const
{
	return _registers[index];
}

void Processor::SetRegister(std::uint8_t index, std::uint32_t value, std::uint8_t delay)
{
	_cpu->QueueDelayMemStore<DelayedMemStore_ProcessorReg>(this, index, value, delay);
}

void Processor::SetRegisterImmediate(std::uint8_t index, std::uint32_t value)
{
	_registers[index] = value;
}

Processor::Processor(CXD8530BQ* cpu) :
	_cpu(cpu)
{
}
