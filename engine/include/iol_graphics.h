#ifndef IOLITE_GRAPHICS_H
#define IOLITE_GRAPHICS_H

#include "iol_definitions.h"
#include "iol_memory.h"

namespace iol
{
	enum
	{
		MaxBoundConstantBuffers = 32,
		MaxBoundTextures = 16,
	};

	struct Event;

	struct VertexLayout;
	struct VertexBuffer;
	struct IndexBuffer;
	struct UniformBuffer;
	struct Shader;
	struct RenderTarget;
	struct GraphicsPipelineState;
	struct VertexArray;
	struct Texture;

	enum class PrimitiveType
	{
		Point,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,

		Count
	};

	enum class ShaderType
	{
		Vertex,
		Fragment,

		Count
	};

	enum class BufferUsage
	{
		StaticDraw,
		StaticRead,
		StaticCopy,
		DynamicDraw,
		DynamicRead,
		DynamicCopy,
		StreamDraw,
		StreamRead,
		StreamCopy,
	};

	enum class BufferAccess
	{
		Read,
		Write,
		ReadWrite
	};

	enum ClearFlags
	{
		ClearFlags_Color = iol_bit(0),
		ClearFlags_Depth = iol_bit(1),
		ClearFlags_Stencil = iol_bit(2),
		ClearFlags_All = ClearFlags_Color | ClearFlags_Depth | ClearFlags_Stencil,
	};

	enum class BlendMode
	{
		/*
		 * no blending
		 * dstRGBA = srcRGBA
		 */
		None,

		/* alpha blending
		 * dstRGB = (srcRGB * srcA) + (dstRGB * (1-srcA))
		 * dstA = srcA + (dstA * (1-srcA))
		 */
		Blend, 

		/*
		 * additive blending
		 * dstRGB = (srcRGB * srcA) + dstRGB
		 * dstA = dstA
		 */
		Add,

		/*
		 * color modulate
		 * dstRGB = srcRGB * dstRGB
		 * dstA = dstA
		 */
		Mod, 

		/*
		 * color multiply
		 * dstRGB = (srcRGB * dstRGB) + (dstRGB * (1-srcA))
		 * dstA = (srcA * dstA) + (dstA * (1-srcA))
		 */
		Multiply,
	};

	enum class VertexSemantic
	{
		Invalid = -1,

		Binormal,
		BlendIndices,
		BlendWeights,
		Color,
		Normal,
		Position,
		PositionTransformed,
		PointSize,
		Tangent,
		Texcoords,
		Custom,

		Count
	};

	enum class VertexType
	{
		Float,
		Int,
		UInt
	};

	enum class VertexSlot
	{
		PerVertex,
		PerInstance
	};

	struct VertexAttributeParam
	{
		VertexSemantic semantic;
		VertexType type;
		uint32 dimension;
		VertexSlot slot;
		size_t vertexBufferIndex;
	};

	enum RenderTargetFlags
	{
		RenderTargetFlags_None,
		RenderTargetFlags_DepthBuffer = iol_bit(1)
	};

	enum class TextureFilter
	{
		Nearest,
		Linear,
		NearestMipMapNearest,
		NearestMipMapLinear,
		LinearMipMapLinear,
		LinearMipMapNearest
	};

	enum class TextureFormat
	{
		RGBA,
		RGB
	};

	struct TextureParam
	{
		TextureFormat format = TextureFormat::RGBA;
		TextureFilter filter = TextureFilter::LinearMipMapLinear;
		bool genMipMaps = true;
	};

	struct ViewportData
	{
		int32 topLeftX;
		int32 topLeftY;
		uint32 width;
		uint32 height;
	};

	enum RasterizerFlags
	{
		RasterizerFlags_None = 0,
		RasterizerFlags_Antialiasing = iol_bit(0),
		RasterizerFlags_BackFaceCulling = iol_bit(1),
		RasterizerFlags_WireFrame = iol_bit(2)
	};

	enum GraphicsPipelineFlags
	{
		GraphicsPipelineFlags_None = 0,
		GraphicsPipelineFlags_DepthEnabled = iol_bit(0),
		GraphicsPipelineFlags_StencilEnabled = iol_bit(1)
	};

	struct GraphicsPipeLineStateParam
	{
		Shader* pShader = nullptr;
		VertexLayout* pVertexLayout = nullptr;
		RenderTarget* pRenderTargets = nullptr;
		size_t numRenderTargets = 0;
		PrimitiveType primitiveType = PrimitiveType::TriangleList;
		BlendMode blendMode = BlendMode::None;
		uint32 rasterizerFlags = RasterizerFlags_BackFaceCulling;
		uint32 pipelineFlags = GraphicsPipelineFlags_DepthEnabled;
	};

	struct GraphicsSystemParam
	{
		void* pWindow;
		uint32 screenWidth;
		uint32 screenHeight;
		bool vsync;
	};

	class IOL_API GraphicsSystem
	{
	public:
		GraphicsSystem() {}
		~GraphicsSystem() {}

		bool                     Create(const GraphicsSystemParam& param);
		void                     Destroy();

		static void              HandleEvent(const Event& evt, void* userData);

		uint32                   GetScreenWidth();
		uint32                   GetScreenHeight();
		float                    GetScreenAspectRatio();

		GraphicsPipelineState*   CreatePipelineState(const GraphicsPipeLineStateParam& param);
		void                     DestroyPipelineState(GraphicsPipelineState* pPipelineState);

		void                     BeginRender(const GraphicsPipelineState* pPipelineState);
		void                     EndRender();
		void                     Clear(const glm::vec4& color, ClearFlags clearFlags);
		void                     Draw(size_t startVertexIndex, size_t numVertices);
		void                     DrawIndexed(size_t numIndices);
		void                     DrawInstanced(size_t startVertexIndex, size_t numVertices, size_t numInstances);
		void                     DrawIndexedInstanced(size_t numIndices, size_t numInstances);
		void                     BindVertexArray(const VertexArray* pVertexArray);
		void                     BindUniformBuffer(const UniformBuffer** pBuffers, size_t numBuffers);
		void                     BindTexture(size_t startSlot, const Texture** pTextures, size_t numTextures);
		void                     SetViewport(const ViewportData& data);
		void                     SetViewportFullscreen();
		void                     SetPipelineState(const GraphicsPipelineState* pPipelineState);

		Shader*                  CreateShader(const char* pSourceCode);
		Shader*                  CreateShaderFromFile(const char* pFilePath);
		void                     DestroyShader(Shader* pShader);

		/* The input-layout object can be reused with any other shader that has an identical input signature. */
		VertexLayout*            CreateVertexLayout(const Shader* pShader, const VertexAttributeParam* pVertexAttributeParams, size_t numVertexAttributes);
		void                     DestroyVertexLayout(VertexLayout* pVertexLayout);

		VertexBuffer*            CreateVertexBuffer(const void* pVertices, size_t size, BufferUsage usage);
		void                     DestroyVertexBuffer(VertexBuffer* pVertexBuffer);

		void*                    MapVertexBuffer(VertexBuffer* pVertexBuffer, BufferAccess access);
		void                     UnmapVertexBuffer(VertexBuffer* pVertexBuffer);
		void                     SetVertexBufferData(VertexBuffer* pVertexBuffer, const void* pVertices, size_t size);

		IndexBuffer*             CreateIndexBuffer(uint32* pIndices, size_t numIndices, BufferUsage usage);
		void                     DestroyIndexBuffer(IndexBuffer* pIndexBuffer);

		void*                    MapIndexBuffer(IndexBuffer* pIndexBuffer, BufferAccess access);
		void                     UnmapIndexBuffer(IndexBuffer* pIndexBuffer);
		void                     SetIndexBufferData(IndexBuffer* pIndexBuffer, uint32* pIndices, size_t numIndices);
		size_t                   GetIndexBufferNumIndices(const IndexBuffer* pIndexBuffer);

		VertexArray*             CreateVertexArray(VertexLayout* pVertexLayout, const VertexBuffer** pVertexBuffers, size_t numVertexBuffers, IndexBuffer* pIndexBuffer);
		void                     DestroyVertexArray(VertexArray* pVertexArray);

		/* 'size' must be 16 bytes aligned.
			'pName' is the name of the uniform in the shader.

			For example:

			layout (std140) uniform UB_material
			{
				vec4 objectColor;
				vec4 lightColor;
			} material;

			'pName' would be 'UB_material'. */
		UniformBuffer*           CreateUniformBuffer(const void* pData, size_t size, BufferUsage usage, const char* pName);
		void                     DestroyUniformBuffer(UniformBuffer* pBuffer);

		void*                    MapUniformBuffer(UniformBuffer* pBuffer, BufferAccess access);
		void                     UnmapUniformBuffer(UniformBuffer* pBuffer);
		void                     SetUniformBufferData(UniformBuffer* pBuffer, const void* pData, size_t size);

		Texture*                 CreateTextureFromFile(const char* pFilePath, const TextureParam& param);
		Texture*                 CreateTexture(uint32 width, uint32 height, uint32 color, const TextureParam& param);
		void                     DestroyTexture(Texture* pTexture);

		RenderTarget*            CreateRenderTarget(uint32 width, uint32 height, RenderTargetFlags flags);
		void                     DestroyRenderTarget(RenderTarget* pRenderTarget);

	private:
		struct GraphicsSystemData* m_data;
	};
}

#endif // IOL_GRAPHICS_H