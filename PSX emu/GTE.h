#pragma once
#include "Processor.h"

class GTE : public Processor
{
public:
	GTE(CXD8530BQ* cpu) : Processor(cpu) {}
};

