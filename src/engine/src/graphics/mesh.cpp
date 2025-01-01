#include "iol_mesh.h"
#include "iol_file.h"
#include "iol_hashmap.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/ext/quaternion_common.hpp"
#include "glm/gtx/norm.hpp"
#include <stdio.h>
#include <string>

#define DB_PERLIN_IMPL
#include "db_perlin.hpp"

using namespace glm;

namespace iol
{
	struct MeshVertex
	{
		vec3 pos;
		vec2 uv;
		vec3 normal;
		size_t index;
	};

	struct MeshFace
	{
		uint32 indices[3];

		MeshFace()
		{
		}

		MeshFace(uint32 A, uint32 B, uint32 C)
		{
			indices[0] = A;
			indices[1] = B;
			indices[2] = C;
		}
	};

	struct MeshVertexKey
	{
		uint32 iPos;
		uint32 iUV;
		uint32 iNormal;

		bool operator==(const MeshVertexKey& other) const
		{
			return memory_Compare(this, &other, sizeof(*this));
		}

		bool operator!=(const MeshVertexKey& other) const
		{
			return !operator==(other);
		}
	};

	uint32 GetHashForIndexCombination(const MeshVertexKey& key)
	{
		return (key.iPos * 293 + key.iUV * 127 + key.iNormal * 41);
	}

	Mesh::Mesh()
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::LoadPrimitive(MeshPrimitiveType type)
	{
		switch (type)
		{
		case MeshPrimitiveType::Quad:
			LoadQuad();
			break;
		case MeshPrimitiveType::Plane:
			LoadTerrain(5.0f, 5, 5.0f, 5.0f, 0.0f, 0.0f, 1.0f);
			break;
		case MeshPrimitiveType::Cube:
			LoadCube();
			break;
		case MeshPrimitiveType::Sphere:
			LoadSphere();
			break;
		case MeshPrimitiveType::Capsule:
			LoadCapsule();
			break;
		default:
			iol_assert(false);
			break;
		}
	}

	void Mesh::LoadQuad()
	{
		float s = 0.5f;

		positions.Create(4);
		positions.PushBack(vec3(-s, -s, 0.0f));
		positions.PushBack(vec3(s, -s, 0.0f));
		positions.PushBack(vec3(s, s, 0.0f));
		positions.PushBack(vec3(-s, s, 0.0f));

		float uvMin = 0.0f;
		float uvMax = 1.0f;

		uvs.Create(4);
		uvs.PushBack(vec2(uvMin, uvMin));
		uvs.PushBack(vec2(uvMax, uvMin));
		uvs.PushBack(vec2(uvMax, uvMax));
		uvs.PushBack(vec2(uvMin, uvMax));

		uint32 _indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		indices.Create(6);
		indices.PushBackArray(_indices, iol_countof(_indices));
	}

	vec3 CalcTerrainVertexPos(float x, float z, float heightMin, float heightMax, float quadSize, float size)
	{
		vec3 pos = vec3(x, 0, z) * quadSize;
		pos.y = mix(heightMin, heightMax, db::perlin(pos.x / size, pos.z / size));
		return pos;
	}

	void Mesh::LoadTerrain(float size, size_t numQuadsPerSide, float tileX, float tileY, float heightMin, float heightMax, float perlinScale)
	{
		size_t numVerticesPerSide = numQuadsPerSide + 1; // One extra vertex per row/column for shared edges
		float quadSize = size / numQuadsPerSide;

		positions.Create(numVerticesPerSide * numVerticesPerSide);
		uvs.Create(numVerticesPerSide * numVerticesPerSide);
		indices.Create(6 * numQuadsPerSide * numQuadsPerSide);

		// Generate vertices and UVs
		for (int32 iVertexY = 0; iVertexY < numVerticesPerSide; iVertexY++)
		{
			for (int32 iVertexX = 0; iVertexX < numVerticesPerSide; iVertexX++)
			{
				// Calculate vertex position
				vec3 vertexPos = CalcTerrainVertexPos(iVertexX, -iVertexY, heightMin, heightMax, quadSize, size * perlinScale);
				positions.PushBack(vertexPos);

				// Calculate UV coordinates
				float u = float(iVertexX) / numQuadsPerSide * tileX; // Normalized UVs
				float v = float(iVertexY) / numQuadsPerSide * tileY;
				uvs.PushBack(vec2(u, v));
			}
		}

		// Generate indices
		for (int32 iQuadY = 0; iQuadY < numQuadsPerSide; iQuadY++)
		{
			for (int32 iQuadX = 0; iQuadX < numQuadsPerSide; iQuadX++)
			{
				// Calculate vertex indices for the two triangles forming a quad
				size_t bottomLeft = iQuadY * numVerticesPerSide + iQuadX;
				size_t bottomRight = bottomLeft + 1;
				size_t topLeft = bottomLeft + numVerticesPerSide;
				size_t topRight = topLeft + 1;

				// First triangle
				indices.PushBack(topLeft);
				indices.PushBack(bottomLeft);
				indices.PushBack(bottomRight);

				// Second triangle
				indices.PushBack(topRight);
				indices.PushBack(topLeft);
				indices.PushBack(bottomRight);
			}
		}
	}

	void Mesh::LoadCube()
	{
		size_t vertexCount = 36; // 3 * 2 * 6
		float s = 0.5f;

		positions.Create(vertexCount);
		uvs.Create(vertexCount);
		indices.Create(vertexCount);

		// bottom face

		positions.PushBack(vec3(s, -s, -s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(s, -s, s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		positions.PushBack(vec3(-s, -s, s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(s, -s, -s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(-s, -s, s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(-s, -s, -s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		// top face

		positions.PushBack(vec3(-s, s, s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(s, s, s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(s, s, -s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		positions.PushBack(vec3(-s, s, -s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(-s, s, s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(s, s, -s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		// back face

		positions.PushBack(vec3(-s, -s, -s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(-s, s, -s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		positions.PushBack(vec3(s, s, -s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(s, s, -s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(s, -s, -s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(-s, -s, -s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		// front face

		positions.PushBack(vec3(s, s, s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		positions.PushBack(vec3(-s, s, s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(-s, -s, s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(-s, -s, s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(s, -s, s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(s, s, s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		// left face

		positions.PushBack(vec3(-s, -s, s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(-s, s, s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		positions.PushBack(vec3(-s, s, -s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(-s, s, -s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(-s, -s, -s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(-s, -s, s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		// right face

		positions.PushBack(vec3(s, s, -s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		positions.PushBack(vec3(s, s, s));
		uvs.PushBack(vec2(0.0f, 1.0f));

		positions.PushBack(vec3(s, -s, s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(s, -s, s));
		uvs.PushBack(vec2(0.0f, 0.0f));

		positions.PushBack(vec3(s, -s, -s));
		uvs.PushBack(vec2(1.0f, 0.0f));

		positions.PushBack(vec3(s, s, -s));
		uvs.PushBack(vec2(1.0f, 1.0f));

		for (size_t i = 0; i < positions.count; i++)
		{
			indices.PushBack(i);
		}
	}

	void Mesh::LoadSphere()
	{

	}

	void Mesh::LoadCapsule()
	{

	}

	bool Mesh::LoadObjFile(const char* pFilePath)
	{
		size_t fileSize;
		char* pBuffer = file_ReadAllText(pFilePath, &fileSize, 0u);

		char* pCurrent = pBuffer;
		char* pEnd = pBuffer + fileSize;
		size_t numPositions = 0, numUVs = 0, numNormals = 0, numIndices = 0;

		while (pCurrent < pEnd)
		{
			if (pCurrent[0] == 'v')
			{
				if (pCurrent[1] == ' ')
				{
					numPositions++;
				}
				else if (pCurrent[1] == 't')
				{
					numUVs++;
				}
				else if (pCurrent[1] == 'n')
				{
					numNormals++;
				}
			}
			else if (pCurrent[0] == 'f')
			{
				numIndices += 3;
			}

			char* pLineEnd = strchr(pCurrent, '\n');

			if (pLineEnd)
				pCurrent = pLineEnd;

			pCurrent++;
		}

		Array<vec3> positions(numPositions);
		Array<vec2> uvs(numUVs);
		Array<vec3> normals(numNormals);

		Array<uint32> indicesPos(numIndices);
		Array<uint32> indicesUV(numIndices);
		Array<uint32> indicesNormal(numIndices);

		size_t i;
		float x, y, z;
		uint32 ip0, ip1, ip2;
		uint32 iuv0, iuv1, iuv2;
		uint32 in0, in1, in2;
		pCurrent = pBuffer;

		while (pCurrent <= pEnd)
		{
			if (pCurrent[0] == 'v')
			{
				if (pCurrent[1] == ' ')
				{
					// Position
					if (sscanf(pCurrent, "v %f %f %f", &x, &y, &z) == 3)
					{
						positions.PushBack(vec3(x, y, z));
					}
				}
				else if (pCurrent[1] == 't')
				{
					// UV
					if (sscanf(pCurrent, "vt %f %f", &x, &y) == 2)
					{
						uvs.PushBack(vec2(x, y));
					}
				}
				else if (pCurrent[1] == 'n')
				{
					// Normal
					if (sscanf(pCurrent, "vn %f %f %f", &x, &y, &z) == 3)
					{
						normals.PushBack(vec3(x, y, z));
					}
				}
			}
			else if (pCurrent[0] == 'f')
			{
				// Index
				if (sscanf(pCurrent, "f %u/%u/%u %u/%u/%u %u/%u/%u", &ip0, &iuv0, &in0, &ip1, &iuv1, &in1, &ip2, &iuv2, &in2) == 9)
				{
					indicesPos.PushBack(ip0 - 1);
					indicesPos.PushBack(ip1 - 1);
					indicesPos.PushBack(ip2 - 1);

					indicesUV.PushBack(iuv0 - 1);
					indicesUV.PushBack(iuv1 - 1);
					indicesUV.PushBack(iuv2 - 1);

					indicesNormal.PushBack(in0 - 1);
					indicesNormal.PushBack(in1 - 1);
					indicesNormal.PushBack(in2 - 1);
				}
			}

			char* pLineEnd = strchr(pCurrent, '\n');

			if (pLineEnd)
				pCurrent = pLineEnd;

			pCurrent++;
		}

		iol_free(pBuffer);

		Hashmap<MeshVertexKey, MeshVertex> combinations;
		combinations.Create(GetHashForIndexCombination, 1024 * 1024);

		for (i = 0; i < numIndices; i++)
		{
			MeshVertexKey key;
			key.iPos = indicesPos[i];
			key.iUV = indicesUV[i];
			key.iNormal = indicesNormal[i];

			MeshVertex vertex;
			vertex.pos = positions[indicesPos[i]];
			vertex.uv = uvs[indicesUV[i]];
			vertex.normal = normals[indicesNormal[i]];
			vertex.index = 0; // will be set later

			combinations.Add(key, vertex);
		}

		this->positions.Create(combinations.GetCount());
		this->uvs.Create(combinations.GetCount());
		this->normals.Create(combinations.GetCount());
		this->indices.Create(numIndices);

		size_t vertexIndex = 0;
		Hashmap<MeshVertexKey, MeshVertex>::Iterator it;

		for (it = combinations.GetIterator(); it.IsValid(); it.Increment())
		{
			MeshVertex& vertex = it.GetValue();

			this->positions.PushBack(vertex.pos);
			this->uvs.PushBack(vertex.uv);
			this->normals.PushBack(vertex.normal);

			vertex.index = vertexIndex++;
		}

		for (i = 0; i < numIndices; i++)
		{
			MeshVertexKey key;
			key.iPos = indicesPos[i];
			key.iUV = indicesUV[i];
			key.iNormal = indicesNormal[i];
			MeshVertex* vertex = combinations.Get(key);

			this->indices.PushBack(vertex->index);
		}

		return true;
	}

	bool Mesh::WriteBinFile(const char* filePath)
	{
		return false;
	}

	bool Mesh::LoadBinFile(const char* filePath)
	{
		return false;
	}

	bool Mesh::GetTrianglesInRadius(glm::vec3 pos, float radius, Array<uint32>& outIndices)
	{
		outIndices.Create(30000);
		float radiusSqr = radius * radius;

		for (size_t i = 0; i < indices.count; i += 3)
		{
			if (outIndices.count + 3 >= outIndices.capacity)
			{
				iol_log_error("outIndices.capacity is too low!");
				return true;
			}

			vec3 v0 = positions[indices[i]];
			vec3 v1 = positions[indices[i + 1]];
			vec3 v2 = positions[indices[i + 2]];

			vec3 center = (v0 + v1 + v2) / 3.0f;

			if (glm::length2(center - pos) < radiusSqr)
			{
				outIndices.PushBack(indices[i]);
				outIndices.PushBack(indices[i + 1]);
				outIndices.PushBack(indices[i + 2]);
			}
		}

		return outIndices.count > 0;
	}
}