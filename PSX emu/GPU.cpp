#include "GPU.h"
#include "MemoryTypes.h"

void GPU::Init()
{
}

void GPU::Tick(double deltaT)
{
}

GPU::GPU(Playstation* playstation)
    : PlaystationComponent(playstation)
    , MemoryChip(MemoryMap::GPU_SIZE)
{
}
