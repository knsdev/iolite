#include "iol_camera.h"
#include "iol_input.h"
#include "iol_core.h"
#include "iol_memory.h"

using namespace glm;

namespace iol
{
	CameraFlying::CameraFlying(Transform* transform, const vec3 position, const vec3 forward)
	{
		this->mouseLastPos = { 0, 0 };
		this->mouseFirstInput = true;

		transform->position = position;
		transform->scale = vec3(1.0f);
		transform->SetForward(forward);
		this->transform = transform;
	}

	CameraFlying::~CameraFlying()
	{
	}

	void CameraFlying::Update(float deltaTime)
	{
		ivec2 mousePos = input::GetMousePosition();

		if (this->mouseFirstInput)
		{
			this->mouseFirstInput = false;
			this->mouseLastPos = mousePos;
		}

		ivec2 mouseDeltaPos = mousePos - this->mouseLastPos;
		this->mouseLastPos = mousePos;

		if (input::IsMouseButtonDown(prop.mouseButtonToRotateCamera))
		{
			transform->RotateAxisWorld(world_up, -mouseDeltaPos.x * this->prop.sensitivityHorizontal);
			transform->RotateAxisLocal(world_right, -mouseDeltaPos.y * this->prop.sensitivityVertical);
		}

		vec3 forward = transform->GetForward();
		vec3 right = transform->GetRight();
		vec3 position = transform->position;
		float step = this->prop.speed * deltaTime;

		if (input::IsKeyDown(IOL_SCANCODE_SPACE))
			position.y += step;
		if (input::IsKeyDown(IOL_SCANCODE_LSHIFT))
			position.y -= step;

		if (input::IsKeyDown(IOL_SCANCODE_W))
			position += forward * step;
		if (input::IsKeyDown(IOL_SCANCODE_S))
			position -= forward * step;

		if (input::IsKeyDown(IOL_SCANCODE_A))
		{
			position -= right * step;
		}
		if (input::IsKeyDown(IOL_SCANCODE_D))
		{
			position += right * step;
		}

		transform->position = position;
	}

	Transform* CameraFlying::GetTransform() const
	{
		return transform;
	}
}