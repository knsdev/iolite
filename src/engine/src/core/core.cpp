#include "iol_core.h"
#include "iol_debug.h"
#include <chrono>

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

	glm::vec3 CreateDirection(float pitch, float yaw)
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		return glm::normalize(direction);
	}
}
