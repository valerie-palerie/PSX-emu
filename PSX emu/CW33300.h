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
		typedef std::int32_t reg_t;
		typedef std::uint32_t ureg_t;
		reg_t zero = 0;
		reg_t at = 0;
		reg_t v[2];
		reg_t a[4];
		reg_t t[10];
		reg_t s[8];
		reg_t k[2];
		reg_t gp = 0;
		reg_t sp = 0;
		reg_t fp = 0;
		reg_t ra = 0;
		ureg_t pc = 0xbfc00000;
		reg_t hi = 0;
		reg_t lo = 0;

	protected:
		reg_t& R(std::uint8_t index);

	public:
		reg_t R_get(std::uint8_t index);
		void R_set(std::uint8_t index, reg_t value);
	};

	Registers registers;

	//****** Instruction definitions ******//
#define INST(name) void op_##name(const Opcode& op)
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
	INST(jmp); INST(jal); INST(jr); INST(jalr); INST(beq); INST(bne); INST(bltz); INST(bgez); INST(bgtz); INST(blez); INST(bltzal); INST(bgezal);
#undef INST
	//****** Instruction definitions ******//

	void ProcessNextInstruction();

	CW33300(CXD8530BQ* cpu) : Processor(cpu) {}
};

