#include "TextureFBO.h"
#include "GLUtils/GLUtils.hpp"


TextureFBO::TextureFBO(unsigned int width, unsigned int height, unsigned int targets, int format) {
	this->width = width;
	this->height = height;

    texture = (GLuint*)calloc(targets, sizeof(GLuint));
    
    CHECK_GL_ERRORS();
    
	// Initialize Texture
	glGenTextures(targets, texture);
    
    for (size_t i = 0; i < targets; i++) {
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    }

    CHECK_GL_ERRORS();
    
	//Create depth bufferGLuint rboId;
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

    CHECK_GL_ERRORS();
    
	// Create FBO and attach buffers
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    for (size_t i = 0; i < targets; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+(GLenum)i, GL_TEXTURE_2D, texture[i], 0);
    }
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
	
	CHECK_GL_ERRORS();
	CHECK_GL_FBO_COMPLETENESS();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TextureFBO::~TextureFBO() {
	glDeleteFramebuffersEXT(1, &fbo);
}

void TextureFBO::bind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);
}

void TextureFBO::unbind() {
	glBindFramebufferEXT(GL_FRAMEBUFFER, 0);
}