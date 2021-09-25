#pragma once
#include <cstdint>
#include <algorithm>

namespace Math
{
	bool DetectOverflowAdd(std::uint32_t lhs, std::uint32_t rhs, std::uint32_t result);
	bool DetectOverflowSubtract(std::uint32_t lhs, std::uint32_t rhs, std::uint32_t result);

	template<typename T>
	T ToggleBit(T val, std::uint8_t bitIndex, bool set)
	{
		if (set)
		{
			val = val | (1 << bitIndex);
		}
		else
		{
			val = val & ~(1 << bitIndex);
		}

		return val;
	}

	template<typename T>
	T SetBits(T val, std::uint8_t highBit, std::uint8_t lowBit, T newVal)
	{
		if (highBit < lowBit)
			std::swap(highBit, lowBit);

		T mask = 0x1;
		if (highBit > 0)
		{
			//Create a mask of 1s under the high bit and the low bit, invert the low mask to zero out everything under the low bit. Set the high bit so complete the mask
			mask = ((1 << highBit) - 1);
			if (lowBit > 0)
				mask &= ~((1 << lowBit) - 1);
			mask |= (1 << highBit);
		}

		//Unset relevant bits
		val &= ~mask;
		//Set relevant bits to new val
		return val | (mask & (newVal << lowBit));
	}

	template<typename T>
	T ToggleBits(T val, std::uint8_t highBit, std::uint8_t lowBit, bool set)
	{
		return SetBits(val, highBit, lowBit, set ? T(~0) : T(0));
	}

	template<typename T>
	bool GetBit(T val, std::uint8_t bitIndex)
	{
		return val & (1 << bitIndex);
	}
}