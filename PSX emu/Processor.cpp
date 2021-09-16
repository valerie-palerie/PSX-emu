#include "Processor.h"

#include "Utils.h"

std::uint32_t Processor::GetRegister(std::uint8_t index) const
{
	return _registers_read[index];
}

void Processor::SetRegister(std::uint8_t index, std::uint32_t value)
{
	_registers_write[index] = value;
}

void Processor::ExecuteInstruction(Opcode opcode)
{
	_registers_read = _registers_write;
}

Processor::Processor(CXD8530BQ* cpu) :
	_cpu(cpu)
{
}
