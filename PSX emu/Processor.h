#pragma once
#include <functional>
#include <string>
#include <vector>
#include "MathUtils.h"
#include "ProcessorTypes.h"

class CXD8530BQ;

class Processor
{
private:
	CXD8530BQ* _cpu;

protected:
	std::vector<uint32> _registers;

	std::vector<ProcessorInstruction> _instructionMap;

public:
	CXD8530BQ* cpu() const { return _cpu; }

	virtual void Init() {}

	virtual uint32 GetRegister(uint8 index) const;
	virtual void SetRegister(uint8 index, uint32 value, uint8 delay = 0);
	virtual void SetRegisterImmediate(uint8 index, uint32 value);

	virtual void ExecuteInstruction(Opcode opcode) = 0;

	explicit Processor(CXD8530BQ* cpu);
	virtual ~Processor() = default;
};