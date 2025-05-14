#pragma once
#include <cassert>
#include <glad/glad.h>

#ifdef DEBUG
    #define glCall(x) glClearError(); x; assert(glLogCall())
#else
    #define glCall(x) x
#endif
// error checking code

void glClearError();
bool glLogCall();