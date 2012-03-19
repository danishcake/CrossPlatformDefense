#pragma once

#define GL_CHECK(x) \
        x; \
        { \
          GLenum glError = glGetError(); \
          if(glError != GL_NO_ERROR) { \
            Log::Error(__FILE__, "glGetError() = %i (0x%.8x) at %s:%i\n", glError, glError, __FILE__, __LINE__); \
          } \
        }

#define EGL_CHECK(x) \
    x; \
    { \
        EGLint eglError = eglGetError(); \
        if(eglError != EGL_SUCCESS) { \
            Log::Error(__FILE__, "eglGetError() = %i (0x%.8x) at line %i\n", eglError, eglError, __FILE__, __LINE__); \
        } \
    }
