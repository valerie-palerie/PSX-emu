#pragma once
#include <cstdint>

namespace Math
{
	bool DetectOverflowAdd(std::uint32_t lhs, std::uint32_t rhs, std::uint32_t result);
	bool DetectOverflowSubtract(std::uint32_t lhs, std::uint32_t rhs, std::uint32_t result);
}