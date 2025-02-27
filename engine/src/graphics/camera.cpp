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

	mat4 Camera::GetViewMatrix() const
	{
		return transform.GetMatrixInverse();
	}

	mat4 Camera::GetProjectionMatrix() const
	{
		if (prop.projectionType == ProjectionType::Perspective)
		{
			return perspective(radians(prop.fieldOfViewDegrees), prop.aspectRatio, prop.nearPlane, prop.farPlane);
		}
		else
		{
			float sizeY = prop.orthographicSize;
			float sizeX = sizeY * prop.aspectRatio;

			return ortho(-sizeX, sizeX, -sizeY, sizeY, prop.nearPlane, prop.farPlane);
		}
	}

	mat4 Camera::GetViewProjectionMatrix() const
	{
		mat4 view = GetViewMatrix();
		mat4 proj = GetProjectionMatrix();

		return proj * view;
	}
}