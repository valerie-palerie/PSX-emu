#pragma once
#include <functional>
#include <string>
#include <vector>
#include "ProcessorTypes.h"

class CXD8530BQ;

class Processor
{
private:
	CXD8530BQ* _cpu;

protected:
	std::vector<std::uint32_t> _registers;

	std::vector<ProcessorInstruction> _instructionMap;

public:
	CXD8530BQ* cpu() const { return _cpu; }

	virtual void Init() {}

	virtual std::uint32_t GetRegister(std::uint8_t index) const;
	virtual void SetRegister(std::uint8_t index, std::uint32_t value, std::uint8_t delay = 0);
	virtual void SetRegisterImmediate(std::uint8_t index, std::uint32_t value);

	virtual void ExecuteInstruction(Opcode opcode) = 0;

	explicit Processor(CXD8530BQ* cpu);
	virtual ~Processor() = default;
};