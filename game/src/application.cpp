#include "application.h"
#include "imgui.h"

using namespace glm;

namespace iol
{
	void GameApplication::Create(const EngineParams& params)
	{
		GraphicsSystem* g = m_graphicsSystem;

		vec3 cameraStartPos = vec3(0.0f, 3.0f, 0.0f);
		vec3 cameraLookDir = vec3(1.0f, -0.5f, -1.0f);

		m_camera = iol_new(Camera);

		m_cameraFlying = iol_new(CameraFlying, &m_camera->transform, cameraStartPos, cameraLookDir);
		m_cameraFlying->prop.mouseButtonToRotateCamera = MouseButton_Right;
		m_cameraFlying->prop.speed = 10.0f;

		m_terrainEditor.Create(m_graphicsSystem, 40.0f, 80, 10.0f, 10.0f);

		m_pipelineStateDefault = m_terrainEditor.GetPipelineStateMVPTexture();
	}

	void GameApplication::Destroy()
	{
		m_terrainEditor.Destroy(m_graphicsSystem);
		iol_free(m_cameraFlying);
		iol_free(m_camera);
	}

	void GameApplication::FixedUpdate(float deltaTime)
	{
	}

	void GameApplication::Update(float deltaTime)
	{
		GraphicsSystem* g = m_graphicsSystem;

		CameraProp& camProp = m_camera->prop;

		if (input::GetKeyState(IOL_SCANCODE_E) == KeyState_Holding)
		{
			camProp.fieldOfViewDegrees += 10.0f * deltaTime;
			camProp.orthographicSize += 1.0f * deltaTime;
		}
		else if (input::GetKeyState(IOL_SCANCODE_F) == KeyState_Holding)
		{
			camProp.fieldOfViewDegrees -= 10.0f * deltaTime;
			camProp.orthographicSize -= 1.0f * deltaTime;
		}

		m_cameraFlying->Update(deltaTime);

		m_terrainEditor.Update(g, m_camera, deltaTime);

		Render(deltaTime);
	}

	void GameApplication::Render(float deltaTime)
	{
		GraphicsSystem* g = m_graphicsSystem;

		//------------------------------------
		// Scene Rendering
		//------------------------------------

		g->BeginRender(m_pipelineStateDefault);
		g->SetViewportFullscreen();
		g->Clear(vec4(0.0f, 0.0f, 0.0f, 1.0f), ClearFlags_All);

		m_terrainEditor.Render(g, m_camera->GetViewProjectionMatrix());

		//------------------------------------
		// GUI Rendering
		//------------------------------------

		ImGui::Begin("Settings");
		ImGui::PushItemWidth(200.0f);

		ImGui::SliderFloat("Camera Field of View", &m_camera->prop.fieldOfViewDegrees, 10.0f, 90.0f);
		ImGui::SliderFloat("Camera Speed", &m_cameraFlying->prop.speed, 1.0f, 30.0f);
		m_terrainEditor.RenderGUI(g);

		ImGui::PopItemWidth();
		ImGui::End();

		g->EndRender();
	}
}