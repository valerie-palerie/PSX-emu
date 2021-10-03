#include "MathUtils.h"

//Was not confident in the previous add/sub implementation so i took these from duckstation, thank you duckstation
//https://github.com/stenzek/duckstation/blob/master/src/core/cpu_core.cpp
bool Math::DetectOverflowAdd(uint32 lhs, uint32 rhs, uint32 result)
{
	return (((result ^ lhs) & (result ^ rhs)) & 0x80000000) != 0;
}

bool Math::DetectOverflowSubtract(uint32 lhs, uint32 rhs, uint32 result)
{
	return (((result ^ lhs) & (lhs ^ rhs)) & 0x80000000) != 0;
}
