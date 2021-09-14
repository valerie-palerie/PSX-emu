#pragma once
#include <vector>

struct ProcessorInstruction;
struct Opcode;
class CXD8530BQ;

class Processor
{
private:
	CXD8530BQ* _cpu;

protected:
	std::vector<std::uint32_t> _registers_read;
	std::vector<std::uint32_t> _registers_write;

	std::vector<ProcessorInstruction> _instructionMap;

public:
	CXD8530BQ* cpu() const { return _cpu; }

	virtual std::uint32_t GetRegister(std::uint8_t index);
	virtual void SetRegister(std::uint8_t index, std::uint32_t value);

	virtual void ExecuteInstruction(Opcode opcode);

	explicit Processor(CXD8530BQ* cpu);
	virtual ~Processor() = default;
};