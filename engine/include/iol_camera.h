#ifndef IOL_CAMERA_H
#define IOL_CAMERA_H

#include "iol_definitions.h"
#include "iol_transform.h"
#include "iol_input_definitions.h"

namespace iol
{
	class Transform;

	enum class ProjectionType
	{
		Perspective,
		Orthographic
	};

	struct CameraProp
	{
		ProjectionType projectionType = ProjectionType::Perspective;
		float fieldOfViewDegrees = 65.0f;
		float aspectRatio = 16.0f / 9.0f;
		float nearPlane = 0.01f;
		float farPlane = 1000.0f;
		float orthographicSize = 1.0f;
	};

	class Camera
	{
	public:
		Camera();
		Camera(const CameraProp& prop);
		~Camera();

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix() const;
		glm::mat4 GetViewProjectionMatrix() const;

		Transform transform;
		CameraProp prop;
	};

	struct CameraFlyingProp
	{
		float speed = 3.0f;
		float sensitivityHorizontal = 0.2f;
		float sensitivityVertical = 0.2f;
		MouseButton mouseButtonToRotateCamera = MouseButton_Right;
	};

	class CameraFlying
	{
	public:
		CameraFlying(Transform* transform, glm::vec3 position, glm::vec3 forward);
		~CameraFlying();

		void        Update(float deltaTime);
		Transform*  GetTransform() const;

		CameraFlyingProp prop;

	private:
		Transform* transform;
		glm::ivec2 mouseLastPos;
		bool mouseFirstInput;
	};
}

#endif // IOL_CAMERA_H