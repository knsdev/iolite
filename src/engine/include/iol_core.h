#ifndef IOLITE_CORE_H
#define IOLITE_CORE_H

#include "iol_definitions.h"

namespace iol
{
	IOL_API size_t				Align(size_t value, size_t alignment);
	IOL_API bool				IsPowerOfTwo(size_t value);
	template<typename T> T		Min(T a, T b);
	template<typename T> T		Max(T a, T b);
	template<typename T> T		Clamp(T value, T min, T max);
	template<typename T> T		Remap(T value, T min, T max, T newMin, T newMax);
	template<typename T> void	Swap(T* pElement0, T* pElement1);

	IOL_API double				GetSeconds();

	IOL_API glm::vec3			CreateDirection(float pitch, float yaw);
}

#include "internal/core_inl.h"

#endif // IOLITE_CORE_H