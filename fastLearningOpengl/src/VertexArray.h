#pragma once

#include "VertexBuffer.h"

class VertexBufferLayout; // forward declaration to avoid circular dependency

class VertexArray{
private:
    unsigned int m_RendererID;
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vbl);

};