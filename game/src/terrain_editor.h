#ifndef IOL_TERRAIN_EDITOR
#define IOL_TERRAIN_EDITOR

#include "iolite.h"

namespace iol
{
	enum TerrainEditToolType
	{
		TerrainEditToolType_DragHeight,
		TerrainEditToolType_Flatten,

		TerrainEditToolType_Count
	};

	enum TerrainEditState
	{
		TerrainEditState_Initial,
		TerrainEditState_DragHeight,

		TerrainEditState_Count
	};

	enum TerrainPipelineStateType
	{
		TerrainPipelineStateType_None,
		TerrainPipelineStateType_MVPTexture,
		TerrainPipelineStateType_MVPTextureWireframe,
		TerrainPipelineStateType_MVPColorWireframe,

		TerrainPipelineStateType_Count
	};

	class TerrainEditor
	{
	public:
		TerrainEditor();
		~TerrainEditor();

		/*
		* Initializes the TerrainEditor, loads graphics-related resources and creates the Terrain Mesh.
		* 
		* g: GraphicsSystem pointer
		* size: terrain size, width & depth of the terrain mesh
		* numQuadsPerSide: how many quads exist per side. Affects the mesh vertex count (low or high-poly).
		* tileX: texture tiling factor in x direction
		* tileY: texture tiling factor in y direction
		*/
		void Create(GraphicsSystem* g, float size = 40.0f, size_t numQuadsPerSide = 80, float tileX = 10.0f, float tileY = 10.0f);

		/*
		* Destroys the TerrainEditor and frees its allocated resources.
		*/
		void Destroy(GraphicsSystem* g);

		void Update(GraphicsSystem* g, const Camera* camera, float deltaTime);

		void Render(GraphicsSystem* g, const glm::mat4& viewProjection);
		void RenderGUI(GraphicsSystem* g);

		const GraphicsPipelineState* GetPipelineStateMVPTexture() const { return m_pipelineStateMVPTexture; }
		const GraphicsPipelineState* GetPipelineStateMVPTextureWireframe() const { return m_pipelineStateMVPTextureWireframe; }
		const GraphicsPipelineState* GetPipelineStateMVPColorWireframe() const { return m_pipelineStateMVPColorWireframe; }

	private:

		struct VertexPosUV
		{
			glm::vec3 pos;
			glm::vec2 uv;
		};

		struct UniformDataMatrices
		{
			glm::mat4 mvp;
		};

		struct UniformDataMaterial
		{
			glm::vec4 color;
		};

		Shader* m_shaderMVPTexture;
		Shader* m_shaderMVPColor;
		VertexLayout* m_vertexLayoutMVPTexture;
		VertexLayout* m_vertexLayoutMVPColor;
		GraphicsPipelineState* m_pipelineStateMVPTexture;
		GraphicsPipelineState* m_pipelineStateMVPTextureWireframe;
		GraphicsPipelineState* m_pipelineStateMVPColorWireframe;

		UniformDataMatrices m_uniformDataMatrices;
		UniformDataMaterial m_uniformDataMaterial;
		UniformBuffer* m_uniformBufferMatrices;
		UniformBuffer* m_uniformBufferMaterial;

		Mesh m_mesh;
		VertexPosUV* m_verticesPosUV;
		glm::vec3* m_verticesPos;
		size_t m_vertexCount;
		VertexBuffer* m_vertexBufferPosUV;
		VertexBuffer* m_vertexBufferPos;
		IndexBuffer* m_indexBuffer;
		VertexArray* m_vertexArrayMVPTexture;
		VertexArray* m_vertexArrayMVPColor;

		Texture* m_texture;

		TerrainEditToolType m_toolType = TerrainEditToolType_DragHeight;
		TerrainEditState m_editState = TerrainEditState_Initial;
		TerrainPipelineStateType m_pipelineStateType = TerrainPipelineStateType_MVPTexture;

		float m_editRadius = 5.0f;
		static constexpr float m_editRadiusMin = 1.0f;
		static constexpr float m_editRadiusMax = 15.0f;
		static constexpr float m_editRadiusScrollSpeed = 20.0f;

		float m_flattenDesiredHeight = FLT_MAX;
		glm::vec2 m_startMousePos;
		glm::vec3 m_startHitPoint;
		Array<uint32> m_selectedIndices;
		Array<glm::vec3> m_selectedOriginalPositions;
		Array<float> m_selectedOriginalDistances;
	};
}

#endif // IOL_TERRAIN_EDITOR