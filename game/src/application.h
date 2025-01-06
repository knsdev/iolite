#ifndef IOL_APPLICATION_H
#define IOL_APPLICATION_H

#include "iolite.h"

namespace iol
{
	struct VertexPos
	{
		glm::vec3 pos;
	};

	struct VertexPosUV
	{
		glm::vec3 pos;
		glm::vec2 uv;
	};

	struct UniformData
	{
		glm::mat4 mvp;
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

		Shader* m_shader;
		VertexLayout* m_vertexLayout;
		Array<GraphicsPipelineState*> m_pipelineStates;
		size_t m_currentPipelineStateIndex;
		UniformData m_uniformData;
		UniformBuffer* m_uniformBuffer;

		Mesh m_mesh;
		VertexPosUV* m_vertices;
		size_t m_vertexCount;
		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
		VertexArray* m_vertexArray;

		Texture* m_texture;

		float m_editRadius = 5.0f;
		const float m_editRadiusMin = 1.0f;
		const float m_editRadiusMax = 15.0f;
		const float m_editRadiusScrollSpeed = 20.0f;

		TerrainEditState m_editState = TerrainEditState_Initial;
		glm::vec2 m_startMousePos;
		glm::vec3 m_startHitPoint;
		Array<uint32> m_selectedIndices;
		Array<glm::vec3> m_selectedOriginalPositions;
		Array<float> m_selectedOriginalDistances;

		float m_perlinOffsetX;
	};
}

#endif // IOL_APPLICATION_H