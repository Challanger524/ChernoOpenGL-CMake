#pragma once

#include "Utility.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

#include <GL/glew.h>

#include <vector>

class VertexArray
{
	unsigned int m_rendererId;

public:
	VertexArray() { GLCall(glGenVertexArrays(1, &m_rendererId)); }
	~VertexArray() { GLCall(glDeleteVertexArrays(1, &m_rendererId)); }

	void Bind() const { GLCall(glBindVertexArray(m_rendererId)); }
	void Unbind() const { GLCall(glBindVertexArray(0)); }

	void AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) {
		Bind();
		vb.Bind();
		const std::vector<VertexBufferElement> &elements = layout.GetElements();
		size_t offset = 0; // suppress warning C4312: 'reinterpret_cast': conversion from 'unsigned int' to 'const void *' of greater size
		for (unsigned int i = 0; i < elements.size(); i++)
		{
			const auto &element = elements[i];
			GLCall(glEnableVertexAttribArray(i));
			GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized,
										 layout.GetStride(), reinterpret_cast<const void *>(offset))); // links `vertex-buffer` to `vao`
			offset += size_t(element.count) * GetSizeOfType(element.type);
		}
	}
};
