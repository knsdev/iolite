#include "graphics_base.h"
#include "iol_debug.h"

namespace iol
{
	static void FillVertexAttribute(VertexAttribute* pAttribute, const VertexAttributeParam& param, size_t offset)
	{
		pAttribute->param.semantic = param.semantic;
		pAttribute->param.type = param.type;
		pAttribute->param.dimension = param.dimension;
		pAttribute->param.slot = param.slot;
		pAttribute->param.vertexBufferIndex = param.vertexBufferIndex;
		pAttribute->offset = offset;
	}

	void graphics_base::CreateVertexLayout(VertexLayoutBase* layout, const VertexAttributeParam* attributeParams, size_t numVertexAttributes, uint32(*fGetTypeSize)(VertexType type))
	{
		iol_assert(attributeParams != nullptr);
		iol_assert(numVertexAttributes > 0);

		layout->numVertexAttributes = numVertexAttributes;
		layout->vertexAttributes = iol_alloc_array(VertexAttribute, numVertexAttributes);

		size_t highestVertexBufferIndex = 0;

		for (size_t i = 0; i < numVertexAttributes; i++)
		{
			if (attributeParams[i].vertexBufferIndex > highestVertexBufferIndex)
				highestVertexBufferIndex = attributeParams[i].vertexBufferIndex;
		}

		layout->numVertexBuffers = highestVertexBufferIndex + 1;

		layout->strides = iol_alloc_array(uint32, layout->numVertexBuffers);
		memory::FillZero(layout->strides, sizeof(*layout->strides) * layout->numVertexBuffers);

		for (size_t i = 0; i < numVertexAttributes; i++)
		{
			size_t vertexBufferIndex = attributeParams[i].vertexBufferIndex;

			FillVertexAttribute(&layout->vertexAttributes[i], attributeParams[i], layout->strides[vertexBufferIndex]);

			uint32 sizeOfType = fGetTypeSize(layout->vertexAttributes[i].param.type);
			layout->strides[vertexBufferIndex] += attributeParams[i].dimension * sizeOfType;
		}
	}

	void graphics_base::DestroyVertexLayout(VertexLayoutBase* layout)
	{
		iol_free(layout->vertexAttributes);
		iol_free(layout->strides);
	}
}