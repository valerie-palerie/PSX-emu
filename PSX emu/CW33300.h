#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include "Processor.h"
#include "DebugUtils.h"
#include "MathUtils.h"

class CXD8530BQ;

class CW33300 : public Processor
{
	friend class CXD8530BQ;

protected:
	//Internal registers separate from the 32 user available registers.
	//Initialize with 0xbadbad to make it clear what's uninitialized memory and what isn't.
	uint32 _r_pc = 0xbadbad;
	uint32 _r_npc = 0xbadbad; //Not a real hardware register, helper for jumps.
	uint32 _r_lo = 0xbadbad;
	uint32 _r_hi = 0xbadbad;

	uint32 _currentInstructionAddress = 0xbadbad; //Address of currently executing instruction

	uint32 _delaySlotAddress = 0xbadbad; //Address of upcoming delay slot

	std::vector<ProcessorInstruction> _branchInstructionMap;
	std::vector<ProcessorInstruction> _specialInstructionMap;

#if _DEBUG
	std::vector<std::unique_ptr<Debug::BaseProcessorDebugCondition>> _debugConditions;
#endif

	bool isExecutingDelaySlot() const { return _delaySlotAddress > 0x0 && _currentInstructionAddress == _delaySlotAddress; }

public:
	//****** Instruction definitions ******//
#define INST(name) int8 op_##name(const Opcode& op)
	//---ALU arithmetic
	INST(add); INST(addu); INST(sub); INST(subu); INST(addi); INST(addiu);
	//---ALU Comparison
	INST(slt); INST(sltu); INST(slti); INST(sltiu);
	//---ALU Logical
	INST(and); INST(or); INST(xor); INST(nor); INST(andi); INST(ori); INST(xori);
	//---ALU Shifting
	INST(sllv); INST(srlv); INST(srav); INST(sll); INST(srl); INST(sra); INST(lui);
	//---ALU Multiply/div
	INST(mult); INST(multu); INST(div); INST(divu); INST(mfhi); INST(mflo); INST(mthi); INST(mtlo);
	//---Load
	INST(lb); INST(lbu); INST(lh); INST(lhu); INST(lw); INST(lwl); INST(lwr);
	//---Store
	INST(sb); INST(sh); INST(sw); INST(swr); INST(swl);
	//---Jump
	INST(j); INST(jal); INST(jr); INST(jalr); INST(beq); INST(bne); INST(bltz); INST(bgez); INST(bgtz); INST(blez); INST(bltzal); INST(bgezal);
	//---Coprocessor
	INST(copn); INST(lwcn); INST(swcn); //LWCn and SWCn might need to be implemented on the coprocessor, i'll get back to this when i know more.
	//---Exception
	INST(syscall); INST(break);
	//---Utility
	INST(invalid);
#undef INST

	//****** Processor Implementation ******//
	virtual void Init() override;
	void SetRegisterImmediate(uint8 index, uint32 value) override;
	void ExecuteInstruction(Opcode opcode) override;

	//****** CW333300 ******//
	Processor* GetCoprocessor(uint8 idx) const;

	void MoveExecutionTo(uint32 address);
	void ProcessNextInstruction();
	ProcessorInstruction* DecodeInstruction(const Opcode& opcode);
	void Jump(uint32 address);

	explicit CW33300(CXD8530BQ* cpu);
};