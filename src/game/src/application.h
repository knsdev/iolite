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
		void UpdateCamera(float deltaTime);

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
		VertexBuffer* m_vertexBuffer;
		IndexBuffer* m_indexBuffer;
		VertexArray* m_vertexArray;

		Texture* m_texture;
	};
}

#endif // IOL_APPLICATION_H