#pragma once

class VertexBuffer{
    private:
        unsigned int m_RendererID; // vertex buffer id cuz opengl need some numeric id to keep track of every object we create | UNIQUE that identifies the object
                                   // internal renderer ID
    public:
        VertexBuffer(const void* dataptr, unsigned int size); // constructor
        ~VertexBuffer(); // destructor
        void Bind() const; // bind the vertex buffer
        void Unbind() const; // unbind the vertex buffer
        void BufferSubData(void* data, unsigned int size); // set the data of the vertex buffer


};