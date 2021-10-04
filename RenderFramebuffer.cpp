#include "RenderFramebuffer.h"

#include <glad/gl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


RenderFramebuffer* RenderFramebuffer::Create(unsigned int width, unsigned int height)
	{
    RenderFramebuffer* rfb = new RenderFramebuffer();

    rfb->Width = width;
    rfb->Height = height;

    // framebuffer configuration
    glGenFramebuffers(1, &rfb->FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, rfb->FramebufferID);
    
    // create a color attachment texture
    glGenTextures(1, &rfb->ColorTexturebufferID);
    glBindTexture(GL_TEXTURE_2D, rfb->ColorTexturebufferID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rfb->Width, rfb->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rfb->ColorTexturebufferID, 0);

    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rfb->DepthRenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, rfb->DepthRenderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, rfb->Width, rfb->Height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rfb->DepthRenderbufferID); // now actually attach it
                                                                               // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        exit(-200);
        }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return rfb;
	}

void RenderFramebuffer::BindAsDestination()
	{
    // render
    glBindFramebuffer(GL_FRAMEBUFFER, this->FramebufferID);

    glViewport(0, 0, this->Width, this->Height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}

RenderFramebuffer::~RenderFramebuffer()
	{

	}
