#include "application.h"

using namespace glm;

namespace iol
{
	void GameApplication::Create(const EngineParams& params)
	{
		GraphicsSystem* g = m_graphicsSystem;

		//------------------------------------
		// Create Camera
		//------------------------------------

		CameraProp prop;
		prop.projectionType = ProjectionType::Perspective;
		prop.aspectRatio = 16.0f / 9.0f;
		prop.orthographicSize = 1.0f;
		prop.fieldOfViewDegrees = 60.0f;
		prop.nearPlane = 0.01f;
		prop.farPlane = 1000.0f;

		vec3 cameraStartPos = vec3(0.0f, 3.0f, 0.0f);
		vec3 cameraLookDir = vec3(1.0f, -0.5f, -1.0f);

		m_camera = new Camera(prop);
		m_cameraFlying = new CameraFlying(&m_camera->transform, cameraStartPos, cameraLookDir);
		
		CameraFlyingProp camFlyProp;
		camFlyProp.mouseButtonToRotateCamera = MouseButton_Middle;
		m_cameraFlying->SetProp(camFlyProp);

		//------------------------------------
		// Load Shader
		// Create VertexLayout
		//------------------------------------

		m_shader = g->CreateShaderFromFile("res/shader/basic_mvp_texture.glsl");

		VertexAttributeParam attributes[] = {
			{ VertexSemantic::Position, VertexType::Float, 3, VertexSlot::PerVertex, 0 },
			{ VertexSemantic::Texcoords, VertexType::Float, 2, VertexSlot::PerVertex, 0 }
		};

		m_vertexLayout = g->CreateVertexLayout(m_shader, attributes, iol_countof(attributes));

		//------------------------------------
		// Create PipelineState
		//------------------------------------

		m_pipelineStates.Create(10);

		{
			// Default
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::TriangleList;
			pipelineParam.pShader = m_shader;
			pipelineParam.pVertexLayout = m_vertexLayout;
			pipelineParam.blendMode = BlendMode::None;
			m_pipelineStates.PushBack(g->CreatePipelineState(pipelineParam));
		}

		{
			// Wireframe
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::TriangleList;
			pipelineParam.pShader = m_shader;
			pipelineParam.pVertexLayout = m_vertexLayout;
			pipelineParam.blendMode = BlendMode::None;
			pipelineParam.rasterizerFlags &= ~RasterizerFlags_BackFaceCulling;
			pipelineParam.rasterizerFlags |= RasterizerFlags_WireFrame;
			m_pipelineStates.PushBack(g->CreatePipelineState(pipelineParam));
		}

		m_currentPipelineStateIndex = 0;

		//------------------------------------
		// Create UniformBuffer
		//------------------------------------

		m_uniformBuffer = g->CreateUniformBuffer(&m_uniformData, sizeof(m_uniformData), BufferUsage::DynamicDraw, "UB_matrices");

		//------------------------------------
		// Load Mesh
		//------------------------------------

		//m_mesh.LoadCube();
		//m_mesh.LoadObjFile("res/model/sphere_low_poly.obj");
		//m_mesh.LoadTerrain(glm::vec3(0.0f, 0.0f, 0.0f), 4, 4, 1.0f, 0.0f, 5.0f);
		//m_mesh.LoadQuad();
		m_mesh.LoadTerrain(40.0f, 80, 10.0f, 10.0f, 0.0f, 0.0f, 0.2f);

		m_vertexCount = m_mesh.GetVertexCount();
		m_vertices = iol_alloc_array(VertexPosUV, m_vertexCount);

		for (size_t i = 0; i < m_vertexCount; i++)
		{
			m_vertices[i].pos = m_mesh.positions[i];
			m_vertices[i].uv = m_mesh.uvs[i];
		}

		m_vertexBuffer = g->CreateVertexBuffer(m_vertices, sizeof(*m_vertices) * m_vertexCount, BufferUsage::DynamicDraw);
		m_indexBuffer = g->CreateIndexBuffer(m_mesh.indices.pData, m_mesh.GetIndexCount(), BufferUsage::DynamicDraw);

		m_vertexArray = g->CreateVertexArray(m_vertexLayout, (const VertexBuffer**)&m_vertexBuffer, 1, m_indexBuffer);

		//------------------------------------
		// Load Texture
		//------------------------------------

		TextureParam texParam;
		texParam.format = TextureFormat::RGBA;
		texParam.filter = TextureFilter::LinearMipMapLinear;
		texParam.genMipMaps = true;
		m_texture = g->CreateTextureFromFile("res/texture/uv_test_texture.png", texParam);
	}

	void GameApplication::Destroy()
	{
		delete m_cameraFlying;
		delete m_camera;

		GraphicsSystem* g = m_graphicsSystem;
		
		for (size_t i = 0; i < m_pipelineStates.count; i++)
			g->DestroyPipelineState(m_pipelineStates[i]);

		g->DestroyUniformBuffer(m_uniformBuffer);
		g->DestroyShader(m_shader);
		g->DestroyVertexLayout(m_vertexLayout);
		g->DestroyVertexBuffer(m_vertexBuffer);
		g->DestroyIndexBuffer(m_indexBuffer);
		g->DestroyVertexArray(m_vertexArray);
		g->DestroyTexture(m_texture);

		iol_free(m_vertices);
	}

	void GameApplication::Update(float deltaTime)
	{
		GraphicsSystem* g = m_graphicsSystem;

		CameraProp camProp = m_camera->GetProp();

		if (input_GetKeyState(IOL_SCANCODE_E) == KeyState_Holding)
		{
			camProp.fieldOfViewDegrees += 10.0f * deltaTime;
			camProp.orthographicSize += 1.0f * deltaTime;
		}
		else if (input_GetKeyState(IOL_SCANCODE_F) == KeyState_Holding)
		{
			camProp.fieldOfViewDegrees -= 10.0f * deltaTime;
			camProp.orthographicSize -= 1.0f * deltaTime;
		}

		m_camera->SetProp(camProp);
		m_cameraFlying->Update(deltaTime);

		const float radius = 5.0f;

		KeyState leftMouseBtnState = input_GetMouseButtonState(MouseButton_Left);
		vec2 mousePos = input_GetMousePosition();
		float screenWidth = g->GetScreenWidth();
		float screenHeight = g->GetScreenHeight();

		switch (m_editState)
		{
		case TerrainEditState_Initial:
			if (leftMouseBtnState == KeyState_Pressed)
			{
				m_startMousePos = mousePos;

				vec3 rayOrigin;
				vec3 rayDir;
				GetRayFromScreenPoint(m_camera->transform.position, m_camera->GetViewProjectionMatrix(), mousePos, screenWidth, screenHeight, rayOrigin, rayDir);

				float distance;
				vec3 hitPoint;
				Array<uint32> hitTriangleIndices;

				if (RayIntersectsMesh(rayOrigin, rayDir, m_mesh, distance, hitPoint, hitTriangleIndices))
				{
					m_startHitPoint = hitPoint;
					m_editState = TerrainEditState_DragHeight;

					if (m_mesh.GetTrianglesInRadius(hitPoint, radius, m_selectedIndices))
					{
						m_selectedOriginalPositions.Create(m_selectedIndices.count);
						m_selectedOriginalDistances.Create(m_selectedIndices.count);

						for (size_t i = 0; i < m_selectedIndices.count; i++)
						{
							uint32 vertexIndex = m_selectedIndices[i];
							vec3 origPos = m_mesh.positions[vertexIndex];
							float origDistance = glm::length(hitPoint - origPos);

							m_selectedOriginalPositions.PushBack(origPos);
							m_selectedOriginalDistances.PushBack(origDistance);
						}
					}
				}
			}
			break;

		case TerrainEditState_DragHeight:
			if (leftMouseBtnState == KeyState_Holding)
			{
				vec2 mouseDiff = mousePos - m_startMousePos;
				float heightDiff = mouseDiff.y * -0.1f;

				for (size_t i = 0; i < m_selectedIndices.count; i++)
				{
					uint32 vertexIndex = m_selectedIndices[i];

					float percent = m_selectedOriginalDistances[i] / radius;
					percent = Clamp(1.0f - percent, 0.0f, 0.9f);
					float finalHeightDiff = heightDiff * percent;

					m_vertices[vertexIndex].pos = m_mesh.positions[vertexIndex] = m_selectedOriginalPositions[i] + vec3(0.0f, finalHeightDiff, 0.0f);
				}
			}
			else
			{
				m_editState = TerrainEditState_Initial;
			}
			break;

		default:
			break;
		}

		Render(deltaTime);
	}

	void GameApplication::Render(float deltaTime)
	{
		GraphicsSystem* g = m_graphicsSystem;

		if (input_GetKeyState(IOL_SCANCODE_TAB) == KeyState_Pressed)
		{
			m_currentPipelineStateIndex = (m_currentPipelineStateIndex + 1) % m_pipelineStates.count;
		}

		mat4 viewProj = m_camera->GetViewProjectionMatrix();
		m_uniformData.mvp = viewProj;
		g->SetUniformBufferData(m_uniformBuffer, &m_uniformData, sizeof(m_uniformData));

		g->SetVertexBufferData(m_vertexBuffer, m_vertices, sizeof(VertexPosUV) * m_vertexCount);

		g->BeginRender(m_pipelineStates[m_currentPipelineStateIndex]);
		g->SetViewportFullscreen();
		g->Clear(vec4(0.0f, 0.0f, 0.0f, 1.0f), ClearFlags_All);

		const UniformBuffer* ubs[] = { m_uniformBuffer };
		g->BindUniformBuffer(ubs, iol_countof(ubs));
		g->BindVertexArray(m_vertexArray);
		g->BindTexture(0, (const Texture**)&m_texture, 1);

		g->DrawIndexed(g->GetIndexBufferNumIndices(m_indexBuffer));

		// Draw second object
		/*vec3 objectPos = vec3(6.0f, 0.0f, 0.0f);
		m_uniformData.mvp = viewProj * glm::translate(mat4(1), objectPos);
		g->SetUniformBufferData(m_uniformBuffer, &m_uniformData, sizeof(m_uniformData));
		g->DrawIndexed(g->GetIndexBufferNumIndices(m_indexBuffer));*/

		g->EndRender();
	}

	void GameApplication::FixedUpdate(float deltaTime)
	{
	}
}