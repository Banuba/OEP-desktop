#pragma once

#if defined(__ANDROID__) /* android platform */
    #include <GLES3/gl3.h>
    #define BNB_GLSL_VERSION_STRING "#version 300 es \n"
#else /* other platform */
    #include <glad/glad.h>
    #define BNB_GLSL_VERSION_STRING "#version 330 core \n"
#endif /* defined(__ANDROID__) */

#define GL_CHECK_ERROR() ((void) 0)
#define GL_CALL(FUNC) [&]() {FUNC; GL_CHECK_ERROR(); }()
