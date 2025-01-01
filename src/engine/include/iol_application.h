#ifndef IOLITE_APPLICATION_H
#define IOLITE_APPLICATION_H

#include "iol_definitions.h"

namespace iol
{
	struct EngineParams;
	class GraphicsSystem;

	class IOL_API Application
	{
	public:
		Application() {}
		virtual ~Application() {}

		virtual void Create(const EngineParams& params) = 0;
		virtual void Destroy() = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void FixedUpdate(float deltaTime) = 0;

		GraphicsSystem* m_graphicsSystem;
	};
}

#endif // IOLITE_APPLICATION_H
