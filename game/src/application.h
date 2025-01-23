#ifndef IOL_APPLICATION_H
#define IOL_APPLICATION_H

#include "iolite.h"
#include "terrain_editor.h"

namespace iol
{
	class GameApplication : public Application
	{
	public:
		GameApplication() { }
		virtual ~GameApplication() { }

		void Create(const EngineParams& params) override;
		void Destroy() override;
		void FixedUpdate(float deltaTime) override;
		void Update(float deltaTime) override;

	private:
		void Render(float deltaTime);

	private:
		Camera* m_camera;
		CameraFlying* m_cameraFlying;
		TerrainEditor m_terrainEditor;
		const GraphicsPipelineState* m_pipelineStateDefault;
	};
}

#endif // IOL_APPLICATION_H