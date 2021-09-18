#pragma once
#include "Memory.h"
#include "ExpansionBoard.h"
#include "MemoryInterface.h"
#include "CXD8530BQ.h"

class Playstation
{
private:
	MemoryChip _dram;
	ExpansionBoard _exp1;
	MemoryChip _scratchPad;
	MemoryChip _io;
	ExpansionBoard _exp2;
	ExpansionBoard _exp3;
	MemoryChip _controlRegs;
	MemoryChip _bios;
	CXD8530BQ _cpu;

	MemoryInterface _memInterface;

public:
	MemoryChip* dram() { return &_dram; }
	ExpansionBoard* exp1() { return &_exp1; }
	MemoryChip* scratchPad() { return &_scratchPad; }
	MemoryChip* io() { return &_io; }
	ExpansionBoard* exp2() { return &_exp2; }
	ExpansionBoard* exp3() { return &_exp3; }
	MemoryChip* controlRegs() { return &_controlRegs; }
	MemoryChip* bios() { return &_bios; }
	CXD8530BQ* cpu() { return &_cpu; }

	MemoryInterface* memInterface() { return &_memInterface; }

	void Init();
	void Tick(float deltaTime);

	Playstation();
};