#ifndef IOLITE_GRAPHICS_BASE_H
#define IOLITE_GRAPHICS_BASE_H

#include "iol_graphics.h"

namespace iol
{
	struct VertexAttribute
	{
		VertexAttributeParam param;
		size_t offset; // calculated automatically by adding the size of basicType and dimension per vertexBufferIndex each iteration
	};

	struct VertexLayoutBase
	{
		VertexAttribute* vertexAttributes;
		size_t numVertexAttributes;
		uint32* strides; // stride for each vertex buffer
		size_t numVertexBuffers;
	};

	void graphicsBase_CreateVertexLayout(VertexLayoutBase* layout, const VertexAttributeParam* vertexAttributeParams, size_t numVertexAttributes, uint32(*fGetTypeSize)(VertexType type));
	void graphicsBase_DestroyVertexLayout(VertexLayoutBase* layout);
}

#endif // IOLITE_GRAPHICS_BASE_H