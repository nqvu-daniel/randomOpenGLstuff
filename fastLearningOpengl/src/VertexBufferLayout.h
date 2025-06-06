#pragma once

#include <cassert>

#include <glad/glad.h>

#include <vector>

static unsigned int GetSizeOfType(unsigned int type)
{
	switch (type)
	{
	case GL_FLOAT:         return 4;
	case GL_UNSIGNED_INT:  return 4;
	case GL_UNSIGNED_BYTE: return 1;
	}

	assert(false);
	return 0;
}

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
};

class VertexBufferLayout
{
	unsigned int m_stride = 0;
	std::vector<VertexBufferElement> m_elements;

public:
	VertexBufferLayout() {}

	template<class T> void Push(unsigned int count); // { static_assert(false); } -> disable cuz gcc and Clang triggers static_assert

	const std::vector<VertexBufferElement> &GetElements() const& { return m_elements; }
	unsigned int GetStride() const { return m_stride; }

};

template<> inline void VertexBufferLayout::Push<float>(unsigned int count) {
     m_elements.push_back({ GL_FLOAT, count, GL_FALSE });  m_stride += GetSizeOfType(GL_FLOAT) * count; }
template<> inline void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
     m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE }); m_stride += GetSizeOfType(GL_UNSIGNED_INT) * count; }
template<> inline void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
     m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE }); m_stride += GetSizeOfType(GL_UNSIGNED_BYTE) * count; }

