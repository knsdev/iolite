#ifndef IOL_APPLICATION_H
#define IOL_APPLICATION_H

#include "iolite.h"

namespace iol
{
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

	enum TerrainEditToolType
	{
		TerrainEditToolType_DragHeight,
		TerrainEditToolType_Flatten
	};

	enum TerrainEditState
	{
		TerrainEditState_Initial,
		TerrainEditState_DragHeight,
	};

	class GameApplication : public Application
	{
	public:
		GameApplication() { }
		virtual ~GameApplication() { }

		void Create(const EngineParams& params) override;
		void Destroy() override;
		void Update(float deltaTime) override;
		void FixedUpdate(float deltaTime) override;

	private:
		void Render(float deltaTime);

	private:
		Camera* m_camera;
		CameraFlying* m_cameraFlying;

		Shader* m_shaderMVPTexture;
		Shader* m_shaderMVPColor;
		VertexLayout* m_vertexLayoutMVPTexture;
		VertexLayout* m_vertexLayoutMVPColor;
		Array<GraphicsPipelineState*> m_pipelineStates;
		GraphicsPipelineState* m_pipelineStateMVPColorWireframe;
		size_t m_currentPipelineStateIndex;
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

		float m_editRadius = 5.0f;
		static constexpr float m_editRadiusMin = 1.0f;
		static constexpr float m_editRadiusMax = 15.0f;
		static constexpr float m_editRadiusScrollSpeed = 20.0f;

		TerrainEditToolType m_toolType = TerrainEditToolType_DragHeight;
		TerrainEditState m_editState = TerrainEditState_Initial;
		glm::vec2 m_startMousePos;
		glm::vec3 m_startHitPoint;
		Array<uint32> m_selectedIndices;
		Array<glm::vec3> m_selectedOriginalPositions;
		Array<float> m_selectedOriginalDistances;

		float m_flattenDesiredHeight;

		float m_perlinOffsetX;
	};
}

#endif // IOL_APPLICATION_H