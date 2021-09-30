#include "GTE.h"
#include "DebugUtils.h"

void GTE::ExecuteInstruction(Opcode opcode)
{
}

GTE::GTE(CXD8530BQ* cpu): Processor(cpu)
{
	_registers.resize(64, 0x0);
}
