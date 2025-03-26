#ifndef IOLITE_CORE_INL_H
#define IOLITE_CORE_INL_H

namespace iol
{
	template<typename T>
	T core::Min(T a, T b)
	{
		return (a < b) ? a : b;
	}

	template<typename T>
	T core::Max(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template<typename T>
	T core::Clamp(T value, T min, T max)
	{
		return (value < min) ? min : (value > max) ? max : value;
	}

	template<typename T>
	T core::Remap(T value, T min, T max, T newMin, T newMax)
	{
		return newMin + (value - min) * (newMax - newMin) / (max - min);
	}

	template<typename T>
	void core::Swap(T* pElement0, T* pElement1)
	{
		T temp = *pElement0;
		*pElement0 = *pElement1;
		*pElement1 = temp;
	}
}

#endif