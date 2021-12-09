#ifndef _GLRT_TEXTURE_
#define _GLRT_TEXTURE_
#include <glad/glad.h>
#include "ResourceMonitor.h"
class Texture : public IResourceUpdateNotify
{
    std::string filepath;
public:
    std::string name;
    GLuint id = 0;
    GLuint64 handle = 0;
    void Load(std::string&& filepath);
    virtual void OnResourceUpdated() override;
};
#endif