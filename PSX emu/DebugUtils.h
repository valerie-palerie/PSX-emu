#pragma once
#include <cstdint>
#include <utility>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>

#include "MemoryTypes.h"

struct ProcessorInstruction;
struct Opcode;
class Processor;
#define ENABLE_DEBUG_LOG 1
#define DEBUG_LOG_ENABLED (ENABLE_DEBUG_LOG && _DEBUG)

namespace Debug
{
	struct BaseProcessorDebugCondition
	{
	protected:
		int _allowedTriggerAmount;

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, std::uint32_t pc) = 0;

	public:
		bool EvaluateCondition(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, std::uint32_t pc);
		BaseProcessorDebugCondition(int allowedTriggerAmount) : _allowedTriggerAmount(allowedTriggerAmount) {}
		virtual ~BaseProcessorDebugCondition() = default;
	};

	struct ProcessorDebugCondition_ReachFirstOfInstruction : public BaseProcessorDebugCondition
	{
	protected:
		std::unordered_set<std::string> _encounteredInstructions;
		std::string _instruction; //if empty, track all instructions

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, std::uint32_t pc) override;
	public:

		ProcessorDebugCondition_ReachFirstOfInstruction(std::string instruction = "", int allowedTriggerAmount = -1);
	};

	struct ProcessorDebugCondition_FirstOfInstructionMatchesSignature : public ProcessorDebugCondition_ReachFirstOfInstruction
	{
	protected:
		std::uint32_t _signature;

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, std::uint32_t pc) override;
	public:
		ProcessorDebugCondition_FirstOfInstructionMatchesSignature(std::string instruction = "", std::uint32_t signature = 0x0, int allowedTriggerAmount = -1)
			: ProcessorDebugCondition_ReachFirstOfInstruction(instruction, allowedTriggerAmount)
			, _signature(signature)
		{
		}
	};

	struct ProcessorDebugCondition_ReachAddress : public BaseProcessorDebugCondition
	{
		std::uint32_t _address;

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, std::uint32_t pc) override;
	public:

		ProcessorDebugCondition_ReachAddress(std::uint32_t address, int allowedTriggerAmount = 1);
	};

	struct MemoryDebugCondition_MemoryAccess
	{
	protected:
		int _allowedTriggerAmount;
		AddressRange _addressRange;
		MemoryAccessFlags _accessFlags;

	public:
		bool EvaluateCondition(std::uint32_t address, MemoryAccessFlags accessFlags);

		MemoryDebugCondition_MemoryAccess(AddressRange addressRange, MemoryAccessFlags accessFlags, int allowedTriggerAmount)
		: _allowedTriggerAmount(allowedTriggerAmount)
		, _addressRange(addressRange)
		, _accessFlags(accessFlags)
		{}

		virtual ~MemoryDebugCondition_MemoryAccess() = default;
	};

	void LogInstruction(const Processor* processor, const Opcode& opcode, ProcessorInstruction* instruction, std::uint32_t pc);
	void LogRegisterWrites(const std::vector<std::uint32_t>& readRegs, const std::vector<std::uint32_t>& writeRegs);

}

//Get/Set functions for the register pointed to by the opcode part.
#define R_GET(name) auto (name) = GetRegister(op.name)
#define R_SET(name, val) SetRegister(op.name, val)
#define R_SET_DELAY(name, val, delay) SetRegister(op.name, val, delay)
