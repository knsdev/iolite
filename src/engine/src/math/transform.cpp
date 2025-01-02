#include "iol_transform.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtx/quaternion.hpp"

namespace iol
{
	glm::vec3 Transform::GetForward() const
	{
		return rotation * world_forward;
	}

	glm::vec3 Transform::GetRight() const
	{
		return rotation * world_right;
	}

	glm::vec3 Transform::GetUp() const
	{
		return rotation * world_up;
	}

	glm::mat4 Transform::GetMatrix() const
	{
		glm::mat4 worldMatrix = glm::translate(position) * glm::toMat4(rotation) * glm::scale(scale);
		return worldMatrix;
	}

	glm::mat4 Transform::GetMatrixInverse() const
	{
		return glm::inverse(GetMatrix());
	}

	void Transform::SetForward(const glm::vec3& forward)
	{
		LookAt(position + normalize(forward));
	}

	void Transform::LookAt(const glm::vec3& pos)
	{
		glm::mat4 lookMat = glm::lookAt(position, pos, world_up);
		rotation = conjugate(quat_cast(lookMat));
	}

	void Transform::RotateAxisWorld(const glm::vec3& axis, float angle)
	{
		glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), axis);
		rotation = deltaRotation * rotation;
	}

	void Transform::RotateAxisLocal(const glm::vec3& axis, float angle)
	{
		glm::quat deltaRotation = glm::angleAxis(glm::radians(angle), axis);
		rotation = rotation * deltaRotation;
	}
}
