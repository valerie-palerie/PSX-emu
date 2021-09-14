#pragma once
#include "Processor.h"

class GTE : public Processor
{
public:
	//****** Processor Implementation ******//
	void ExecuteInstruction(Opcode opcode) override;

	explicit GTE(CXD8530BQ* cpu);
};

