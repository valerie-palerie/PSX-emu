#include "MathUtils.h"

//Was not confident in the previous add/sub implementation so i took these from duckstation, thank you duckstation
//https://github.com/stenzek/duckstation/blob/master/src/core/cpu_core.cpp
bool Math::DetectOverflowAdd(std::uint32_t lhs, std::uint32_t rhs, std::uint32_t result)
{
	return (((result ^ lhs) & (result ^ rhs)) & 0x80000000) != 0;
}

bool Math::DetectOverflowSubtract(std::uint32_t lhs, std::uint32_t rhs, std::uint32_t result)
{
	return (((result ^ lhs) & (lhs ^ rhs)) & 0x80000000) != 0;
}
