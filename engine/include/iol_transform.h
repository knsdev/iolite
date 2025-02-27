#ifndef IOL_TRANSFORM_H
#define IOL_TRANSFORM_H

#include "iol_definitions.h"
#include "glm/glm.hpp"
#include "glm/ext/quaternion_float.hpp"

namespace iol
{
	static constexpr glm::vec3 world_right = glm::vec3(1.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	static constexpr glm::vec3 world_forward = glm::vec3(0.0f, 0.0f, -1.0f);

	class IOL_API Transform
	{
	public:
		glm::vec3  position;
		glm::quat  rotation;
		glm::vec3  scale;

		glm::vec3  GetForward() const;
		glm::vec3  GetRight() const;
		glm::vec3  GetUp() const;
		glm::mat4  GetMatrix() const;
		glm::mat4  GetMatrixInverse() const;

		void       SetForward(const glm::vec3& forward);
		void       LookAt(const glm::vec3& position);
		void       RotateAxisWorld(const glm::vec3& axis, float angle);
		void       RotateAxisLocal(const glm::vec3& axis, float angle);
	};
}

#endif // IOL_TRANSFORM_H
