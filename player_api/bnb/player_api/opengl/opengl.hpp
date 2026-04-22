#pragma once

#include <bnb/utils/defs.hpp>

#if BNB_OS_ANDROID
    #include <GLES3/gl3.h>
#else // other platforms
    #include <glad/glad.h>
#endif // BNB_OS_ANDROID

void __print_opengl_errors(const char* file, int line);

#if defined(_DEBUG) || defined(DEBUG) || true
    #define GL_CHECK_ERROR()                           \
        do {                                           \
            __print_opengl_errors(__FILE__, __LINE__); \
        } while (0)
#else
    #define GL_CHECK_ERROR() ((void) 0)
#endif
#define GL_CALL(FUNC)     \
    do {                  \
        FUNC;             \
        GL_CHECK_ERROR(); \
    } while (0)
