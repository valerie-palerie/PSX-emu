#include "Utils.h"
#include <iostream>
#include <bitset>
#include "Processor.h"

void Debug::LogInstruction(const Processor* processor, const Opcode& opcode, ProcessorInstruction* instruction, std::uint32_t pc, bool isDelaySlot)
{	
	//Print the details of the opcode we're processing.
	std::cout << std::hex << "0x" << pc << ": 0x" << opcode.opcode << std::dec << (isDelaySlot ? " (delay)" : "") << "\n[" << instruction->name << "] (" << std::bitset<6>(opcode.op) << ")(" << std::bitset<26>(opcode.cop) << ")\n";
	std::cout << "	op:" << std::uint16_t(opcode.op) << " rs : " << std::uint16_t(opcode.rs) << " rt : " << std::uint16_t(opcode.rt) << " rd : " << std::uint16_t(opcode.rd) << " shift : " << std::uint16_t(opcode.shift) << " func : " << std::uint16_t(opcode.func) << " imm : " << opcode.imm << " cop : " << opcode.cop << "\n";

	if (instruction->structure.segmentMask != 0)
	{
		switch (instruction->structure.format)
		{
		case InstructionFormat::R:
			std::cout << "	" << instruction->name;
			for (std::uint8_t i = 4; i > 0; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
				{
					if (i == 1 && !(opcode.func & 0b111100)) //stinky immediate shift instruction
						std::cout << std::hex << " 0x" << std::uint16_t(opcode.GetSegment(i)) << std::dec;
					else
						std::cout << " $" << std::uint16_t(opcode.GetSegment(i));
				}
			}

			std::cout << "\n	" << instruction->name;

			for (std::uint8_t i = 4; i > 0; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
				{
					if (i == 1 && !(opcode.func & 0b111100)) //stinky immediate shift instruction
						std::cout << " " << std::uint16_t(opcode.GetSegment(i));
					else
						std::cout << std::hex << " 0x" << processor->GetRegister(opcode.GetSegment(i)) << std::dec;
				}
			}
			break;
		case InstructionFormat::I:
			std::cout << "	" << instruction->name;
			for (std::uint8_t i = 4; i > 2; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
					std::cout << " $" << std::uint16_t(opcode.GetSegment(i));
			}
			std::cout << std::hex << " $imm 0x" << opcode.imm << std::dec << " (" << std::int16_t(opcode.imm) << ")";
			std::cout << "\n	" << instruction->name;

			for (std::uint8_t i = 4; i > 2; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
					std::cout << std::hex << " 0x" << processor->GetRegister(opcode.GetSegment(i)) << std::dec;
			}
			std::cout << std::hex << " $imm 0x" << opcode.imm << std::dec << " (" << std::int16_t(opcode.imm) << ")";

			break;
		case InstructionFormat::J:
			std::cout << std::hex << "	" << instruction->name << " 0x" << opcode.cop << " (0x" << (opcode.cop << 2) << ")" << std::dec;
			break;
		}

		std::cout << '\n';
	}
}

void Debug::LogRegisterWrites(const std::vector<std::uint32_t>& readRegs, const std::vector<std::uint32_t>& writeRegs)
{
	for (std::uint8_t i = 0; i < std::min(readRegs.size(), writeRegs.size()); ++i)
		if (writeRegs[i] != readRegs[i])
			std::cout << "		R" << std::uint16_t(i) << ": 0x" << std::hex << readRegs[i] << " -> 0x" << writeRegs[i] << "\n";
	std::cout << "\n";
}
