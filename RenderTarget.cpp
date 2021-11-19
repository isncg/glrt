#include "RenderTarget.h"
#include "utils/glhelpers.h"
#include "utils/log.h"
#include "utils/stdhelpers.h"
int RenderTarget::width()
{
	return _size.x;
}
int RenderTarget::height()
{
	return _size.y;
}
const glm::ivec2& RenderTarget::size()
{
	return _size;
}
void RenderTarget::Init(int width, int height, int nColors, bool depth)
{
	_size.x = width;
	_size.y = height;
	GLASSERT(glGenFramebuffers(1, &fbo));
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	if (nColors > 0)
	{
		GLuint* tex = new GLuint[nColors];
		glGenTextures(nColors, tex);
		//GLASSERT(glCreateTextures(GL_TEXTURE_2D, nColors, pColorTextures));
		for (int i = 0; i < nColors; i++)
		{
			glBindTexture(GL_TEXTURE_2D, tex[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, tex[i], 0);
			//GLASSERT(glTextureStorage2D(pColorTextures[i], 1, GL_RGBA8, width, height));
			//GLASSERT(glNamedFramebufferTexture2DEXT(fbo, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, pColorTextures[i], 0));
			colorTextures.push_back({ tex[i] });
		}

		delete[] tex;
	}

	if (depth)
	{
		GLuint tex;
		glGenTextures(nColors, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
		depthTexture.id = tex;
	}
	auto err = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
	{
		std::vector<const char*> errs{
			"GL_FRAMEBUFFER_COMPLETE 0x8CD5",
			"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6",
			"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7",
			"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB",
			"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC",
			"GL_FRAMEBUFFER_UNSUPPORTED 0x8CDD"
		};
		log(string_format("Frame buffer incomplete: %d %x, %s}", fbo, err, errs[err - GL_FRAMEBUFFER_COMPLETE]).c_str(), LogLevel::Error);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::Bind(int passID)
{
	GLASSERT(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	glViewport(0, 0, _size.x, _size.y);
	this->passID = passID;
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(clearBit);
}

void RenderTarget::Draw(Renderer& renderer)
{
	renderer.Draw(passID);
}
