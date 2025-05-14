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