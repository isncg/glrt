#include <Windows.h>
#include "../include/Texture.h"
#include <freeimage/FreeImage.h>
#include <utils/utils.h>
#pragma comment(lib, "FreeImage.lib")
template <class T> void INPLACESWAP(T& a, T& b) {
    a ^= b; b ^= a; a ^= b;
}
static BOOL SwapRedBlue32(FIBITMAP* dib) {
    if (FreeImage_GetImageType(dib) != FIT_BITMAP) {
        return FALSE;
    }

    const unsigned bytesperpixel = FreeImage_GetBPP(dib) / 8;
    if (bytesperpixel > 4 || bytesperpixel < 3) {
        return FALSE;
    }

    const unsigned height = FreeImage_GetHeight(dib);
    const unsigned pitch = FreeImage_GetPitch(dib);
    const unsigned lineSize = FreeImage_GetLine(dib);

    BYTE* line = FreeImage_GetBits(dib);
    for (unsigned y = 0; y < height; ++y, line += pitch) {
        for (BYTE* pixel = line; pixel < line + lineSize; pixel += bytesperpixel) {
            INPLACESWAP(pixel[0], pixel[2]);
        }
    }

    return TRUE;
}

void Texture::Load(std::string&& filepath)
{
    GLuint oldid = id;
    //image format
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    //pointer to the image, once loaded
    FIBITMAP* dib(0);
    //pointer to the image data
    BYTE* bits(0);
    //image width and height
    unsigned int width(0), height(0);
    //OpenGL's image ID to map to
    GLuint gl_texID = 0;
    GLuint64 gl_texHandle = 0;
    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(filepath.c_str(), 0);
    //if still unknown, try to guess the file format from the file extension
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filepath.c_str());
    //if still unkown, return failure
    if (fif == FIF_UNKNOWN)
    {
        auto err = string_format("Unknown texture file format: %s", filepath.c_str());
        log(err.c_str(), LogLevel::Error);
        //MessageBoxA(NULL, err.c_str(), "Resource error", MB_ICONERROR);
        name = err;
        return;
    }

    //check that the plugin has reading capabilities and load the file
    if (FreeImage_FIFSupportsReading(fif))
    {
        int retry = 5;
		while (retry-- > 0)
        {
            dib = FreeImage_Load(fif, filepath.c_str());
            if (dib)
                break;
            Sleep(200);
        }
    }
    //if the image failed to load, return failure
    if (!dib)
    {
        auto err = string_format("Failed to load texture: %s", filepath.c_str());
        log(err.c_str(), LogLevel::Error);
        name = err;
        return;
    }
    SwapRedBlue32(dib);
    //retrieve the image data
    bits = FreeImage_GetBits(dib);
    //get the image width and height
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    int bpp = FreeImage_GetBPP(dib);
    //if this somehow one of these failed (they shouldn't), return failure
    if ((bits == 0) || (width == 0) || (height == 0))
        return;

    //if this texture ID is in use, unload the current texture
   /* if (m_texID.find(texID) != m_texID.end())
        glDeleteTextures(1, &(m_texID[texID]));*/

        //generate an OpenGL texture ID for this texture
    glGenTextures(1, &gl_texID);
    //store the texture ID mapping
    //m_texID[texID] = gl_texID;
    //bind to the new texture ID
    glBindTexture(GL_TEXTURE_2D, gl_texID);
    //store the texture data for OpenGL use
    GLenum format = GL_RGB;
    GLint internalformat = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;

    if (bpp % 3 == 0)
    {
        format = GL_RGB;
        if (bpp / 3 == 8)
        {
            type = GL_UNSIGNED_BYTE;
            internalformat = GL_RGB;
        }
        else if (bpp / 3 == 16)
        {
            type = GL_UNSIGNED_SHORT;
            internalformat = GL_RGB16F;
        }
    }
    else if (bpp <= 16)
    {
        format = GL_RED;
        if (bpp == 16)
        {
            internalformat = GL_LUMINANCE16F_ARB;
            type = GL_UNSIGNED_SHORT;
        }
        else
        {
            internalformat = GL_RED;
            type = GL_UNSIGNED_BYTE;
        }
    }
    else if (bpp % 32 == 0)
    {
        format = GL_RGBA;
        internalformat = GL_RGBA;
    }
   
    GLASSERT(glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, bits));
    //if (NULL == sampler)
    //{
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_texHandle = glGetTextureHandleARB(gl_texID);
    //}
    glGenerateMipmap(GL_TEXTURE_2D);
    //Free FreeImage's copy of the data
    FreeImage_Unload(dib);
    id = gl_texID;
    handle = gl_texHandle;
    name = filepath;
    //return true;

    if (oldid > 0)
        glDeleteTextures(1, &oldid);
    this->filepath = filepath;
    ResourceMonitor::Instance().Watch(filepath, this);
}
void Texture::OnResourceUpdated()
{
    Load(std::move(filepath));
}