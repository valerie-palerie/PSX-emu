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
	//Two sets of registers, loading a value from memory has a delay and we emulate it by reading and writing on different sets of registers, then syncing them after the instruction has finished.
	std::vector<std::uint32_t> _registers_read;
	std::vector<std::uint32_t> _registers_write;

	std::vector<ProcessorInstruction> _instructionMap;

public:
	CXD8530BQ* cpu() const { return _cpu; }

	virtual std::uint32_t GetRegister(std::uint8_t index) const;
	virtual void SetRegister(std::uint8_t index, std::uint32_t value);

	virtual void ExecuteInstruction(Opcode opcode);

	explicit Processor(CXD8530BQ* cpu);
	virtual ~Processor() = default;
};