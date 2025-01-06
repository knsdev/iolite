#ifndef GRAPHICS_INTERNAL_OPENGL_H
#define GRAPHICS_INTERNAL_OPENGL_H

#include <stdlib.h>
#include <string.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include "SDL.h"
#include "GL/glew.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "stb_image/stb_image.h"
#include "iol_graphics.h"
#include "graphics_base.h"
#include "iol_definitions.h"
#include "iol_debug.h"
#include "iol_memory.h"
#include "iol_array.h"
#include "iol_string.h"
#include "iol_event.h"
#include "iol_file.h"

namespace iol
{
	static GLenum s_textureSlots[] =
	{
		GL_TEXTURE0,
		GL_TEXTURE1,
		GL_TEXTURE2,
		GL_TEXTURE3,
		GL_TEXTURE4,
		GL_TEXTURE5,
		GL_TEXTURE6,
		GL_TEXTURE7,
		GL_TEXTURE8,
		GL_TEXTURE9,
		GL_TEXTURE10,
		GL_TEXTURE11,
		GL_TEXTURE12,
		GL_TEXTURE13,
		GL_TEXTURE14,
		GL_TEXTURE15,
	};

	struct Texture
	{
		uint32 id;
		int32 width;
		int32 height;
		TextureFormat format;
	};

	struct RenderTarget
	{

	};

	struct Shader
	{
		GLuint programId;
	};

	struct UniformBuffer
	{
		GLuint id;
		const char* pName;
	};

	struct VertexLayout
	{
		VertexLayoutBase base;
	};

	struct VertexBuffer
	{
		GLuint id;
	};

	struct IndexBuffer
	{
		GLuint id;
		size_t numIndices;
	};

	struct VertexArray
	{
		GLuint id;

#ifdef IOL_DEBUG
		VertexLayout* pVertexLayout;
		const VertexBuffer** pVertexBuffers;
		size_t numVertexBuffers;
		IndexBuffer* pIndexBuffer;
#endif
	};

	uint32			gl_ConvertBufferUsage(BufferUsage usage);
	uint32			gl_ConvertBufferAccess(BufferAccess access);
	uint32			gl_ConvertVertexType(VertexType type);
	uint32			gl_GetSizeOfVertexType(VertexType basicType);
	uint32			gl_ConvertPrimitiveType(PrimitiveType primitiveType);
	GLuint			gl_CompileShader(GLuint shaderType, const char* pSourceCode);
	void			gl_SetBlendMode(BlendMode blendMode);
	uint32			gl_ConvertTextureFilter(TextureFilter filter);
	Texture*		gl_CreateTexture(int32 width, int32 height, const TextureParam& param, uint32 color, const void* pDataUncompressed);

#ifdef IOL_DEBUG
	void			gl_HandleDebugEvent(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
	void			gl_SetupDebug();
#endif
}

#endif // GRAPHICS_INTERNAL_OPENGL_H