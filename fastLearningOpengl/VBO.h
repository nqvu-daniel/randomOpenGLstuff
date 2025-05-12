#pragma once
#include <glad/glad.h>

class VBO
{
public:
    // ID reference for the Vertex Buffer Object
    unsigned int ID;
    // Constructor that generates a Vertex Buffer Object and links it to vertices
    VBO(GLfloat* vertices, GLsizeiptr size);

    void Bind();
    void Unbind();
    void Delete();
};
