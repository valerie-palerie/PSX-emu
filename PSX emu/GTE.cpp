#include "GTE.h"
#include "Utils.h"

void GTE::ExecuteInstruction(Opcode opcode)
{
}

GTE::GTE(CXD8530BQ* cpu): Processor(cpu)
{
	_registers_read.resize(32, 0xbadbad);
}
