#include "GLTexture2D.h"

#include <iostream>


GLTexture2D::GLTexture2D(std::shared_ptr<ContextBase> & c, int width, int height, const GLint internalFormat, const GLenum format, const GLenum type)
    : IGLObject(c), glId(0), w(-1), h(-1), glInternalFormat(GL_NONE), glFormat(GL_NONE), glType(GL_NONE), glUnit(GL_NONE)
{
    //make our context current
    glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
    //push all enable attributes. OpenGL ES doesn't have those functions...
    glPushAttrib(GL_ENABLE_BIT);
    //enable 2D texturing and disable rectangle texturing, becaue it causes errors
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_RECTANGLE);
#endif
    //check if the texture size is exceeded
    GLint maxTextureDim = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureDim);
    if (width > maxTextureDim || height > maxTextureDim) {
        //size exceeded. resize, but keep aspect
        std::cout << "Maximum OpenGL texure size of " << maxTextureDim << " exceeded. Resizing texture." << std::endl;
        const float aspect = (float)width / (float)height;
        if (width > height) {
            width = maxTextureDim;
            height = (int)((float)maxTextureDim / aspect);
        }
        else {
            height = maxTextureDim;
            width = (int)((float)maxTextureDim * aspect);
        }
    }
    //create texture id
    GLuint textureId;
    glGenTextures(1, &textureId);
    //bind texture
    glBindTexture(GL_TEXTURE_2D, textureId);
    //create textuere without content
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
    //set up texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
#ifdef USE_OPENGL_DESKTOP
    //special setup needed for a depth texture. OpenGL ES does not support GL_DEPTH_TEXTURE_MODE and always uses GL_LUMINANCE!
    if (GL_DEPTH_COMPONENT == format) {
        //sample like regular texture, value is in alpha channel
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    }
#endif
    //unbind the texture again
    glBindTexture(GL_TEXTURE_2D, 0);
#ifdef USE_OPENGL_DESKTOP
    //restore enabled attributes
    glPopAttrib();
#endif
    //check for errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cout << "Error 0x" << std::hex << error << " creating 2D texture!" << std::endl;
        //delete texture again
        glDeleteTextures(1, &textureId);
        return;
    }
    //set object values
    glId = textureId;
    w = width;
    h = height;
    glInternalFormat = internalFormat;
    glFormat = format;
    glType = type;
    valid = true;
}

GLsizei GLTexture2D::getWidth() const
{
    return w;
}

GLsizei GLTexture2D::getHeight() const
{
    return h;
}

GLuint GLTexture2D::getId() const
{
    return glId;
}

GLenum GLTexture2D::getFormat() const
{
    return glFormat;
}

GLenum GLTexture2D::getType() const
{
    return glType;
}

bool GLTexture2D::setAutoMipMaps(const bool enable)
{
    if (glId > 0) {
        glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
        //push all enable attributes. OpenGL ES doesn't have those functions...
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_TEXTURE_2D);
#endif
        glBindTexture(GL_TEXTURE_2D, glId);
//		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, enable ? GL_TRUE : GL_FALSE);
        glBindTexture(GL_TEXTURE_2D, 0);
#ifdef USE_OPENGL_DESKTOP
        //restore enabled attributes
        glPopAttrib();
#endif
        //mark as changed
        changed = true;
        return true;
    }
    return false;
}

bool GLTexture2D::setMagMinFilter(const GLenum magfilter, const GLenum minfilter)
{
    if (glId > 0) {
        glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
        //push all enable attributes. OpenGL ES doesn't have those functions...
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_TEXTURE_2D);
#endif
        glBindTexture(GL_TEXTURE_2D, glId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magfilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minfilter);
        glBindTexture(GL_TEXTURE_2D, 0);
#ifdef USE_OPENGL_DESKTOP
        //restore enabled attributes
        glPopAttrib();
#endif
        //mark as changed
        changed = true;
        return true;
    }
    return false;
}

bool GLTexture2D::setWrapST(const GLenum wraps, const GLenum wrapt)
{
    if (glId > 0) {
        glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
        //push all enable attributes. OpenGL ES doesn't have those functions...
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_TEXTURE_2D);
#endif
        glBindTexture(GL_TEXTURE_2D, glId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
        glBindTexture(GL_TEXTURE_2D, 0);
#ifdef USE_OPENGL_DESKTOP
        //restore enabled attributes
        glPopAttrib();
#endif
        //mark as changed
        changed = true;
        return true;
    }
    return false;
}

bool GLTexture2D::setPixels(const Image & image, const GLint level)
{
    if (glId > 0) {
        Image sourceImage;
        //check if we need to resize the texture
        if (w != image.width() || h != image.height()) {
            sourceImage = image.scaled(w, h);
        }
        else {
            sourceImage = image;
        }
        return setPixels(sourceImage.pixels(), level, w, h);
    }
    return false;
}

bool GLTexture2D::setPixels(const GLvoid * pixels, const GLint level, const GLsizei width, GLsizei height)
{
    if (glId > 0) {
        glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
        //push all enable attributes. OpenGL ES doesn't have those functions...
        glPushAttrib(GL_ENABLE_BIT);
        glEnable(GL_TEXTURE_2D);
#endif
        glBindTexture(GL_TEXTURE_2D, glId);
        if (level == 0 || width <= 0 || height <= 0) {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, glFormat, glType, pixels);
        }
        else {
            glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, width, height, glFormat, glType, pixels);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
#ifdef USE_OPENGL_DESKTOP
        //restore enabled attributes
        glPopAttrib();
#endif
        //mark as changed
        changed = true;
        //check for errors
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "Error 0x" << std::hex << error << " updating 2D texture " << glId << "!"<< std::endl;
            valid = false;
            return false;
        }
        //TODO: Re-generate mipmaps?!
        return true;
    }
    return false;
}

bool GLTexture2D::bind(const Parameter<GLenum> & parameter)
{
    if (glId > 0) {
        glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
        glEnable(GL_TEXTURE_2D);
#endif
        glContext->glActiveTexture(parameter);
        glBindTexture(GL_TEXTURE_2D, glId);
        glUnit = parameter;
        changed = false;
        return true;
    }
    return false;
}

bool GLTexture2D::unbind()
{
    if (glId > 0) {
        if (glUnit != GL_NONE) {
            glContext->makeCurrent();
#ifdef USE_OPENGL_DESKTOP
            glEnable(GL_TEXTURE_2D);
#endif
            glContext->glActiveTexture(glUnit);
            glBindTexture(GL_TEXTURE_2D, 0);
            glUnit = GL_NONE;
            return true;
        }
        else {
            //print an error, but still return true, because the outcome is ok...
            std::cout << "Texture " << glId << " was not bound before!" << std::endl;
            return true;
        }
    }
    return false;
}

GLTexture2D::~GLTexture2D()
{
    if (glId > 0) {
        glContext->makeCurrent();
        glDeleteTextures(1, &glId);
        glId = 0;
        w = -1;
        h = -1;
        glInternalFormat = GL_NONE;
        glFormat = GL_NONE;
        glType = GL_NONE;
        glUnit = GL_NONE;
    }
}

//------------------------------------------------------------------------------------------------------

GLTextureException::GLTextureException(const char * errorString) throw()
    : GLException(errorString)
{
}

GLTextureException::GLTextureException(const std::string & errorString) throw()
    : GLException(errorString)
{
}
