#include "Processor.h"

#include "DebugUtils.h"

void Processor::SyncRegisters() 
{ 
	if (_needsRegisterSync)
	{
		_needsRegisterSync = false;
		_registers_read = _registers_write;
	}
}

std::uint32_t Processor::GetRegister(std::uint8_t index) const
{
	return _registers_read[index];
}

void Processor::SetRegister(std::uint8_t index, std::uint32_t value)
{
	_needsRegisterSync = true;
	_registers_write[index] = value;
}

Processor::Processor(CXD8530BQ* cpu) :
	_cpu(cpu)
{
}
