#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include <X11/X.h>
	#include <X11/Xlib.h>
	#ifdef USE_OPENGL_DESKTOP
		#include <GL/glx.h>
	#endif
#endif

#include "GLBase.h"

#define CHECK_OPENGL_ERRORS //Define this to check for OpenGL errors. If this is not defined glErrorOccurred() will be a nop.
#define USE_OPENGL_ERROR_CALLBACK //Define this to use the glDebugMessageCallback functionality available with OpenGL 4.3+ if available.

/*!
OpenGL context base class. Derive from this to create system specific contexts, e.g. Desktop GL or ES.
*/
class ContextBase
{
	static std::map<GLenum, std::string> m_ErrorMap; //!<Map holding human-readable string for opengl error codes.
    static std::shared_ptr<std::ostream> m_Out; //!<Output stream the error messages will be output to. Default is std::cout.
    static bool m_OutputErrors; //!<When set to true OpenGL errors are output to @sa m_Out. Default is off.

    /*
    Set the output stream all OpenGL errors go to.
    \param[in] out New output stream for OpenGL errors.
    */
	static void setErrorOutputStream(std::shared_ptr<std::ostream> out);

    /*
    Toggle OpenGL error output.
    \param[in] enable Pass true to output OpenGL errors to m_Out.
    */
    static void setOutputErrors(bool enable);

protected:
	int versionMajor; //!<OpenGL version information.
	int versionMinor; //!<OpenGL version information.
	std::vector<std::string> extensions; //!<List of OpenGL extension strings.

	/*!
	Get all extension strings from OpenGL context.
	\note Call only with valid and current context. Writes to \extensions.
	\todo Get OS-specific extensions too
	*/
	void getExtensions();

	struct Binding {
		void (GLAPIENTRYP * adressOfFunctionPointer)(); //!<Pointer to where the function pointer is stored.
		const std::string nameOfFunction;               //!<Name of the function we're trying to bind;

		Binding(void (GLAPIENTRYP * adressOfFuncPointer)(), const std::string & functionName) : adressOfFunctionPointer(adressOfFuncPointer), nameOfFunction(functionName) {	*adressOfFunctionPointer = nullptr;	}
	};
    std::vector<Binding> bindings; //!<Holds adresses of OpenGL functions and the names.

	/*!
	Get function bindings for all OpenGL functions in \bindings.
	\return Returns true if ALL functions were sucessfully bound.
	*/
	bool getBindings();

	/*!
	Constructor should only (and must) be called by derived classes.
	\note This sets up the \bindings vector.
	*/
	ContextBase();

public:
	//Function pointers for stuff missing in OS OpenGL interface
	void (GLAPIENTRYP glActiveTexture)(GLenum texture);                 /*!<Activate a specific texture unit*/
	//Function pointers to OpenGL shader functions.
	GLuint (GLAPIENTRYP glCreateShader)(GLenum shaderType);             /*!<Function prototype for shader creation.*/
	void (GLAPIENTRYP glShaderSource)(GLuint shader, GLsizei count, const GLchar **string, const GLint *length); /*!<Function prototype for shader source creation.*/
	void (GLAPIENTRYP glCompileShader)(GLuint shader);                /*!<Function prototype for shader compilation.*/
	GLuint (GLAPIENTRYP glCreateProgram)(void);                         /*!<Function prototype for shader program object creation.*/
	void (GLAPIENTRYP glAttachShader)(GLuint program, GLuint shader); /*!<Function prototype for attaching shader to program object.*/
	void (GLAPIENTRYP glLinkProgram)(GLuint program);                 /*!<Function prototype for shader compilation.*/
	void (GLAPIENTRYP glUseProgram)(GLuint program);                  /*!<Function prototype for using a shader program object.*/
	void (GLAPIENTRYP glDetachShader)(GLuint program, GLuint shader); /*!<Disconnect attached shader from program.*/
	void (GLAPIENTRYP glDeleteShader)(GLuint shader);                 /*!<Function prototype for deleting a shader source.*/
	void (GLAPIENTRYP glValidateProgram)(GLuint program);             /*!<Function prototype for validating a shader program.*/
	void (GLAPIENTRYP glDeleteProgram)(GLuint program);               /*!<Function prototype for deleting a shader program.*/
	void (GLAPIENTRYP glGetShaderiv)(GLuint shader, GLenum pname, GLint *params); /*!<Function prototype for getting parameters from a shader object.*/
	void (GLAPIENTRYP glGetShaderInfoLog)(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog); /*!<Function prototype to retrive infolog for a shader.*/
	void (GLAPIENTRYP glGetProgramiv)(GLuint program, GLenum pname, GLint *params); /*!<Function prototype for getting parameters from a program object.*/
	void (GLAPIENTRYP glGetProgramInfoLog)(GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog); /*!<Function prototype to retrive infolog for a program.*/
	//Function pointers to OpenGL shader uniform value functions.
	GLint (GLAPIENTRYP glGetUniformLocation)(GLuint program, const GLchar * name); /*!<Retrieve location of a uniform variable in a shader program.*/
	//float
	void (GLAPIENTRYP glUniform1f)(GLint location, GLfloat v0);                                     /*!<Set single uniform floating point value.*/
	void (GLAPIENTRYP glUniform2f)(GLint location, GLfloat v0, GLfloat v1);                         /*!<Set 2 uniform floating point values.*/
	void (GLAPIENTRYP glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);             /*!<Set 3 uniform floating point values.*/
	void (GLAPIENTRYP glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3); /*!<Set 4 uniform floating point values.*/
	void (GLAPIENTRYP glUniform1fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	void (GLAPIENTRYP glUniform2fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	void (GLAPIENTRYP glUniform3fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	void (GLAPIENTRYP glUniform4fv)(GLint location, GLsizei count, GLfloat *v);                     /*!<Set uniform floating point value vector.*/
	//int
	void (GLAPIENTRYP glUniform1i)(GLint location, GLint v0);                               /*!<Set single uniform int value.*/
	void (GLAPIENTRYP glUniform2i)(GLint location, GLint v0, GLint v1);                     /*!<Set 2 uniform int values.*/
	void (GLAPIENTRYP glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);           /*!<Set 3 uniform int values.*/
	void (GLAPIENTRYP glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3); /*!<Set 4 uniform int values.*/
	void (GLAPIENTRYP glUniform1iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	void (GLAPIENTRYP glUniform2iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	void (GLAPIENTRYP glUniform3iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	void (GLAPIENTRYP glUniform4iv)(GLint location, GLsizei count, GLint *v);               /*!<Set uniform int value vector.*/
	//matrix
	void (GLAPIENTRYP glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat *v); /*!<Set a 2x2 uniform floating point matrix.*/
	void (GLAPIENTRYP glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat *v); /*!<Set a 3x3 uniform floating point matrix.*/
	void (GLAPIENTRYP glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, GLfloat *v); /*!<Set a 4x4 uniform floating point matrix.*/
	//vertex attributes and arrays
	GLint (GLAPIENTRYP glGetAttribLocation)(GLuint  program, const GLchar * name);
	void (GLAPIENTRYP glBindAttribLocation)(GLuint program, GLuint index, const GLchar * name);
	void (GLAPIENTRYP glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
	void (GLAPIENTRYP glEnableVertexAttribArray)(GLuint index);
	void (GLAPIENTRYP glDisableVertexAttribArray)(GLuint index);
	void (GLAPIENTRYP glDrawArrays)(GLenum mode, GLint first, GLsizei count);
	//tesselation
	void (GLAPIENTRYP glPatchParameteri)(GLenum pname, GLint value);
	void (GLAPIENTRYP glPatchParameterfv)(GLenum pname, const GLfloat * values);
	//FBOs
	void (GLAPIENTRYP glClampColor)(GLenum  target,  GLenum  clamp);
	void (GLAPIENTRYP glGenFramebuffers)(GLsizei n, GLuint *ids);
	void (GLAPIENTRYP glDeleteFramebuffers)(GLsizei n, const GLuint *ids);
	void (GLAPIENTRYP glBindFramebuffer)(GLenum target, GLuint id);
	//void (GLAPIENTRYP glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
	void (GLAPIENTRYP glFramebufferTexture2D)(GLenum target, GLenum attachmentPoint, GLenum textureTarget, GLuint textureId, GLint  level); 
	void (GLAPIENTRYP glBlitFramebuffer)(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
	GLenum (GLAPIENTRYP glCheckFramebufferStatus)(GLenum target);
    void (GLAPIENTRYP glDiscardFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum * attachments); //only on OpenGL ES (2.0)
	//VBOs, PBOs
	void (GLAPIENTRYP glGenBuffers)(GLsizei n, GLuint *ids);
	void (GLAPIENTRYP glDeleteBuffers)(GLsizei n, const GLuint *ids);
	void (GLAPIENTRYP glBindBuffer)(GLenum target, GLuint id);
	void (GLAPIENTRYP glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
	void * (GLAPIENTRYP glMapBuffer)(GLenum target, GLenum access);
	GLboolean (GLAPIENTRYP glUnmapBuffer)(GLenum target);
	//VAOs
	void (GLAPIENTRYP glGenVertexArrays)(GLsizei n, GLuint * arrays);
	void (GLAPIENTRYP glDeleteVertexArrays)(GLsizei n, const GLuint * arrays);
	void (GLAPIENTRYP glBindVertexArray)(GLuint array);

#if defined(WIN32) || defined(_WIN32)
	//wglGetExtensionsString();
	int (GLAPIENTRYP wglGetSwapInterval)(void);
	bool (GLAPIENTRYP wglSwapInterval)(int interval);
	bool (GLAPIENTRYP wglChoosePixelFormat)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
		void (GLAPIENTRYP glXSwapInterval)(Display *dpy, GLXDrawable drawable, int interval);
		GLXContext (GLAPIENTRYP glXCreateContextAttribs)(Display *dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int *attrib_list);
    #endif
#endif

	/*!
	Make this context current so all follwing OpenGL operations use it.
	*/
	virtual bool makeCurrent();

	/*
	Destroy context. Override this with the necessary functions to tear down the context.
	*/
	virtual void destroy();

	/*!
	Return major number of the OpenGL context version.
	\return Returns the major number of the OpenGL context version.
	*/
	int getMajorVersion() const;

	/*!
	Return minor number of the OpenGL context version.
	\return Returns the minor number of the OpenGL context version.
	*/
	int getMinorVersion() const;

	/*!
	Check if an OpenGL extension is supported.
	\param[in] extensionName The name of the extension to check for (case-insensitive).
	\return Returns true if the extension is supported.
	\todo A bit of work, but it would be good to check if all necessary functions for that extension are actually bound...
	*/
	bool isExtensionAvailable(const std::string & extensionName) const;

	/*!
	Check if the context uses direct or indirect rendering.
	\return Returns true if direct rendering is used. Always true on Windows platforms.
	*/
	virtual bool isDirect() const;

	/*!
	Check if this context has been set up correctly.
	\return Returns true if the context is usable.
	*/
	virtual bool isValid() const;

	//TODO: Move those to a shader class!!!
	GLuint createShader(const std::string & vertexCode, const std::string & fragmentCode, std::string & errorString);
	GLuint createShaderFromFile(const std::string & vertexFile, const std::string & fragmentFile, std::string & errorString);
	const std::string getShaderError(const GLuint vertexHandle, const GLuint fragmentHandle, const GLuint programHandle);

	/*!
	Convert openGL error identifier to a readable representation using GLU.
	\param[in] error The OpenGL error code retrieved via a glGetError() call.
	\return Returns the string representation of the error.
	*/
    static std::string glErrorToString(GLenum error);

    /*
    Check if an OpenGL is pending and prints it to @sa m_Out if @sa m_OutputErrors is set, then removes it from the error stack.
    \return Returns true if an OpenGL error happened.
    \note Use the macro @sa glErrorOccurred.
    */
	bool glErrorHappened(const std::string file, int line);

#ifdef CHECK_OPENGL_ERRORS
    #define glErrorOccurred() (glErrorHappened(__FILE__, __LINE__))
#else
    #define glErrorOccurred() (false)
#endif

#ifdef USE_OPENGL_ERROR_CALLBACK
	static void GLAPIENTRY openglErrorCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, void * userParam);
#endif

	/*
	Destructor calls \destroy by default.
	*/
    virtual ~ContextBase();
};
