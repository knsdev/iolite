#include "iol_graphics.h"
#include "graphics_opengl.h"

namespace iol
{
	struct GraphicsPipelineState
	{
		const Shader* pShader;
		const VertexLayout* pVertexLayout;
		uint32 primitiveType;
		uint32 rasterizerFlags;
		uint32 pipelineFlags;
		BlendMode blendMode;
	};

	struct GraphicsSystemData
	{
		SDL_Window* pWindow;
		SDL_GLContext glContext;
		uint32 screenWidth;
		uint32 screenHeight;
		const GraphicsPipelineState* pPipelineState;
		BlendMode blendMode;
		uint32 shaderProgramId;
		uint32 rasterizerFlags;
		uint32 pipelineFlags;
	};

	bool GraphicsSystem::Create(const GraphicsSystemParam& param)
	{
#ifdef IOL_DEBUG
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

		iol_verify(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);

		iol_verify(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) == 0);
		iol_verify(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2) == 0);
		const char* glsl_version = "#version 420";

		iol_verify(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);
		iol_verify(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) == 0);
		iol_verify(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) == 0);
		iol_verify(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0) == 0);
		iol_verify(SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0) == 0);

		m_data = iol_new(GraphicsSystemData);
		GraphicsSystemData* pSystem = m_data;
		pSystem->pWindow = (SDL_Window*)param.pWindow;
		pSystem->glContext = SDL_GL_CreateContext((SDL_Window*)param.pWindow);
		pSystem->blendMode = BlendMode::None;
		pSystem->screenWidth = param.screenWidth;
		pSystem->screenHeight = param.screenHeight;

		if (pSystem->glContext == nullptr)
		{
			iol_log_error("Failed to create opengl context!");
			return false;
		}

		GLenum glewResult = glewInit();

		if (glewResult != GLEW_OK)
		{
			iol_log_error("Failed to init GLEW! result: %s", glewGetErrorString(glewResult));
			return false;
		}

#ifdef IOL_DEBUG
		gl::SetupDebug();
#endif

		SDL_GL_MakeCurrent(pSystem->pWindow, pSystem->glContext);

		if (param.vsync)
			iol_verify(SDL_GL_SetSwapInterval(1) == 0);
		else
			iol_verify(SDL_GL_SetSwapInterval(0) == 0);
		
		//-------------------------------------
		// ImGui
		//-------------------------------------

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// Setup Platform/Renderer backends
		ImGui_ImplSDL2_InitForOpenGL(pSystem->pWindow, pSystem->glContext);
		ImGui_ImplOpenGL3_Init(glsl_version);

		//-------------------------------------

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		event_system::AddListener(EventType_WindowResize, HandleEvent, m_data);

		return true;
	}

	void GraphicsSystem::Destroy()
	{
		event_system::RemoveListener(EventType_WindowResize, GraphicsSystem::HandleEvent, m_data);
		
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		
		SDL_GL_DeleteContext(m_data->glContext);
		iol_delete(m_data);
	}

	void GraphicsSystem::HandleEvent(const Event& evt, void* userData)
	{
		GraphicsSystemData* graphics = (GraphicsSystemData*)userData;

		switch (evt.type)
		{
		case EventType_WindowResize:
			iol_log("window resize: %u x %u", evt.data.windowResize.newWidth, evt.data.windowResize.newHeight);
			graphics->screenWidth = evt.data.windowResize.newWidth;
			graphics->screenHeight = evt.data.windowResize.newHeight;
			break;

		default:
			break;
		}
	}

	uint32 GraphicsSystem::GetScreenWidth()
	{
		return m_data->screenWidth;
	}

	uint32 GraphicsSystem::GetScreenHeight()
	{
		return m_data->screenHeight;
	}

	float GraphicsSystem::GetScreenAspectRatio()
	{
		return m_data->screenWidth / (float)m_data->screenHeight;
	}

	GraphicsPipelineState* GraphicsSystem::CreatePipelineState(const GraphicsPipeLineStateParam& param)
	{
		GraphicsPipelineState* state = iol_alloc(GraphicsPipelineState);
		state->pShader = (Shader*)param.pShader;
		state->pVertexLayout = (VertexLayout*)param.pVertexLayout;
		state->primitiveType = gl::ConvertPrimitiveType(param.primitiveType);
		state->rasterizerFlags = param.rasterizerFlags;
		state->blendMode = param.blendMode;
		state->pipelineFlags = param.pipelineFlags;

		return state;
	}

	void GraphicsSystem::DestroyPipelineState(GraphicsPipelineState* state)
	{
		iol_free(state);
	}

#ifdef IOL_DEBUG
	void gl::HandleDebugEvent(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		const char* severityStr = "";

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severityStr = "info";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			severityStr = "low";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severityStr = "medium";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			severityStr = "high";
			break;
		default:
			severityStr = "unknown";
			break;
		}

		iol_log_raw("[gl_debug] source: 0x%x, type: 0x%x, id: 0x%x, severity: %s, message: %s", source, type, id, severityStr, message);

		if (severity != GL_DEBUG_SEVERITY_NOTIFICATION && severity != GL_DEBUG_SEVERITY_LOW)
		{
			iol_breakpoint;
		}
	}

	void gl::SetupDebug()
	{
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

		// Ignore Warning: Buffer performance warning: Buffer object 2 (bound to GL_UNIFORM_BUFFER (0), and GL_UNIFORM_BUFFER_EXT,
		// usage hint is GL_DYNAMIC_DRAW) is being copied/moved from VIDEO memory to SYSTEM HEAP memory.
		//GLuint ids[] = { 0x20072 };
		//glDebugMessageControl(0x8246, 0x8250, GL_DONT_CARE, iol_countof(ids), ids, GL_FALSE);

		glDebugMessageCallback(gl::HandleDebugEvent, nullptr);
	}
#endif

	uint32 gl::ConvertBufferUsage(BufferUsage usage)
	{
		uint32 usageGL = 0;

		switch (usage)
		{
		case BufferUsage::StaticDraw:
			usageGL = GL_STATIC_DRAW;
			break;
		case BufferUsage::StaticRead:
			usageGL = GL_STATIC_READ;
			break;
		case BufferUsage::StaticCopy:
			usageGL = GL_STATIC_COPY;
			break;
		case BufferUsage::DynamicDraw:
			usageGL = GL_DYNAMIC_DRAW;
			break;
		case BufferUsage::DynamicRead:
			usageGL = GL_DYNAMIC_READ;
			break;
		case BufferUsage::DynamicCopy:
			usageGL = GL_DYNAMIC_COPY;
			break;
		case BufferUsage::StreamDraw:
			usageGL = GL_STREAM_DRAW;
			break;
		case BufferUsage::StreamRead:
			usageGL = GL_STREAM_READ;
			break;
		case BufferUsage::StreamCopy:
			usageGL = GL_STREAM_COPY;
			break;
		}

		return usageGL;
	}

	uint32 gl::ConvertBufferAccess(BufferAccess access)
	{
		uint32 bufferAccessGL = 0;

		switch (access)
		{
		case BufferAccess::Read:
			bufferAccessGL = GL_READ_ONLY;
			break;
		case BufferAccess::Write:
			bufferAccessGL = GL_WRITE_ONLY;
			break;
		case BufferAccess::ReadWrite:
			bufferAccessGL = GL_READ_WRITE;
			break;
		}

		return bufferAccessGL;
	}

	uint32 gl::ConvertVertexType(VertexType type)
	{
		uint32 resultType = 0;

		switch (type)
		{
		case VertexType::Float:
			resultType = GL_FLOAT;
			break;
		case VertexType::Int:
			resultType = GL_INT;
			break;
		case VertexType::UInt:
			resultType = GL_UNSIGNED_INT;
			break;
		}

		return resultType;
	}

	uint32 gl::GetSizeOfVertexType(VertexType basicType)
	{
		switch (basicType)
		{
		case VertexType::Float:
			return sizeof(GLfloat);
		case VertexType::Int:
			return sizeof(GLint);
		case VertexType::UInt:
			return sizeof(GLuint);
		}

		return 0;
	}

	uint32 gl::ConvertPrimitiveType(PrimitiveType primitiveType)
	{
		switch (primitiveType)
		{
		case PrimitiveType::Point:
			return GL_POINTS;
		case PrimitiveType::LineList:
			return GL_LINES;
		case PrimitiveType::LineStrip:
			return GL_LINE_STRIP;
		case PrimitiveType::TriangleList:
			return GL_TRIANGLES;
		case PrimitiveType::TriangleStrip:
			return GL_TRIANGLE_STRIP;
		}

		return 0;
	}

	void gl::SetBlendMode(BlendMode blendMode)
	{
		switch (blendMode)
		{
		case BlendMode::None:
			glDisable(GL_BLEND);
			break;

		case BlendMode::Blend:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendMode::Add:
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case BlendMode::Mod:
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ONE, GL_ONE);
			break;

		case BlendMode::Multiply:
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		}
	}

	VertexBuffer* GraphicsSystem::CreateVertexBuffer(const void* pVertices, size_t size, BufferUsage usage)
	{
		VertexBuffer* buffer = iol_alloc(VertexBuffer);

		glGenBuffers(1, &buffer->id);
		glBindBuffer(GL_ARRAY_BUFFER, buffer->id);
		glBufferData(GL_ARRAY_BUFFER, size, pVertices, gl::ConvertBufferUsage(usage));

		return buffer;
	}

	void GraphicsSystem::DestroyVertexBuffer(VertexBuffer* pVertexBuffer)
	{
		GLuint id = pVertexBuffer->id;
		glDeleteBuffers(1, &id);
		iol_free(pVertexBuffer);
	}

	void* GraphicsSystem::MapVertexBuffer(VertexBuffer* pVertexBuffer, BufferAccess access)
	{
		GLuint id = pVertexBuffer->id;
		void* pMappedBuffer;
		pMappedBuffer = glMapNamedBuffer(id, gl::ConvertBufferAccess(access));

		return pMappedBuffer;
	}

	void GraphicsSystem::UnmapVertexBuffer(VertexBuffer* pVertexBuffer)
	{
		GLuint id = ((VertexBuffer*)pVertexBuffer)->id;
		glUnmapNamedBuffer(id);
	}

	void GraphicsSystem::SetVertexBufferData(VertexBuffer* pVertexBuffer, const void* pVertices, size_t size)
	{
		void* pMapped = GraphicsSystem::MapVertexBuffer(pVertexBuffer, BufferAccess::Write);
		memory::Copy(pMapped, size, pVertices);
		GraphicsSystem::UnmapVertexBuffer(pVertexBuffer);
	}

	IndexBuffer* GraphicsSystem::CreateIndexBuffer(uint32* pIndices, size_t numIndices, BufferUsage usage)
	{
		IndexBuffer* pBuffer = iol_alloc(IndexBuffer);

		glGenBuffers(1, &pBuffer->id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pBuffer->id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*pIndices) * numIndices, pIndices, gl::ConvertBufferUsage(usage));
		pBuffer->numIndices = numIndices;

		return pBuffer;
	}

	void GraphicsSystem::DestroyIndexBuffer(IndexBuffer* pIndexBuffer)
	{
		GLuint id = pIndexBuffer->id;
		glDeleteBuffers(1, &id);
		iol_free(pIndexBuffer);
	}

	void* GraphicsSystem::MapIndexBuffer(IndexBuffer* pIndexBuffer, BufferAccess access)
	{
		GLuint id = pIndexBuffer->id;
		void* pMappedBuffer;
		pMappedBuffer = glMapNamedBuffer(id, gl::ConvertBufferAccess(access));

		return pMappedBuffer;
	}

	void GraphicsSystem::UnmapIndexBuffer(IndexBuffer* pIndexBuffer)
	{
		GLuint id = pIndexBuffer->id;
		glUnmapNamedBuffer(id);
	}

	void GraphicsSystem::SetIndexBufferData(IndexBuffer* pIndexBuffer, uint32* pIndices, size_t numIndices)
	{
		void* pMapped = GraphicsSystem::MapIndexBuffer(pIndexBuffer, BufferAccess::Write);
		memory::Copy(pMapped, numIndices * sizeof(uint32), pIndices);
		GraphicsSystem::UnmapIndexBuffer(pIndexBuffer);
	}

	size_t GraphicsSystem::GetIndexBufferNumIndices(const IndexBuffer* pIndexBuffer)
	{
		return pIndexBuffer->numIndices;
	}

	UniformBuffer* GraphicsSystem::CreateUniformBuffer(const void* pData, size_t size, BufferUsage usage, const char* pName)
	{
		iol_assert(size % 16 == 0); // buffer data must be 16 bytes aligned

		UniformBuffer* pBuffer = iol_alloc(UniformBuffer);

		pBuffer->pName = pName;

		glGenBuffers(1, &pBuffer->id);
		glBindBuffer(GL_UNIFORM_BUFFER, pBuffer->id);
		glBufferData(GL_UNIFORM_BUFFER, size, pData, gl::ConvertBufferUsage(usage));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return pBuffer;
	}

	void GraphicsSystem::DestroyUniformBuffer(UniformBuffer* pBuffer)
	{
		GLuint id = pBuffer->id;

		glDeleteBuffers(1, &id);
		iol_free(pBuffer);
	}

	void* GraphicsSystem::MapUniformBuffer(UniformBuffer* pBuffer, BufferAccess access)
	{
		GLuint id = pBuffer->id;
		void* pMappedBuffer;
		pMappedBuffer = glMapNamedBuffer(id, gl::ConvertBufferAccess(access));

		return pMappedBuffer;
	}

	void GraphicsSystem::UnmapUniformBuffer(UniformBuffer* pBuffer)
	{
		GLuint id = pBuffer->id;
		glUnmapNamedBuffer(id);
	}

	void GraphicsSystem::SetUniformBufferData(UniformBuffer* pBuffer, const void* pData, size_t size)
	{
		void* pMapped = GraphicsSystem::MapUniformBuffer(pBuffer, BufferAccess::Write);
		memory::Copy(pMapped, size, pData);
		GraphicsSystem::UnmapUniformBuffer(pBuffer);
	}

	VertexLayout* GraphicsSystem::CreateVertexLayout(const Shader* pShader, const VertexAttributeParam* pVertexAttributeParams, size_t numVertexAttributes)
	{
		VertexLayout* pVertexLayout = iol_alloc(VertexLayout);
		graphics_base::CreateVertexLayout(&pVertexLayout->base, pVertexAttributeParams, numVertexAttributes, gl::GetSizeOfVertexType);

		return pVertexLayout;
	}

	void GraphicsSystem::DestroyVertexLayout(VertexLayout* pVertexLayout)
	{
		graphics_base::DestroyVertexLayout(&pVertexLayout->base);
		iol_free(pVertexLayout);
	}
	
	VertexArray* GraphicsSystem::CreateVertexArray(VertexLayout* pVertexLayout, const VertexBuffer** pVertexBuffers, size_t numVertexBuffers, IndexBuffer* pIndexBuffer)
	{
		VertexArray* pVAO = iol_alloc(VertexArray);

		glGenVertexArrays(1, &pVAO->id);
		glBindVertexArray(pVAO->id);

		VertexLayoutBase* pInputLayout = &pVertexLayout->base;

		if (pIndexBuffer != nullptr)
		{
			IndexBuffer* pIndexBufferImpl = (IndexBuffer*)pIndexBuffer;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pIndexBufferImpl->id);
		}

		for (size_t i = 0; i < pInputLayout->numVertexAttributes; i++)
		{
			const VertexAttribute* attribute = &pInputLayout->vertexAttributes[i];
			GLuint attribIdx = (GLuint)i;
			size_t vertexBufferIdx = attribute->param.vertexBufferIndex;
			GLsizei stride = pInputLayout->strides[vertexBufferIdx];

			iol_assert(attribute->param.dimension > 0 && attribute->param.dimension <= 4);

			glEnableVertexAttribArray(attribIdx);

			glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffers[vertexBufferIdx]->id);

			glVertexAttribPointer(
				attribIdx,
				(GLint)attribute->param.dimension,
				gl::ConvertVertexType(attribute->param.type),
				GL_FALSE,
				stride,
				(const void*)attribute->offset);

			GLuint divisor = (attribute->param.slot == VertexSlot::PerInstance) ? 1 : 0;
			glVertexAttribDivisor(attribIdx, divisor);
		}

#if IOL_DEBUG
		pVAO->pVertexLayout = pVertexLayout;
		pVAO->pVertexBuffers = pVertexBuffers;
		pVAO->numVertexBuffers = numVertexBuffers;
		pVAO->pIndexBuffer = pIndexBuffer;
#endif

		return pVAO;
	}

	void GraphicsSystem::DestroyVertexArray(VertexArray* pVertexArray)
	{
		glDeleteVertexArrays(1, &pVertexArray->id);
		iol_free(pVertexArray);
	}

	GLuint gl::CompileShader(GLuint shaderType, const char* pSourceCode)
	{
		GLuint shader;
		shader = glCreateShader(shaderType);

		glShaderSource(shader, 1, &pSourceCode, NULL);
		glCompileShader(shader);

		GLint compileStatus;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

		if (compileStatus != GL_TRUE)
		{
			GLsizei messageLength = 0;
			GLchar message[1024];
			glGetShaderInfoLog(shader, 1024, &messageLength, message);
			iol_log_error("Failed to compile shader!\n%s", message);
			return (GLuint)-1;
		}

		return shader;
	}

	Shader* GraphicsSystem::CreateShader(const char* pSourceCode)
	{
		Shader* pShader = iol_alloc(Shader);

		const char* pShaderTypeKey = "#type";
		size_t shaderTypeKeyLength = string::GetLength(pShaderTypeKey);

		const char* shaderTypes[] = { "vertex", "fragment" };
		const char* pCodeStarts[iol_countof(shaderTypes)];
		const char* pCodeEnds[iol_countof(shaderTypes)];

		memory::FillZero(pCodeStarts, sizeof(pCodeStarts));
		memory::FillZero(pCodeEnds, sizeof(pCodeEnds));

		const char* pCurrent = pSourceCode;

		while (pCurrent && *pCurrent)
		{
			pCurrent = string::Find(pCurrent, pShaderTypeKey) + shaderTypeKeyLength;
			pCurrent = string::Skip(pCurrent, " \t");

			for (size_t i = 0; i < iol_countof(shaderTypes); ++i)
			{
				if (string::Compare(pCurrent, shaderTypes[i]))
				{
					pCurrent = string::GetNextLine(pCurrent);
					pCodeStarts[i] = pCurrent;

					pCurrent = string::Find(pCurrent, pShaderTypeKey);
					pCodeEnds[i] = pCurrent;
					break;
				}
			}
		}

		if (pCodeStarts[0] == nullptr || pCodeStarts[1] == nullptr)
		{
			iol_log_error("failed to find all required shader types");
			return nullptr;
		}

		char* pVertexShaderSource = string::SubstringAlloc(pCodeStarts[0], pCodeEnds[0]);
		char* pFragmentShaderSource = string::SubstringAlloc(pCodeStarts[1], pCodeEnds[1]);

		pShader->programId = glCreateProgram();

		GLuint vertexShader = gl::CompileShader(GL_VERTEX_SHADER, pVertexShaderSource);
		iol_assert(vertexShader != (GLuint)-1);

		GLuint fragmentShader = gl::CompileShader(GL_FRAGMENT_SHADER, pFragmentShaderSource);
		iol_assert(fragmentShader != (GLuint)-1);

		glAttachShader(pShader->programId, vertexShader);
		glAttachShader(pShader->programId, fragmentShader);

		glLinkProgram(pShader->programId);
		glValidateProgram(pShader->programId);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		iol_free(pVertexShaderSource);
		iol_free(pFragmentShaderSource);

		return pShader;
	}

	void GraphicsSystem::DestroyShader(Shader* pShader)
	{
		glDeleteProgram(pShader->programId);
		iol_free(pShader);
	}

	Shader* GraphicsSystem::CreateShaderFromFile(const char* pFilePath)
	{
		char* pShaderSource = file::ReadAllText(pFilePath, nullptr, 0u);
		Shader* pShader = GraphicsSystem::CreateShader(pShaderSource);
		iol_free(pShaderSource);

		return pShader;
	}

	Texture* gl::CreateTexture(int32 width, int32 height, const TextureParam& param, uint32 color, const void* pDataUncompressed)
	{
		uint32 format;

		if (param.format == TextureFormat::RGB)
			format = GL_RGB;
		else
			format = GL_RGBA;

		Texture* pTexture = iol_alloc(Texture);
		pTexture->width = width;
		pTexture->height = height;

		glGenTextures(1, &pTexture->id);
		glBindTexture(GL_TEXTURE_2D, pTexture->id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		uint32 minFilter;
		uint32 maxFilter;

		switch (param.filter)
		{
		case TextureFilter::Linear:
			minFilter = GL_LINEAR;
			maxFilter = GL_LINEAR;
			break;
		case TextureFilter::Nearest:
			minFilter = GL_NEAREST;
			maxFilter = GL_NEAREST;
			break;
		case TextureFilter::NearestMipMapNearest:
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
			maxFilter = GL_NEAREST;
			break;
		case TextureFilter::NearestMipMapLinear:
			minFilter = GL_NEAREST_MIPMAP_LINEAR;
			maxFilter = GL_LINEAR;
			break;
		case TextureFilter::LinearMipMapLinear:
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			maxFilter = GL_LINEAR;
			break;
		case TextureFilter::LinearMipMapNearest:
			minFilter = GL_NEAREST_MIPMAP_NEAREST;
			maxFilter = GL_NEAREST;
			break;
		default:
			minFilter = GL_LINEAR;
			maxFilter = GL_LINEAR;
			break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);

		GLfloat maxAnisotropy;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

		if (pDataUncompressed != nullptr)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pDataUncompressed);
		}
		else
		{
			uint32 length = width * height;
			uint32* pixels = iol_alloc_array(uint32, length);

			for (size_t i = 0; i < length; i++)
			{
				pixels[i] = color;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
			iol_free(pixels);
		}

		if (param.genMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		return pTexture;
	}

	Texture* GraphicsSystem::CreateTextureFromFile(const char* pFilePath, const TextureParam& param)
	{
		size_t size;
		uint8* pDataCompressed = (uint8*)file::ReadAll(pFilePath, &size, 0u);

		if (pDataCompressed == nullptr)
		{
			iol_log_error("Failed to load texture. File not found: '%s'", pFilePath);
			return nullptr;
		}

		int32 width, height, numChannels;

		switch (param.format)
		{
		case TextureFormat::RGB:
			numChannels = 3;
			break;
		default:
			numChannels = 4;
			break;
		}

		stbi_set_flip_vertically_on_load(1);
		uint8* pDataUncompressed = stbi_load_from_memory(pDataCompressed, (int)size, &width, &height, &numChannels, numChannels);
		iol_free(pDataCompressed);

		if (pDataUncompressed == nullptr)
		{
			iol_log_error("Failed to read texture: '%s'", pFilePath);
			return nullptr;
		}

		Texture* texture = gl::CreateTexture(width, height, param, 0, pDataUncompressed);
		stbi_image_free(pDataUncompressed);

		return texture;
	}

	Texture* GraphicsSystem::CreateTexture(uint32 width, uint32 height, uint32 color, const TextureParam& param)
	{
		return gl::CreateTexture(width, height, param, color, nullptr);
	}

	void GraphicsSystem::DestroyTexture(Texture* pTexture)
	{
		glDeleteTextures(1, &pTexture->id);
		iol_free(pTexture);
	}

	RenderTarget* GraphicsSystem::CreateRenderTarget(uint32 width, uint32 height, RenderTargetFlags flags)
	{
		iol_assert(false);
		return nullptr;
	}

	void GraphicsSystem::DestroyRenderTarget(RenderTarget* pRenderTarget)
	{
		iol_assert(false);
	}

	void GraphicsSystem::BeginRender(const GraphicsPipelineState* pPipelineState)
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		GraphicsSystem::SetPipelineState(pPipelineState);

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode, 0);

		ImVec2 viewportSize = viewport->Size;
		m_data->screenWidth = (uint32)viewportSize.x;
		m_data->screenHeight = (uint32)viewportSize.y;
	}

	void GraphicsSystem::EndRender()
	{
		glFinish();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}

		SDL_GL_SwapWindow(m_data->pWindow);
	}

	void GraphicsSystem::Clear(const glm::vec4& color, ClearFlags clearFlags)
	{
		glClearColor(color.x, color.y, color.z, color.w);

		GLbitfield mask = 0;

		if (clearFlags & ClearFlags_Color)
			mask |= GL_COLOR_BUFFER_BIT;
		if (clearFlags & ClearFlags_Depth)
			mask |= GL_DEPTH_BUFFER_BIT;
		if (clearFlags & ClearFlags_Stencil)
			mask |= GL_STENCIL_BUFFER_BIT;

		glClear(mask);
	}

	void GraphicsSystem::Draw(size_t startVertexIndex, size_t numVertices)
	{
		glDrawArrays(m_data->pPipelineState->primitiveType, (GLint)startVertexIndex, (GLsizei)numVertices);
	}

	void GraphicsSystem::DrawIndexed(size_t numIndices)
	{
		glDrawElements(m_data->pPipelineState->primitiveType, (GLsizei)numIndices, GL_UNSIGNED_INT, nullptr);
	}

	void GraphicsSystem::DrawInstanced(size_t startVertexIndex, size_t numVertices, size_t numInstances)
	{
		glDrawArraysInstanced(m_data->pPipelineState->primitiveType, (GLint)startVertexIndex, (GLsizei)numVertices, (GLsizei)numInstances);
	}

	void GraphicsSystem::DrawIndexedInstanced(size_t numIndices, size_t numInstances)
	{
		glDrawElementsInstanced(m_data->pPipelineState->primitiveType, (GLsizei)numIndices, GL_UNSIGNED_INT, nullptr, (GLsizei)numInstances);
	}

	void GraphicsSystem::BindVertexArray(const VertexArray* pVertexArray)
	{
#ifdef IOL_DEBUG
		// PipelineState stores the VertexLayout (required for D3D12),
		// so for compatibility reasons the VertexLayout is not allowed to change during the execution of a GraphicsCommandList.
		iol_assert(pVertexArray->pVertexLayout == m_data->pPipelineState->pVertexLayout);
#endif

		glBindVertexArray(pVertexArray->id);
	}

	void GraphicsSystem::BindUniformBuffer(const UniformBuffer** pBuffers, size_t numBuffers)
	{
		const GraphicsPipelineState* pState = m_data->pPipelineState;

		for (GLuint i = 0; i < numBuffers; ++i)
		{
			const UniformBuffer* pCBuffer = (const UniformBuffer*)pBuffers[i];

			GLuint blockIdx = glGetUniformBlockIndex(pState->pShader->programId, pCBuffer->pName);
			iol_assert(blockIdx != (GLuint)-1);

			glUniformBlockBinding(pState->pShader->programId, blockIdx, i);
			glBindBufferBase(GL_UNIFORM_BUFFER, i, pCBuffer->id);
		}
	}

	void GraphicsSystem::BindTexture(size_t startSlot, const Texture** pTextures, size_t numTextures)
	{
		GLuint start = (GLuint)startSlot;
		GLuint slotIdx;
		size_t i;

		for (slotIdx = start, i = 0; slotIdx < start + numTextures; ++slotIdx, ++i)
		{
			Texture* pTextureImpl = (Texture*)pTextures[i];

			glActiveTexture(s_textureSlots[slotIdx]);
			glBindTexture(GL_TEXTURE_2D, pTextureImpl->id);
		}
	}

	void GraphicsSystem::SetViewport(const ViewportData& data)
	{
		glViewport((GLint)data.topLeftX, (GLint)data.topLeftY, (GLsizei)data.width, (GLsizei)data.height);
	}

	void GraphicsSystem::SetViewportFullscreen()
	{
		ViewportData vp;
		vp.topLeftX = 0;
		vp.topLeftY = 0;
		vp.width = GraphicsSystem::GetScreenWidth();
		vp.height = GraphicsSystem::GetScreenHeight();
		SetViewport(vp);
	}

	void GraphicsSystem::SetPipelineState(const GraphicsPipelineState* pPipelineState)
	{
		GraphicsSystemData* pSystem = m_data;

		const GraphicsPipelineState* pState = (const GraphicsPipelineState*)pPipelineState;
		pSystem->pPipelineState = pState;

		if (pSystem->shaderProgramId != pState->pShader->programId)
		{
			pSystem->shaderProgramId = pState->pShader->programId;
			glUseProgram(pState->pShader->programId);
		}

		if ((pSystem->rasterizerFlags & RasterizerFlags_BackFaceCulling) != (pState->rasterizerFlags & RasterizerFlags_BackFaceCulling))
		{
			if (pState->rasterizerFlags & RasterizerFlags_BackFaceCulling)
			{
				pSystem->rasterizerFlags |= RasterizerFlags_BackFaceCulling;
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
			}
			else
			{
				pSystem->rasterizerFlags &= ~RasterizerFlags_BackFaceCulling;
				glDisable(GL_CULL_FACE);
			}
		}

		if ((pSystem->rasterizerFlags & RasterizerFlags_WireFrame) != (pState->rasterizerFlags & RasterizerFlags_WireFrame))
		{
			if (pState->rasterizerFlags & RasterizerFlags_WireFrame)
			{
				pSystem->rasterizerFlags |= RasterizerFlags_WireFrame;
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				pSystem->rasterizerFlags &= ~RasterizerFlags_WireFrame;
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		if ((pSystem->pipelineFlags & GraphicsPipelineFlags_DepthEnabled) != (pState->pipelineFlags & GraphicsPipelineFlags_DepthEnabled))
		{
			if (pState->pipelineFlags & GraphicsPipelineFlags_DepthEnabled)
			{
				pSystem->pipelineFlags |= GraphicsPipelineFlags_DepthEnabled;
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				pSystem->pipelineFlags &= ~GraphicsPipelineFlags_DepthEnabled;
				glDisable(GL_DEPTH_TEST);
			}
		}

		if (pSystem->blendMode != pState->blendMode)
		{
			pSystem->blendMode = pState->blendMode;
			gl::SetBlendMode(pState->blendMode);
		}
	}
}