#ifndef IOLITE_CORE_H
#define IOLITE_CORE_H

#include "iol_definitions.h"

namespace iol
{
	template<typename T>
	struct Array;

	size_t Align(size_t value, size_t alignment);
	bool IsPowerOfTwo(size_t value);

	template<typename T> T Min(T a, T b);
	template<typename T> T Max(T a, T b);
	template<typename T> T Clamp(T value, T min, T max);
	template<typename T> T Remap(T value, T min, T max, T newMin, T newMax);
	template<typename T> void Swap(T* pElement0, T* pElement1);

	double GetSeconds();

	glm::vec3 CreateDirection(float pitch, float yaw);

	bool RayIntersectsTriangle(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, float& t, glm::vec3& hitPoint);
	bool RayIntersectsMesh(glm::vec3 rayOrigin, glm::vec3 rayDir, const class Mesh& mesh, float& t, glm::vec3& hitPoint, Array<uint32>& hitTriangleIndices);
	void GetRayFromScreenPoint(glm::vec3 cameraPos, const glm::mat4x4& cameraViewProj, glm::vec2 screenPoint, float screenWidth, float screenHeight, glm::vec3& rayOrigin, glm::vec3& rayDir);
}

#include "internal/core_inl.h"

#endif // IOLITE_CORE_H