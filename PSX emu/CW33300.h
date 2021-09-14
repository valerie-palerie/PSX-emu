#pragma once
#include <cstdint>
#include <vector>
#include "Processor.h"
#include "Utils.h"

class CXD8530BQ;

class CW33300 : public Processor
{
public:
	struct Registers
	{
	public:
		using reg_t = std::uint32_t;
		reg_t zero = 0;					//R0
		reg_t at = 0xbadbad;			//R1
		reg_t v[2] = { 0xbadbad };		//R2-R3
		reg_t a[4] = { 0xbadbad };		//R4-R7
		reg_t t[10] = { 0xbadbad };		//R8-R15 (0-7) + R24-R25 (8-9)
		reg_t s[8] = { 0xbadbad };		//R16-R23
		reg_t k[2] = { 0xbadbad };		//R26-R27
		reg_t gp = 0xbadbad;			//R28
		reg_t sp = 0xbadbad;			//R29
		reg_t fp = 0xbadbad;			//R30
		reg_t ra = 0xbadbad;			//R31
		reg_t pc = MEM_BIOS;			//Program Counter
		reg_t hi = 0xbadbad;			//Div Remainder
		reg_t lo = 0xbadbad;			//Div Quotient

	protected:
		reg_t& R(std::uint8_t index);

	public:
		reg_t R_get(std::uint8_t index);
		void R_set(std::uint8_t index, reg_t value);
	};

	Registers registers;

#if _DEBUG
	Registers debugRegisters;
#endif


	std::vector<ProcessorInstruction> instructionMap;
	std::vector<ProcessorInstruction> branchInstructionMap;
	std::vector<ProcessorInstruction> specialInstructionMap;

	std::uint32_t nextInstruction = 0x0;

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
	INST(copn); INST(lwcn); INST(swcn);
	//---Exception
	INST(syscall); INST(break);
	//---Utility
	INST(invalid);
#undef INST
	//****** Instruction definitions ******//

	ProcessorInstruction* MapInstruction(const Opcode& opcode);
	void ProcessNextInstruction();

	CW33300(CXD8530BQ* cpu);
};

