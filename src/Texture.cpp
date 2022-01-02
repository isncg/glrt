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

struct ImageLoader
{
    std::string name;
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP* dib = 0;
    BYTE* bits = 0;
    int bpp = 0;
    unsigned int width = 0, height = 0;
    bool loaded = false;

    GLenum format = GL_RGB;
    GLint internalformat = GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;

    void Load(std::string&& filepath)
    {
        name = filepath;
        loaded = false;
        //check the file signature and deduce its format
        fif = FreeImage_GetFileType(filepath.c_str(), 0);
        //if still unknown, try to guess the file format from the file extension
        if (fif == FIF_UNKNOWN)
            fif = FreeImage_GetFIFFromFilename(filepath.c_str());

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
        bpp = FreeImage_GetBPP(dib);
        //if this somehow one of these failed (they shouldn't), return failure
        if ((bits == 0) || (width == 0) || (height == 0))
            return;
        loaded = true;


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

    }

    void Unload()
    {
        FreeImage_Unload(dib);
    }
};



void Texture::Load(std::string&& filepath)
{
	GLuint oldid = id;
	GLuint gl_texID = 0;
	GLuint64 gl_texHandle = 0;
	ImageLoader loader;
	loader.Load(std::move(filepath));
	if (!loader.loaded)
		return;

	glGenTextures(1, &gl_texID);
	glBindTexture(GL_TEXTURE_2D, gl_texID);


	GLASSERT(glTexImage2D(GL_TEXTURE_2D, 0, loader.internalformat, loader.width, loader.height, 0, loader.format, loader.type, loader.bits));
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gl_texHandle = glGetTextureHandleARB(gl_texID);
	glGenerateMipmap(GL_TEXTURE_2D);

	loader.Unload();
	id = gl_texID;
	handle = gl_texHandle;
	name = filepath;
    target = GL_TEXTURE_2D;
	if (oldid > 0)
		glDeleteTextures(1, &oldid);
	this->filepath = filepath;
	ResourceMonitor::Instance().Watch(filepath, this);
}
void Texture::LoadCube(std::vector<std::string> filepath)
{
    //glEnable(GL_TEXTURE_CUBE_MAP);
    GLuint oldid = id;
    GLuint gl_texID = 0;
    GLuint64 gl_texHandle = 0;

    if (filepath.size() < 6)
        return;
    ImageLoader loader[6];
    for (int i = 0; i < 6; i++)
        loader[i].Load(std::move(filepath[i]));
    bool failed = false;
    for (int i = 0; i < 6; i++)
    {
        if (!loader[i].loaded)
        {
            failed = true;
            break;
        }
    }

    if (!failed)
    {
        for (int i = 1; i < 6; i++)
        {
            if (loader[i].format != loader[0].format ||
                loader[i].internalformat != loader[0].internalformat ||
                loader[i].bpp != loader[0].bpp ||
                loader[i].type != loader[0].type ||
                loader[i].width !=loader[0].width ||
                loader[i].height != loader[0].height)
            {
                failed = true;
                break;
            }
        }
    }

    if (!failed)
    {
        GLASSERT(glGenTextures(1, &gl_texID));
        GLASSERT(glBindTexture(GL_TEXTURE_CUBE_MAP, gl_texID));
        //GLASSERT(glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, loader[0].internalformat, loader[0].width, loader[0].height));
		for (int i = 0; i < 6; i++)
		{
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, loader[0].internalformat, loader[0].width, loader[0].height, 0,
                loader[0].format, loader[0].type, loader[i].bits);
           /* GLASSERT(glTexSubImage3D(
				GL_TEXTURE_CUBE_MAP, 0,
				0, 0, i,
				loader[0].width, loader[0].height, 1,
				loader[0].format, loader[0].type,
				loader[i].bits
			));*/
		}

        //GLASSERT(glTexImage2D(GL_TEXTURE_2D, 0, loader.internalformat, loader.width, loader.height, 0, loader.format, loader.type, loader.bits));
        GLASSERT(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLASSERT(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLASSERT(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLASSERT(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLASSERT(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
        gl_texHandle = glGetTextureHandleARB(gl_texID);
        //GLASSERT(glGenerateMipmap(GL_TEXTURE_2D));
    }

    for (int i = 0; i < 6; i++)
    {
        loader[i].Unload();
    }

    id = gl_texID;
    handle = gl_texHandle;
    name = filepath[0];
    target = GL_TEXTURE_CUBE_MAP;
}
void Texture::OnResourceUpdated()
{
    Load(std::move(filepath));
}