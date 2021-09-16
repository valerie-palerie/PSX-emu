#pragma once
#include "CXD8530BQ.h"
#include "Memory.h"

class Playstation
{
private:
	MemoryChip _bios;
	MemoryChip _dram;
	CXD8530BQ _cpu;

public:
	MemoryChip* bios() { return &_bios; }
	MemoryChip* dram() { return &_dram; }
	CXD8530BQ* cpu() { return &_cpu; }

	void Init();
	void Tick(float deltaTime);

	Playstation();
};