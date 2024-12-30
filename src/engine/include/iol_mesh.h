#ifndef IOLITE_MESH_H
#define IOLITE_MESH_H

#include "iol_definitions.h"
#include "iol_array.h"

namespace iol
{
	enum class MeshPrimitiveType
	{
		Quad,
		Plane,
		Cube,
		Sphere,
		Capsule
	};

	class Mesh
	{
	public:
		Mesh();
		~Mesh();
		
		bool LoadObjFile(const char* filePath);
		bool WriteBinFile(const char* filePath);
		bool LoadBinFile(const char* filePath);

		void LoadPrimitive(MeshPrimitiveType type);
		void LoadQuad();
		void LoadTerrain(float size, size_t numQuadsPerSide, float heightMin, float heightMax, float tileX, float tileY);
		void LoadCube();
		void LoadSphere();
		void LoadCapsule();
		
		size_t GetVertexCount() const { return positions.count; }
		size_t GetIndexCount() const { return indices.count; }

		Array<glm::vec3> positions;
		Array<glm::vec2> uvs;
		Array<glm::vec3> normals;
		Array<uint32> indices;
	};
}

#endif // IOLITE_MESH_H