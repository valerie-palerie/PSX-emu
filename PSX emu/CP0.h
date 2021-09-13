#pragma once
#include "Processor.h"

class CP0 : public Processor
{
public:
	CP0(CXD8530BQ* cpu) : Processor(cpu) {}
};

