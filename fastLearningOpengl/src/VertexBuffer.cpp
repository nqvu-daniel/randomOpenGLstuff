#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* dataptr, unsigned int size)
{
    glCall(glGenBuffers(1, &m_RendererID));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    glCall(glBufferData(GL_ARRAY_BUFFER, size, dataptr, GL_STATIC_DRAW));

}

VertexBuffer::~VertexBuffer()
{
    glCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::BufferSubData(void* data, unsigned int size)
{
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    glCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}