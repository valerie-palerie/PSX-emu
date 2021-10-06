#pragma once
#include <cstdint>
#include <algorithm>

using uint = unsigned int;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;

namespace Math
{
	bool DetectOverflowAdd(uint32 lhs, uint32 rhs, uint32 result);
	bool DetectOverflowSubtract(uint32 lhs, uint32 rhs, uint32 result);

	template<typename T>
	bool GetBit(T val, uint8 bitIndex)
	{
		return val & (1 << bitIndex);
	}

	template<typename T>
	T ToggleBit(T val, uint8 bitIndex, bool set)
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
	T GetBits(T val, uint8 lowBit, uint8 highBit)
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

		//Unset irrelevant bits
		val &= mask;
		return val >> lowBit;
	}

	template<typename T>
	T SetBits(T val, uint8 lowBit, uint8 highBit, T newVal)
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
	T ToggleBits(T val, uint8 lowBit, uint8 highBit, bool set)
	{
		return SetBits(val, highBit, lowBit, set ? T(~0) : T(0));
	}
}