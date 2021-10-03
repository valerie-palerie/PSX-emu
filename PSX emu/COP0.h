#pragma once
#include "Processor.h"
#include "MathUtils.h"
#include <cstdint>
#include <vector>

class COP0 : public Processor
{
	friend class CXD8530BQ;

public:
	//****** Instruction definitions ******//
#define INST(name) int8 op_##name(const Opcode& op)
	//---Coprocessor
	INST(mfc); INST(cfc); INST(mtc); INST(ctc); INST(lwc); INST(swc); INST(bcf); INST(bct); INST(rfe);//? INST(tlbx);
	//---Utility
	INST(invalid);
#undef INST
	//****** Processor Implementation ******//
	void ExecuteInstruction(Opcode opcode) override;

	//****** COP0 ******//
	ProcessorInstruction* DecodeInstruction(const Opcode& opcode);

	explicit COP0(CXD8530BQ* cpu);
};

