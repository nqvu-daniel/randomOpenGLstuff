#pragma once
#include <cassert>
#include <glad/glad.h>
#include "VertexArray.h"
#include "ElementIndexBuffer.h"
#include "Shader.h"

#define DEBUG
#ifdef DEBUG
    #define glCall(x) glClearError(); x; assert(glLogCall())
#else
    #define glCall(x) x
#endif
// error checking code

void glClearError();
bool glLogCall();

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& va, const ElementIndexBuffer& ib, const Shader& shader) const;
};