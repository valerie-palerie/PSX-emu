#pragma once
#include "Processor.h"
#include <cstdint>
#include <vector>

class COP0 : public Processor
{

public:
	//****** Instruction definitions ******//
#define INST(name) std::int8_t op_##name(const Opcode& op)
	//---Coprocessor
	INST(mfc); INST(cfc); INST(mtc); INST(ctc); INST(lwc); INST(swc); INST(bcf); INST(bct); INST(rfe);//? INST(tlbx);
	//---Utility
	INST(invalid);
#undef INST
	//****** Processor Implementation ******//
	std::uint32_t GetRegister(std::uint8_t index) const override;
	void SetRegister(std::uint8_t index, std::uint32_t value) override;
	void ExecuteInstruction(Opcode opcode) override;

	//****** COP0 ******//
	ProcessorInstruction* DecodeInstruction(const Opcode& opcode);

	explicit COP0(CXD8530BQ* cpu);
};

