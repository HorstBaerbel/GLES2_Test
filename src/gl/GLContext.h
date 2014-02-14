#pragma once

#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
#elif defined(__linux__)
	#include<X11/X.h>
	#include<X11/Xlib.h>
	#include<GL/glx.h>
#endif

#include <GL/gl.h>

#include "ContextBase.h"

/*!
Desktop OpenGL context class. Uses WGL on Windows and GLX/X11 on Linux.
*/
class GLContext : public ContextBase
{
#if defined(WIN32) || defined(_WIN32)
    HGLRC hRC; //!<OpenGL rendering-context for surface
    HDC hDC;   //!<DC of window (if rendering to a window)
#elif defined(__linux__)
	Display * xDisplay; //!<the X11 display we're rendering to
	Window xWindow; //!<the X11 window we're rendering to
	GLXContext context; //!<The GLX context used for rendering
	int glxVersionMajor; //!<The major version number of the GLX context aquired.
	int glxVersionMinor; //!<The minor version number of the GLX context aquired.

    static int (*X11OriginalHandler)(Display *, XErrorEvent *); //!<The original X11 error handler used before we set out own.
    static int X11ErrorHandler(Display * display, XErrorEvent * event); //!<Replacement error handler when calling glXCreateContextAttribs.
#endif

public:
#if defined(WIN32) || defined(_WIN32)
	/*!
	Create Desktop OpenGL context.
	\param[in] dc Device context of window to render to. This can be a real window or fullscreen.
	\todo Throw exception if context creation fails.
	*/
	GLContext(HDC dc);
#elif defined(__linux__)
	/*!
	Create Desktop OpenGL context.
	\param[in] display Display handle to create context for.
	\param[in] display Device context of window to render to. This can be a real window or fullscreen.
	\param[in] fbConfig Framebuffer configuration of context.
	\todo Throw exception if context creation fails.
	*/
	GLContext(Display * display, Window & window, GLXFBConfig & fbConfig);
#endif

	virtual bool makeCurrent() override;
	virtual void destroy() override;

	virtual bool isDirect() const override;
	virtual bool isValid() const override;

	virtual ~GLContext();
};
