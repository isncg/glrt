#ifndef _GLRT_TEXTURE_
#define _GLRT_TEXTURE_
#include <glad/glad.h>
struct Texture
{
    std::string name;
    GLuint id;
    GLuint64 handle;
};
#endif