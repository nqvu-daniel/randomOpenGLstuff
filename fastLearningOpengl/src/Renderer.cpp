#include "Renderer.h"
#include <iostream>

void glClearError(){
    while(glGetError()!= GL_NO_ERROR);
}

bool glLogCall(){
    while(GLenum error = glGetError()){
        std::cerr << "\n[OpenGL Error] (" << error << ")\n"
                  << "----------------------------------------\n";
        return false;
    }
    return true;
}


void Renderer::Clear() const{
    glCall(glClear(GL_COLOR_BUFFER_BIT));
}
void Renderer::Draw(const VertexArray& va, const ElementIndexBuffer& ib, const Shader& shader) const{

            shader.Bind();
            va.Bind();
            ib.Bind();
            glCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}