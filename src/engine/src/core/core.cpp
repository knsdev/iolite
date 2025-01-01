#include "iol_core.h"
#include "iol_debug.h"
#include "iol_mesh.h"
#include <chrono>
#include <float.h>

using namespace glm;

namespace iol
{
	size_t Align(size_t value, size_t alignment)
	{
		iol_assert(IsPowerOfTwo(alignment));

		return (value + alignment - 1) & ~(alignment - 1);
	}

	bool IsPowerOfTwo(size_t value)
	{
		return (value & (value - 1)) == 0;
	}

	double GetSeconds()
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto duration = now.time_since_epoch();
		long long nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
		double sec = nanosec * 1.0e-9;

		return sec;
	}

	vec3 CreateDirection(float pitch, float yaw)
	{
		vec3 direction;
		direction.x = cos(radians(yaw)) * cos(radians(pitch));
		direction.y = sin(radians(pitch));
		direction.z = sin(radians(yaw)) * cos(radians(pitch));
		return normalize(direction);
	}

	bool RayIntersectsTriangle(vec3 rayOrigin, vec3 rayDir, vec3 v0, vec3 v1, vec3 v2, float& t, vec3& hitPoint)
	{
		const float epsilon = 1e-8;

		vec3 edge1 = v1 - v0;
		vec3 edge2 = v2 - v0;

		// Compute determinant
		vec3 h = cross(rayDir, edge2);
		float a = dot(edge1, h);

		if (a > -epsilon && a < epsilon)
			return false; // Ray is parallel to triangle

		float f = 1.0 / a;
		vec3 s = rayOrigin - v0;
		float u = f * dot(s, h);

		if (u < 0.0 || u > 1.0)
			return false;

		vec3 q = cross(s, edge1);
		float v = f * dot(rayDir, q);

		if (v < 0.0 || u + v > 1.0)
			return false;

		// Compute t (distance along the ray)
		t = f * dot(edge2, q);

		if (t > epsilon)
		{
			// Intersection point
			hitPoint = rayOrigin + rayDir * t;
			return true;
		}

		return false; // No hit
	}

	bool RayIntersectsMesh(vec3 rayOrigin, vec3 rayDir, const Mesh& mesh, float& t, vec3& hitPoint, Array<uint32>& hitTriangleIndices)
	{
		vec3 closestHit;
		float closestHitDistance = FLT_MAX;
		hitTriangleIndices.Create(3);

		for (uint32 i = 0; i < mesh.indices.count; i += 3)
		{
			vec3 v0 = mesh.positions[mesh.indices[i]];
			vec3 v1 = mesh.positions[mesh.indices[i + 1]];
			vec3 v2 = mesh.positions[mesh.indices[i + 2]];

			float distance;
			vec3 hit;
			bool result = RayIntersectsTriangle(rayOrigin, rayDir, v0, v1, v2, distance, hit);

			if (result && distance < closestHitDistance)
			{
				closestHitDistance = distance;
				closestHit = hit;
				hitTriangleIndices.Clear();
				hitTriangleIndices.PushBack(mesh.indices[i]);
				hitTriangleIndices.PushBack(mesh.indices[i + 1]);
				hitTriangleIndices.PushBack(mesh.indices[i + 2]);
			}
		}

		if (closestHitDistance != FLT_MAX)
		{
			hitPoint = closestHit;
			t = closestHitDistance;
			return true;
		}

		return false;
	}

	void GetRayFromScreenPoint(glm::vec3 cameraPos, const glm::mat4x4& cameraViewProj, glm::vec2 screenPoint, float screenWidth, float screenHeight, glm::vec3& rayOrigin, glm::vec3& rayDir)
	{
		float xNDC = (2.0f * screenPoint.x) / screenWidth - 1.0f;
		float yNDC = 1.0f - (2.0f * screenPoint.y) / screenHeight;

		vec4 ndc = vec4(xNDC, yNDC, -1.0f, 1.0f); // Near plane, clip space

		// Convert to world space
		mat4 invViewProj = inverse(cameraViewProj);
		vec4 worldPos = invViewProj * ndc;
		worldPos /= worldPos.w;

		rayOrigin = cameraPos;
		rayDir = normalize(vec3(worldPos) - rayOrigin);
	}
}
