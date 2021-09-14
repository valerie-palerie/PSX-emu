#pragma once

class CXD8530BQ;

class Processor
{
private:
	CXD8530BQ* _cpu;

public:
	CXD8530BQ* cpu() const { return _cpu; }

	Processor(CXD8530BQ* cpu);
};