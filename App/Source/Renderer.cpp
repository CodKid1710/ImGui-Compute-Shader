#include "Renderer.h"

#include <iostream>

Texture CreateTexture(int width, int height)
{
	Texture result;
	result.Width = width;
	result.Height = height;

	glCreateTextures(GL_TEXTURE_2D, 1, &result.Handle);

	glTextureStorage2D(result.Handle, 1, GL_RGBA32F, width, height);

	glTextureParameteri(result.Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(result.Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTextureParameteri(result.Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(result.Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return result;
}

Framebuffer CreateFramebufferWithTexture(const Texture texture)
{
	Framebuffer result;

	glCreateFramebuffers(1, &result.Handle);

	if (!AttachTextureToFramebuffer(result, texture))
	{
		glDeleteFramebuffers(1, &result.Handle);
		return {};
	}

	return result;
}

bool AttachTextureToFramebuffer(Framebuffer& framebuffer, const Texture texture)
{
	glNamedFramebufferTexture(framebuffer.Handle, GL_COLOR_ATTACHMENT0, texture.Handle, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Framebuffer is not complete!" << std::endl;
		return false;
	}

	framebuffer.ColorAttachment = texture;
	return true;
}

void BlitFramebufferToSwapchain(const Framebuffer framebuffer)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.Handle);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // swapchain

	glBlitFramebuffer(0, 0, framebuffer.ColorAttachment.Width, framebuffer.ColorAttachment.Height, // Source rect
		0, 0, framebuffer.ColorAttachment.Width, framebuffer.ColorAttachment.Height,               // Destination rect
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
}