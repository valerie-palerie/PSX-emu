#include "DMAController.h"
#include "MemoryTypes.h"
#include "Playstation.h"

void DMAController::Init()
{

}

void DMAController::Tick(double deltaT)
{
}

DMAController::DMAController(Playstation* playstation)
    : PlaystationComponent(playstation)
    , MemoryChip(MemoryMap::DMA_SIZE)
    , _dram(playstation->dram())
{
}
