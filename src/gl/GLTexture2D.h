#pragma once

#include "GLBase.h"
#include "ContextBase.h"


class GLTexture2D : public IBindableObject, public IChangeableObject, public IGLObject
{
	GLuint glId;
	GLsizei w;
	GLsizei h;
	GLenum glInternalFormat;
	GLenum glFormat;
	GLenum glType;
	GLenum glUnit;

public:
	GLTexture2D(std::shared_ptr<ContextBase> & c, const int width, const int height, const GLint internalFormat = GL_RGBA, const GLenum format = GL_RGBA, const GLenum type = GL_UNSIGNED_BYTE);

	GLsizei getWidth() const;
	GLsizei getHeight() const;
	GLuint getId() const;
	GLenum getFormat() const;
	GLenum getType() const;

	bool setAutoMipMaps(const bool enable = false);
	bool setMagMinFilter(const GLenum magfilter = GL_LINEAR, const GLenum minfilter = GL_LINEAR);
	bool setWrapST(const GLenum wraps = GL_CLAMP_TO_EDGE, const GLenum wrapt = GL_CLAMP_TO_EDGE);
	bool setPixels(const GLvoid * pixels = nullptr, const GLint level = 0, const GLsizei width = -1, GLsizei height = -1);

	bool bind(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	bool unbind(std::shared_ptr<ParameterBase> parameter = nullptr) override;

	~GLTexture2D();
};

//------------------------------------------------------------------------------------------------------

class GLTextureException : public GLException
{
public:
	GLTextureException(const char * errorString) throw();
	GLTextureException(const std::string & errorString) throw();
};
