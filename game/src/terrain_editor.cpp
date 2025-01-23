#include "terrain_editor.h"
#include "imgui.h"

using namespace glm;

namespace iol
{
	TerrainEditor::TerrainEditor()
	{
	}

	TerrainEditor::~TerrainEditor()
	{
	}

	void TerrainEditor::Create(GraphicsSystem* g, float size, size_t numQuadsPerSide, float tileX, float tileY)
	{
		//------------------------------------
		// Load Shader
		// Create VertexLayout
		//------------------------------------

		m_shaderMVPTexture = g->CreateShaderFromFile("res/shader/basic_mvp_texture.glsl");
		m_shaderMVPColor = g->CreateShaderFromFile("res/shader/basic_mvp_color.glsl");

		VertexAttributeParam attributesPosUV[] = {
			{ VertexSemantic::Position, VertexType::Float, 3, VertexSlot::PerVertex, 0 },
			{ VertexSemantic::Texcoords, VertexType::Float, 2, VertexSlot::PerVertex, 0 }
		};

		VertexAttributeParam attributesPos[] = {
			{ VertexSemantic::Position, VertexType::Float, 3, VertexSlot::PerVertex, 0 }
		};

		m_vertexLayoutMVPTexture = g->CreateVertexLayout(m_shaderMVPTexture, attributesPosUV, iol_countof(attributesPosUV));
		m_vertexLayoutMVPColor = g->CreateVertexLayout(m_shaderMVPColor, attributesPos, iol_countof(attributesPos));

		//------------------------------------
		// Create PipelineState
		//------------------------------------

		{
			// MVP Texture
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::TriangleList;
			pipelineParam.pShader = m_shaderMVPTexture;
			pipelineParam.pVertexLayout = m_vertexLayoutMVPTexture;
			pipelineParam.blendMode = BlendMode::None;
			m_pipelineStateMVPTexture = g->CreatePipelineState(pipelineParam);
		}

		{
			// MVP Texture Wireframe
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::TriangleList;
			pipelineParam.pShader = m_shaderMVPTexture;
			pipelineParam.pVertexLayout = m_vertexLayoutMVPTexture;
			pipelineParam.blendMode = BlendMode::None;
			pipelineParam.rasterizerFlags &= ~RasterizerFlags_BackFaceCulling;
			pipelineParam.rasterizerFlags |= RasterizerFlags_WireFrame;
			m_pipelineStateMVPTextureWireframe = g->CreatePipelineState(pipelineParam);
		}

		{
			// MVP Color Wireframe
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::TriangleList;
			pipelineParam.pShader = m_shaderMVPColor;
			pipelineParam.pVertexLayout = m_vertexLayoutMVPColor;
			pipelineParam.blendMode = BlendMode::None;
			pipelineParam.rasterizerFlags &= ~RasterizerFlags_BackFaceCulling;
			pipelineParam.rasterizerFlags |= RasterizerFlags_WireFrame;
			m_pipelineStateMVPColorWireframe = g->CreatePipelineState(pipelineParam);
		}

		//------------------------------------
		// Create UniformBuffer
		//------------------------------------

		m_uniformBufferMatrices = g->CreateUniformBuffer(&m_uniformDataMatrices, sizeof(m_uniformDataMatrices), BufferUsage::DynamicDraw, "UB_matrices");
		m_uniformBufferMaterial = g->CreateUniformBuffer(&m_uniformDataMaterial, sizeof(m_uniformDataMaterial), BufferUsage::DynamicDraw, "UB_material");

		//------------------------------------
		// Create Terrain Mesh
		//------------------------------------

		m_mesh.LoadTerrain(size, numQuadsPerSide, tileX, tileY);

		m_vertexCount = m_mesh.GetVertexCount();
		m_verticesPosUV = iol_alloc_array(VertexPosUV, m_vertexCount);

		for (size_t i = 0; i < m_vertexCount; i++)
		{
			m_verticesPosUV[i].pos = m_mesh.positions[i];
			m_verticesPosUV[i].uv = m_mesh.uvs[i];
		}

		m_vertexBufferPosUV = g->CreateVertexBuffer(m_verticesPosUV, sizeof(*m_verticesPosUV) * m_vertexCount, BufferUsage::DynamicDraw);

		m_verticesPos = iol_alloc_array(glm::vec3, m_vertexCount);

		for (size_t i = 0; i < m_vertexCount; i++)
		{
			m_verticesPos[i] = m_mesh.positions[i];
		}

		m_vertexBufferPos = g->CreateVertexBuffer(m_verticesPos, sizeof(*m_verticesPos) * m_vertexCount, BufferUsage::DynamicDraw);

		m_indexBuffer = g->CreateIndexBuffer(m_mesh.indices.pData, m_mesh.GetIndexCount(), BufferUsage::DynamicDraw);

		m_vertexArrayMVPTexture = g->CreateVertexArray(m_vertexLayoutMVPTexture, (const VertexBuffer**)&m_vertexBufferPosUV, 1, m_indexBuffer);
		m_vertexArrayMVPColor = g->CreateVertexArray(m_vertexLayoutMVPColor, (const VertexBuffer**)&m_vertexBufferPos, 1, m_indexBuffer);

		//------------------------------------
		// Load Texture
		//------------------------------------

		TextureParam texParam;
		texParam.format = TextureFormat::RGBA;
		texParam.filter = TextureFilter::LinearMipMapLinear;
		texParam.genMipMaps = true;
		//m_texture = g->CreateTextureFromFile("res/texture/uv_test_texture.png", texParam);
		m_texture = g->CreateTextureFromFile("res/texture/horror_texture_pack/Floor/Horror_Floor_12-512x512.png", texParam);
	}

	void TerrainEditor::Destroy(GraphicsSystem* g)
	{
		g->DestroyPipelineState(m_pipelineStateMVPTexture);
		g->DestroyPipelineState(m_pipelineStateMVPTextureWireframe);
		g->DestroyPipelineState(m_pipelineStateMVPColorWireframe);
		g->DestroyUniformBuffer(m_uniformBufferMatrices);
		g->DestroyUniformBuffer(m_uniformBufferMaterial);
		g->DestroyShader(m_shaderMVPTexture);
		g->DestroyShader(m_shaderMVPColor);
		g->DestroyVertexLayout(m_vertexLayoutMVPTexture);
		g->DestroyVertexLayout(m_vertexLayoutMVPColor);
		g->DestroyVertexBuffer(m_vertexBufferPosUV);
		g->DestroyVertexBuffer(m_vertexBufferPos);
		g->DestroyIndexBuffer(m_indexBuffer);
		g->DestroyVertexArray(m_vertexArrayMVPTexture);
		g->DestroyVertexArray(m_vertexArrayMVPColor);
		g->DestroyTexture(m_texture);

		iol_free(m_verticesPosUV);
		iol_free(m_verticesPos);
	}

	void TerrainEditor::Update(GraphicsSystem* g, const Camera* camera, float deltaTime)
	{
		KeyState leftMouseBtnState = input_GetMouseButtonState(MouseButton_Left);
		vec2 mousePos = input_GetMousePosition();
		float screenWidth = g->GetScreenWidth();
		float screenHeight = g->GetScreenHeight();
		vec2 mouseScrollDelta = input_GetMouseScrollDelta();

		m_editRadius -= mouseScrollDelta.y * deltaTime * m_editRadiusScrollSpeed;
		m_editRadius = Clamp(m_editRadius, m_editRadiusMin, m_editRadiusMax);

		if (m_toolType != TerrainEditToolType_Flatten)
			m_flattenDesiredHeight = FLT_MAX;

		switch (m_editState)
		{
		case TerrainEditState_Initial:
		{
			m_selectedIndices.Clear();

			vec3 rayOrigin;
			vec3 rayDir;
			ScreenPointToRay(camera->transform.position, camera->GetViewProjectionMatrix(), mousePos, screenWidth, screenHeight, rayOrigin, rayDir);

			float distance;
			vec3 hitPoint;
			Array<uint32> hitTriangleIndices;

			if (RayIntersectsMesh(rayOrigin, rayDir, m_mesh, distance, hitPoint, hitTriangleIndices))
			{
				m_selectedIndices.Create(30000);

				if (m_toolType == TerrainEditToolType_DragHeight)
				{
					m_mesh.GetTrianglesInRadius(hitPoint, m_editRadius, m_selectedIndices);

					if (leftMouseBtnState == KeyState_Pressed)
					{
						m_editState = TerrainEditState_DragHeight;
						m_startMousePos = mousePos;
						m_startHitPoint = hitPoint;

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
				else if (m_toolType == TerrainEditToolType_Flatten)
				{
					m_mesh.GetTrianglesInRadiusIgnoreHeight(hitPoint, m_editRadius, m_selectedIndices);

					if (leftMouseBtnState == KeyState_Pressed || leftMouseBtnState == KeyState_Holding)
					{
						if (m_flattenDesiredHeight == FLT_MAX)
							m_flattenDesiredHeight = hitPoint.y;

						for (size_t i = 0; i < m_selectedIndices.count; i++)
						{
							uint32 vertexIndex = m_selectedIndices[i];

							m_verticesPos[vertexIndex].y = m_flattenDesiredHeight;
							m_verticesPosUV[vertexIndex].pos.y = m_flattenDesiredHeight;
							m_mesh.positions[vertexIndex].y = m_flattenDesiredHeight;
						}
					}
					else
					{
						m_flattenDesiredHeight = FLT_MAX;
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

					float percent = m_selectedOriginalDistances[i] / m_editRadius;
					percent = Clamp(1.0f - percent, 0.0f, 0.9f);
					float finalHeightDiff = heightDiff * percent;

					m_verticesPos[vertexIndex]
						= m_verticesPosUV[vertexIndex].pos
						= m_mesh.positions[vertexIndex]
						= m_selectedOriginalPositions[i] + vec3(0.0f, finalHeightDiff, 0.0f);
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

		if (input_GetKeyState(IOL_SCANCODE_TAB) == KeyState_Pressed)
		{
			if (m_pipelineStateType == TerrainPipelineStateType_MVPTexture)
				m_pipelineStateType = TerrainPipelineStateType_MVPTextureWireframe;
			else
				m_pipelineStateType = TerrainPipelineStateType_MVPTexture;
		}
	}

	void TerrainEditor::Render(GraphicsSystem* g, const mat4& viewProjection)
	{
		//------------------------------------
		// Draw Terrain Mesh
		//------------------------------------

		switch (m_pipelineStateType)
		{
		case TerrainPipelineStateType_MVPTexture:
			g->SetPipelineState(m_pipelineStateMVPTexture);
			break;
		case TerrainPipelineStateType_MVPTextureWireframe:
			g->SetPipelineState(m_pipelineStateMVPTextureWireframe);
			break;
		default:
			iol_assert(false);
			break;
		}

		m_uniformDataMatrices.mvp = viewProjection;
		g->SetUniformBufferData(m_uniformBufferMatrices, &m_uniformDataMatrices, sizeof(m_uniformDataMatrices));

		g->SetVertexBufferData(m_vertexBufferPosUV, m_verticesPosUV, sizeof(*m_verticesPosUV) * m_vertexCount);
		g->SetVertexBufferData(m_vertexBufferPos, m_verticesPos, sizeof(*m_verticesPos) * m_vertexCount);

		g->SetIndexBufferData(m_indexBuffer, m_mesh.indices.pData, m_mesh.indices.count);

		g->BindVertexArray(m_vertexArrayMVPTexture);
		const UniformBuffer* ubsMVPTexture[] = { m_uniformBufferMatrices };
		g->BindUniformBuffer(ubsMVPTexture, iol_countof(ubsMVPTexture));
		g->BindTexture(0, (const Texture**)&m_texture, 1);

		g->DrawIndexed(g->GetIndexBufferNumIndices(m_indexBuffer));

		//------------------------------------
		// Draw Terrain Selection
		//------------------------------------

		if (m_selectedIndices.count > 0)
		{
			g->Clear(vec4(0.0f, 0.0f, 0.0f, 1.0f), ClearFlags_Depth);

			g->SetPipelineState(m_pipelineStateMVPColorWireframe);
			g->SetIndexBufferData(m_indexBuffer, m_selectedIndices.pData, m_selectedIndices.count);

			m_uniformDataMaterial.color = glm::vec4(0.f, 1.f, 0.f, 1.f);
			g->SetUniformBufferData(m_uniformBufferMaterial, &m_uniformDataMaterial, sizeof(m_uniformDataMaterial));

			g->BindVertexArray(m_vertexArrayMVPColor);
			const UniformBuffer* ubsMVPColor[] = { m_uniformBufferMatrices, m_uniformBufferMaterial };
			g->BindUniformBuffer(ubsMVPColor, iol_countof(ubsMVPColor));
			g->BindTexture(0, (const Texture**)&m_texture, 1);

			g->DrawIndexed(m_selectedIndices.count);
		}
	}

	void TerrainEditor::RenderGUI(GraphicsSystem* g)
	{
		ImGui::SliderFloat("Terrain Edit Radius", &m_editRadius, m_editRadiusMin, m_editRadiusMax);

		static const char* s_terrainEditToolTypeNames[] = {
			"DragHeight",
			"Flatten"
		};

		const char* terrainToolName = s_terrainEditToolTypeNames[m_toolType];
		ImGui::SliderInt("Terrain Tool", (int*)&m_toolType, 0, 1, terrainToolName);
	}
}