#include "iol_camera.h"
#include "iol_core.h"
#include "iol_memory.h"
#include "iol_input.h"
#include "glm/ext/matrix_clip_space.hpp"

using namespace glm;

namespace iol
{
	Camera::Camera()
	{
	}

	Camera::Camera(const CameraProp& prop)
	{
		this->prop = prop;
	}

	Camera::~Camera()
	{
	}

	glm::mat4 Camera::GetViewMatrix() const
	{
		return transform.GetMatrixInverse();
	}

	glm::mat4 Camera::GetProjectionMatrix() const
	{
		if (prop.projectionType == ProjectionType::Perspective)
		{
			return glm::perspective(glm::radians(prop.fieldOfViewDegrees), prop.aspectRatio, prop.nearPlane, prop.farPlane);
		}
		else
		{
			float sizeY = prop.orthographicSize;
			float sizeX = sizeY * prop.aspectRatio;

			return glm::ortho(-sizeX, sizeX, -sizeY, sizeY, prop.nearPlane, prop.farPlane);
		}
	}

	glm::mat4 Camera::GetViewProjectionMatrix() const
	{
		glm::mat4 view = GetViewMatrix();
		glm::mat4 proj = GetProjectionMatrix();

		return proj * view;
	}
}