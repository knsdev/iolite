#include "application.h"

using namespace glm;

#define VERTEX_DATA_TYPE_POS	1
#define VERTEX_DATA_TYPE_UV		1

namespace iol
{
	void GameApplication::Create(const EngineParams& params)
	{
		GraphicsSystem& g = *m_graphicsSystem;

		//**********************************
		// Create Camera
		//**********************************

		CameraProp prop;
		prop.projectionType = ProjectionType::Perspective;
		prop.aspectRatio = 16.0f / 9.0f;
		prop.orthographicSize = 1.0f;
		prop.fieldOfViewDegrees = 60.0f;
		prop.nearPlane = 0.01f;
		prop.farPlane = 1000.0f;

		vec3 cameraStartPos = vec3(0.0f, 0.0f, 3.0f);
		vec3 cameraLookDir = vec3(0.0f, 0.0f, -1.0f);

		m_camera = new Camera(prop);
		m_cameraFlying = new CameraFlying(&m_camera->transform, cameraStartPos, cameraLookDir);

		//**********************************
		// Load Shader
		// Create VertexLayout
		//**********************************

#if VERTEX_DATA_TYPE_POS != 0 && VERTEX_DATA_TYPE_UV != 0
		m_shader = g.CreateShaderFromFile("res/shader/basic_mvp_texture.glsl");
#elif VERTEX_DATA_TYPE_POS != 0
		m_shader = g.CreateShaderFromFile("res/shader/basic_mvp.glsl");
#endif

		VertexAttributeParam attributes[] = {
#if VERTEX_DATA_TYPE_POS != 0
			{ VertexSemantic::Position, VertexType::Float, 3, VertexSlot::PerVertex, 0 },
#endif
#if VERTEX_DATA_TYPE_UV != 0
			{ VertexSemantic::Texcoords, VertexType::Float, 2, VertexSlot::PerVertex, 0 }
#endif
		};

		m_vertexLayout = g.CreateVertexLayout(m_shader, attributes, iol_countof(attributes));

		//**********************************
		// Create PipelineState
		//**********************************

		m_pipelineStates.Create(10);

		{
			// Default
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::TriangleList;
			pipelineParam.pShader = m_shader;
			pipelineParam.pVertexLayout = m_vertexLayout;
			pipelineParam.blendMode = BlendMode::None;
			m_pipelineStates.PushBack(g.CreatePipelineState(pipelineParam));
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
			m_pipelineStates.PushBack(g.CreatePipelineState(pipelineParam));
		}

		{
			// Points
			GraphicsPipeLineStateParam pipelineParam;
			pipelineParam.primitiveType = PrimitiveType::Point;
			pipelineParam.pShader = m_shader;
			pipelineParam.pVertexLayout = m_vertexLayout;
			pipelineParam.blendMode = BlendMode::None;
			pipelineParam.rasterizerFlags &= ~RasterizerFlags_BackFaceCulling;
			pipelineParam.rasterizerFlags |= RasterizerFlags_WireFrame;
			m_pipelineStates.PushBack(g.CreatePipelineState(pipelineParam));
		}

		m_currentPipelineStateIndex = 0;

		//**********************************
		// Create UniformBuffer
		//**********************************

		m_uniformBuffer = g.CreateUniformBuffer(&m_uniformData, sizeof(m_uniformData), BufferUsage::DynamicDraw, "UB_matrices");

		//**********************************
		// Load Mesh
		//**********************************

		//m_mesh.LoadCube();
		m_mesh.LoadObjFile("res/model/sphere_low_poly.obj");

		size_t vertexCount = m_mesh.GetVertexCount();

#if VERTEX_DATA_TYPE_POS != 0 && VERTEX_DATA_TYPE_UV != 0
		VertexPosUV* vertices = iol_alloc_array(VertexPosUV, vertexCount);
#elif VERTEX_DATA_TYPE_POS != 0
		VertexPos* vertices = iol_alloc_array(VertexPos, vertexCount);
#else
#error unknown VertexDataType
#endif

		for (size_t i = 0; i < vertexCount; i++)
		{
#if VERTEX_DATA_TYPE_POS != 0
			vertices[i].pos = m_mesh.positions[i];
#endif

#if VERTEX_DATA_TYPE_UV != 0
			vertices[i].uv = m_mesh.uvs[i];
#endif
		}

		m_vertexBuffer = g.CreateVertexBuffer(vertices, sizeof(*vertices) * vertexCount, BufferUsage::DynamicDraw);
		m_indexBuffer = g.CreateIndexBuffer(m_mesh.indices.pData, m_mesh.GetIndexCount(), BufferUsage::DynamicDraw);

		iol_free(vertices);

		m_vertexArray = g.CreateVertexArray(m_vertexLayout, (const VertexBuffer**)&m_vertexBuffer, 1, m_indexBuffer);

		//**********************************
		// Load Texture
		//**********************************

		TextureParam texParam;
		texParam.filter = TextureFilter::LinearMipMapLinear;
		texParam.format = TextureFormat::RGBA;
		texParam.genMipMaps = true;
		m_texture = g.CreateTextureFromFile("res/texture/prototype_black.png", texParam);
	}

	void GameApplication::Destroy()
	{
		delete m_cameraFlying;
		delete m_camera;

		GraphicsSystem& g = *m_graphicsSystem;
		
		for (size_t i = 0; i < m_pipelineStates.count; i++)
			g.DestroyPipelineState(m_pipelineStates[i]);

		g.DestroyUniformBuffer(m_uniformBuffer);
		g.DestroyShader(m_shader);
		g.DestroyVertexLayout(m_vertexLayout);
		g.DestroyVertexBuffer(m_vertexBuffer);
		g.DestroyIndexBuffer(m_indexBuffer);
		g.DestroyVertexArray(m_vertexArray);
		g.DestroyTexture(m_texture);
	}

	void GameApplication::Update(float deltaTime)
	{
		UpdateCamera(deltaTime);
		Render(deltaTime);
	}

	void GameApplication::UpdateCamera(float deltaTime)
	{
		CameraProp prop = m_camera->GetProp();

		if (input_GetKeyState(IOL_SCANCODE_E) == KeyState_Holding)
		{
			prop.fieldOfViewDegrees += 10.0f * deltaTime;
			prop.orthographicSize += 1.0f * deltaTime;
		}
		else if (input_GetKeyState(IOL_SCANCODE_F) == KeyState_Holding)
		{
			prop.fieldOfViewDegrees -= 10.0f * deltaTime;
			prop.orthographicSize -= 1.0f * deltaTime;
		}

		m_camera->SetProp(prop);

		m_cameraFlying->Update(deltaTime);
	}

	void GameApplication::Render(float deltaTime)
	{
		GraphicsSystem& g = *m_graphicsSystem;

		if (input_GetKeyState(IOL_SCANCODE_TAB) == KeyState_Pressed)
		{
			m_currentPipelineStateIndex = (m_currentPipelineStateIndex + 1) % m_pipelineStates.count;
		}

		mat4 viewProj = m_camera->GetViewProjectionMatrix();
		m_uniformData.mvp = viewProj;
		g.SetUniformBufferData(m_uniformBuffer, &m_uniformData, sizeof(m_uniformData));

		g.BeginRender(m_pipelineStates[m_currentPipelineStateIndex]);
		g.SetViewportFullscreen();
		g.Clear(vec4(0.0f, 0.0f, 0.0f, 1.0f), ClearFlags_All);

		const UniformBuffer* ubs[] = { m_uniformBuffer };
		g.BindUniformBuffer(ubs, iol_countof(ubs));
		g.BindVertexArray(m_vertexArray);
		g.BindTexture(0, (const Texture**)&m_texture, 1);

		g.DrawIndexed(g.GetIndexBufferNumIndices(m_indexBuffer));

		// Draw second object
		vec3 objectPos = vec3(6.0f, 0.0f, 0.0f);
		m_uniformData.mvp = viewProj * glm::translate(mat4(1), objectPos);
		g.SetUniformBufferData(m_uniformBuffer, &m_uniformData, sizeof(m_uniformData));
		g.DrawIndexed(g.GetIndexBufferNumIndices(m_indexBuffer));

		g.EndRender();
	}

	void GameApplication::FixedUpdate(float deltaTime)
	{
	}
}