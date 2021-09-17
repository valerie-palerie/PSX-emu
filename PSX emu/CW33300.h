#pragma once
#include <cstdint>
#include <vector>
#include "Processor.h"
#include "Utils.h"

class CXD8530BQ;

class CW33300 : public Processor
{
protected:
	//Internal registers separate from the 32 user available registers.
	//Initialize with 0xbadbad to make it clear what's uninitialized memory and what isn't.
	std::uint32_t _r_pc = 0xbadbad;
	std::uint32_t _r_lo = 0xbadbad;
	std::uint32_t _r_hi = 0xbadbad;

	std::uint32_t _nextInstruction = 0xbadbad;
	std::uint32_t _debugPC = 0xbadbad;

	std::vector<ProcessorInstruction> _branchInstructionMap;
	std::vector<ProcessorInstruction> _specialInstructionMap;

public:
	//****** Instruction definitions ******//
#define INST(name) std::int8_t op_##name(const Opcode& op)
	//---ALU arithmetic
	INST(add); INST(addu); INST(sub); INST(subu); INST(addi); INST(addiu);
	//---ALU Comparison
	INST(slt); INST(sltu); INST(slti); INST(sltiu);
	//---ALU Logical
	INST(and); INST(or ); INST(xor); INST(nor); INST(andi); INST(ori); INST(xori);
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
	std::uint32_t GetRegister(std::uint8_t index) const override;
	void SetRegister(std::uint8_t index, std::uint32_t value) override;
	void ExecuteInstruction(Opcode opcode) override;

	//****** CW333300 ******//
	Processor* GetCoprocessor(std::uint8_t idx) const;
	void ProcessNextInstruction();
	ProcessorInstruction* DecodeInstruction(const Opcode& opcode);
	void Jump(std::uint32_t address);

	explicit CW33300(CXD8530BQ* cpu);
};