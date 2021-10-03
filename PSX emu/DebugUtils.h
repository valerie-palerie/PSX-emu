#pragma once
#include <cstdint>
#include <utility>
#include <functional>
#include <string>
#include <vector>
#include <unordered_set>
#include "MathUtils.h"
#include "MemoryTypes.h"

struct ProcessorInstruction;
struct Opcode;
class Processor;
#define ENABLE_DEBUG_LOG 0
#define DEBUG_LOG_ENABLED (ENABLE_DEBUG_LOG && _DEBUG)

namespace Debug
{
	struct BaseProcessorDebugCondition
	{
	protected:
		int _allowedTriggerAmount;

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc) = 0;

	public:
		bool EvaluateCondition(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc);
		BaseProcessorDebugCondition(int allowedTriggerAmount) : _allowedTriggerAmount(allowedTriggerAmount) {}
		virtual ~BaseProcessorDebugCondition() = default;
	};

	struct ProcessorDebugCondition_ReachFirstOfInstruction : public BaseProcessorDebugCondition
	{
	protected:
		std::unordered_set<std::string> _encounteredInstructions;
		std::string _instruction; //if empty, track all instructions

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc) override;
	public:

		ProcessorDebugCondition_ReachFirstOfInstruction(std::string instruction = "", int allowedTriggerAmount = -1);
	};

	struct ProcessorDebugCondition_FirstOfInstructionMatchesSignature : public ProcessorDebugCondition_ReachFirstOfInstruction
	{
	protected:
		uint32 _signature;

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc) override;
	public:
		ProcessorDebugCondition_FirstOfInstructionMatchesSignature(std::string instruction = "", uint32 signature = 0x0, int allowedTriggerAmount = -1)
			: ProcessorDebugCondition_ReachFirstOfInstruction(instruction, allowedTriggerAmount)
			, _signature(signature)
		{
		}
	};

	struct ProcessorDebugCondition_ReachAddress : public BaseProcessorDebugCondition
	{
		uint32 _address;

		virtual bool EvaluateCondition_Internal(Processor* processor, const Opcode& currentOpcode, ProcessorInstruction* currentInstruction, uint32 pc) override;
	public:

		ProcessorDebugCondition_ReachAddress(uint32 address, int allowedTriggerAmount = 1);
	};

	struct MemoryDebugCondition_MemoryAccess
	{
	protected:
		int _allowedTriggerAmount;
		AddressRange _addressRange;
		MemoryAccessFlags _accessFlags;

	public:
		bool EvaluateCondition(uint32 address, MemoryAccessFlags accessFlags);

		MemoryDebugCondition_MemoryAccess(AddressRange addressRange, MemoryAccessFlags accessFlags, int allowedTriggerAmount)
		: _allowedTriggerAmount(allowedTriggerAmount)
		, _addressRange(addressRange)
		, _accessFlags(accessFlags)
		{}

		virtual ~MemoryDebugCondition_MemoryAccess() = default;
	};

	void LogInstruction(const Processor* processor, const Opcode& opcode, ProcessorInstruction* instruction, uint32 pc);
	void LogRegisterWrites(const std::vector<uint32>& readRegs, const std::vector<uint32>& writeRegs);

}

//Get/Set functions for the register pointed to by the opcode part.
#define R_GET(name) auto (name) = GetRegister(op.name)
#define R_SET(name, val) SetRegister(op.name, val)
#define R_SET_DELAY(name, val, delay) SetRegister(op.name, val, delay)
