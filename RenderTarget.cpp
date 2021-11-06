#include "RenderTarget.h"

void RenderTarget::Init(int width, int height, int nColors, bool depth)
{
	glGenFramebuffers(1, &fbo);
	if (nColors > 0)
	{
		GLuint* pColorTextures = new GLuint[nColors];
		glCreateTextures(GL_TEXTURE_2D, nColors, pColorTextures);
		for (int i = 0; i < nColors; i++)
		{
			glTextureStorage2D(pColorTextures[i], 0, GL_RGBA8, width, height);
			glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + i, pColorTextures[i], 0);
			colorTextures.push_back({ pColorTextures[i] });
		}

		delete[] pColorTextures;
	}

	if (depth)
	{
		GLuint depthTexture;
		glCreateTextures(GL_TEXTURE_2D, 1, &depthTexture);
		glTextureStorage2D(depthTexture, 0, GL_R16F, width, height);
		glNamedFramebufferTexture(fbo, GL_DEPTH_ATTACHMENT, depthTexture, 0);
	}
}

void RenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}
