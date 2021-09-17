#pragma once
#include "Memory.h"
#include "ExpansionBoard.h"
#include "MemoryInterface.h"
#include "CXD8530BQ.h"

class Playstation
{
private:
	MemoryChip _bios;
	MemoryChip _dram;
	MemoryChip _interruptControlRegister;
	MemoryChip _cacheControlRegister;
	ExpansionBoard _exp1;
	ExpansionBoard _exp2;
	ExpansionBoard _exp3;
	CXD8530BQ _cpu;

	MemoryInterface _memInterface;

public:
	MemoryChip* bios() { return &_bios; }
	MemoryChip* dram() { return &_dram; }
	ExpansionBoard* exp1() { return &_exp1; }
	ExpansionBoard* exp2() { return &_exp2; }
	ExpansionBoard* exp3() { return &_exp3; }
	CXD8530BQ* cpu() { return &_cpu; }

	MemoryInterface* memInterface() { return &_memInterface; }

	void Init();
	void Tick(float deltaTime);

	Playstation();
};