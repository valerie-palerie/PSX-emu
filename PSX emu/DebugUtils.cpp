#include "DebugUtils.h"
#include <iostream>
#include <bitset>
#include "Processor.h"

bool Debug::BaseProcessorDebugCondition::EvaluateCondition(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc)
{
	if (_allowedTriggerAmount == 0)
		return false;

	bool cond = EvaluateCondition_Internal(processor, currentOpcode, currentInstruction, pc);
	if (cond && _allowedTriggerAmount > 0)
		--_allowedTriggerAmount;

	return cond;
}

bool Debug::ProcessorDebugCondition_ReachAddress::EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc)
{
	return pc == _address;
}

Debug::ProcessorDebugCondition_ReachAddress::ProcessorDebugCondition_ReachAddress(uint32 address, int allowedTriggerAmount)
	: BaseProcessorDebugCondition(allowedTriggerAmount)
	, _address(address)
{
}

bool Debug::ProcessorDebugCondition_ReachFirstOfInstruction::EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc)
{
	if (_instruction.length() == 0 || _instruction == currentInstruction->name)
	{
		if (_encounteredInstructions.find(currentInstruction->name) == _encounteredInstructions.end())
		{
			_encounteredInstructions.insert(currentInstruction->name);
			return true;
		}
	}
	return false;
}

Debug::ProcessorDebugCondition_ReachFirstOfInstruction::ProcessorDebugCondition_ReachFirstOfInstruction(std::string instruction, int allowedTriggerAmount)
	: BaseProcessorDebugCondition(allowedTriggerAmount)
	, _instruction(std::move(instruction))
{
}

void Debug::LogInstruction(const Processor* processor, const Opcode& opcode, ProcessorInstruction* instruction, uint32 pc)
{
	//Print the details of the opcode we're processing.
	std::cout << std::hex << "0x" << pc << ": 0x" << opcode.bits << std::dec << "\n[" << instruction->name << "] (" << std::bitset<6>(opcode.op) << ")(" << std::bitset<26>(opcode.cop) << std::hex << ")\n";
	std::cout << "	op:" << uint16(opcode.op) << " rs:" << uint16(opcode.rs) << " rt:" << uint16(opcode.rt) << " rd:" << uint16(opcode.rd) << " shift:" << uint16(opcode.shift) << " func:" << uint16(opcode.func) << " imm:" << opcode.imm << " cop:" << opcode.cop << "\n";

	if (instruction->structure.segmentMask != 0)
	{
		switch (instruction->structure.format)
		{
		case InstructionFormat::R:
			std::cout << "	" << instruction->name;
			for (uint8 i = 4; i > 0; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
				{
					if (i == 1 && !(opcode.func & 0b111100)) //stinky immediate shift instruction
						std::cout << std::hex << " 0x" << uint16(opcode.GetSegment(i)) << std::dec;
					else
						std::cout << " $" << uint16(opcode.GetSegment(i));
				}
			}

			std::cout << "\n	" << instruction->name;

			for (uint8 i = 4; i > 0; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
				{
					if (i == 1 && !(opcode.func & 0b111100)) //stinky immediate shift instruction
						std::cout << " " << uint16(opcode.GetSegment(i));
					else
						std::cout << std::hex << " 0x" << processor->GetRegister(opcode.GetSegment(i)) << std::dec;
				}
			}
			break;
		case InstructionFormat::I:
		{
			std::cout << "	" << instruction->name;
			int32 immValue = opcode.imm_se;
			bool isJump = false;
			if (instruction->name == "beq" || instruction->name == "bne" || instruction->name == "bltz" || instruction->name == "bgez" || instruction->name == "bgtz" || instruction->name == "blez" || instruction->name == "bltzal" || instruction->name == "bgezal")
			{
				immValue = immValue << 2;
				isJump = true;
			}
			for (uint8 i = 4; i > 2; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
					std::cout << " $" << uint16(opcode.GetSegment(i));
			}
			std::cout << std::hex << " $imm 0x" << opcode.imm << std::dec << " (" << immValue << ")";
			std::cout << "\n	" << instruction->name;

			for (uint8 i = 4; i > 2; --i)
			{
				if ((0x1 << i) & instruction->structure.segmentMask)
					std::cout << std::hex << " 0x" << processor->GetRegister(opcode.GetSegment(i)) << std::dec;
			}
			std::cout << std::hex << " $imm 0x" << opcode.imm << std::dec << (isJump ? std::hex : std::dec) << (isJump ? " (0x" : " (") << (isJump ? (pc + 4 + immValue) : immValue) << ")";

			break;
		}
		case InstructionFormat::J:
			std::cout << std::hex << "	" << instruction->name << " 0x" << opcode.cop << " (0x" << (opcode.cop << 2) << ")" << std::dec;
			break;
		}

		std::cout << '\n';
	}
}

void Debug::LogRegisterWrites(const std::vector<uint32>& readRegs, const std::vector<uint32>& writeRegs)
{
	for (uint8 i = 0; i < std::min(readRegs.size(), writeRegs.size()); ++i)
		if (writeRegs[i] != readRegs[i])
			std::cout << "		R" << uint16(i) << ": 0x" << std::hex << readRegs[i] << " -> 0x" << writeRegs[i] << "\n";
	std::cout << "\n";
}

bool Debug::MemoryDebugCondition_MemoryAccess::EvaluateCondition(uint32 address, MemoryAccessFlags accessFlags)
{
	if(_allowedTriggerAmount == 0)
		return false;

	if(_addressRange.Contains(address) && (uint8(accessFlags) & uint8(_accessFlags)))
	{
		if (_allowedTriggerAmount > 0)
			--_allowedTriggerAmount;

		return true;
	}

	return false;
}

bool Debug::ProcessorDebugCondition_FirstOfInstructionMatchesSignature::EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc)
{
	if (ProcessorDebugCondition_ReachFirstOfInstruction::EvaluateCondition_Internal(processor, currentOpcode, currentInstruction, pc))
		return _signature == currentOpcode.bits;
	return false;
}
