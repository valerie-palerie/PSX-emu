#pragma once
#include "Memory.h"
#include "ExpansionBoard.h"
#include "MemoryInterface.h"
#include "CXD8530BQ.h"
#include "DMAController.h"
#include "GPU.h"
#include "SPU.h"
#include "CDROMController.h"


class Playstation
{
private:
	MemoryChip _dram;
	ExpansionBoard _exp1;
	MemoryChip _scratchPad;
	HardwareRegs _memCtrl;
	MemoryChip _sio;			//Unimplemented controller
	HardwareRegs _memCtrl2;
	HardwareRegs _timers;		//maybe a controller???
	MemoryChip _mdec;			//unimplemented controller
	HardwareRegs _interruptCtrl;
	ExpansionBoard _exp2;
	ExpansionBoard _exp3;
	MemoryChip _bios;
	HardwareRegs _cacheCtrl;

	CXD8530BQ _cpu;
	GPU _gpu;
	SPU _spu;
	DMAController _dma;
	CDROMController _cdrom;
	MemoryInterface _memInterface;

	std::vector<PlaystationComponent*> _components;

public:
	MemoryChip* dram() { return &_dram; }
	ExpansionBoard* exp1() { return &_exp1; }
	MemoryChip* scratchPad() { return &_scratchPad; }
	HardwareRegs* memCtrl() { return &_memCtrl; }
	MemoryChip* sio() { return &_sio; }
	HardwareRegs* memCtrl2() { return &_memCtrl2; }
	HardwareRegs* timers() { return &_timers; }
	MemoryChip* mdec() { return &_mdec; }
	HardwareRegs* interruptCtrl() { return &_interruptCtrl; }
	ExpansionBoard* exp2() { return &_exp2; }
	ExpansionBoard* exp3() { return &_exp3; }
	MemoryChip* bios() { return &_bios; }
	HardwareRegs* cacheCtrl() { return &_cacheCtrl; }

	CXD8530BQ* cpu() { return &_cpu; }
	GPU* gpu() { return &_gpu; }
	SPU* spu() { return &_spu; }
	DMAController* dma() { return &_dma; }
	CDROMController* cdrom() { return &_cdrom; }
	MemoryInterface* memInterface() { return &_memInterface; }

	void Init();
	void Tick(double deltaT);

	Playstation();
};