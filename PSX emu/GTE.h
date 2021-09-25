#pragma once
#include "Processor.h"

class GTE : public Processor
{
	friend class CXD8530BQ;

public:
	//****** Processor Implementation ******//
	void ExecuteInstruction(Opcode opcode) override;

	explicit GTE(CXD8530BQ* cpu);
};

