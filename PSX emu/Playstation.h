#pragma once
#include "CXD8530BQ.h"
#include "Memory.h"

class Playstation
{
private:
	CXD8530BQ _cpu;
	MemoryChip _bios;

public:
	CXD8530BQ* cpu() { return &_cpu; }
	MemoryChip* bios() { return &_bios; }

	void Tick(float deltaTime);

	Playstation();
};