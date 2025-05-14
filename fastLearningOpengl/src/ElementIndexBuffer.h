#pragma once

class ElementIndexBuffer{
    private:
        unsigned int m_RendererID; // vertex buffer id cuz opengl need some numeric id to keep track of every object we create | UNIQUE that identifies the object
                                   // internal renderer ID
        unsigned int m_Count; // number of elements in the vertex buffer
    public:
        ElementIndexBuffer(const void* data, unsigned int count); 
        ~ElementIndexBuffer(); 
        void Bind() const; 
        void Unbind() const; 

        inline unsigned int GetCount() const { return m_Count; }
};