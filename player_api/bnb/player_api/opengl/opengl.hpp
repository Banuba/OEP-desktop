#pragma once

#if defined(__ANDROID__) // android platform
    #include <GLES3/gl3.h>
    #define BNB_GLSL_VERSION_STRING "#version 300 es \n"
#else // other platforms
    #include <glad/glad.h>
    #define BNB_GLSL_VERSION_STRING "#version 330 core \n"
#endif // defined(__ANDROID__)

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
