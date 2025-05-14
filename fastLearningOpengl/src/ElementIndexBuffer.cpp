#include "ElementIndexBuffer.h"
#include "Renderer.h"

ElementIndexBuffer::ElementIndexBuffer(const void* data, unsigned int count) : m_Count(count)  
{
    assert(sizeof(unsigned int) == sizeof(GLuint)); // make sure 
    glCall(glGenBuffers(1, &m_RendererID));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));

}

ElementIndexBuffer::~ElementIndexBuffer()
{
    glCall(glDeleteBuffers(1, &m_RendererID));
}

void ElementIndexBuffer::Bind() const
{
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void ElementIndexBuffer::Unbind() const
{
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}