#include "GLContext.h"

#include <iostream>


int (*GLContext::X11OriginalHandler)(Display *, XErrorEvent *) = nullptr;


#if defined(WIN32) || defined(_WIN32)
GLContext::GLContext(HDC dc)
	: hDC(nullptr), hRC(nullptr)
{
	//create OpenGL context
	HGLRC rc = wglCreateContext(dc);
	if (rc == nullptr) {
		destroy();
		std::cout << "Failed to create a render context!" << std::endl;
		return;
	}
	//make context current
	if(!wglMakeCurrent(dc, rc)) {
		destroy();
        std::cout << "Failed to make render context current!" << std::endl;
		return;
	}
	//get extensions
	getExtensions();
	//get function bindings here
	if (!getBindings()) {
        std::cout << "Failed to get all function bindings!" << std::endl;
	}
	//setup members
	hDC = dc;
	hRC = rc;
}

#elif defined(__linux__)

GLContext::GLContext(Display * display, Window & window, GLXFBConfig & fbConfig)
	: xDisplay(nullptr), xWindow(0), context(nullptr), glxVersionMajor(1), glxVersionMinor(0)
{
	//framebuffer configs were added in GLX version 1.3 so check for that
	if (!glXQueryVersion(display, &glxVersionMajor, &glxVersionMinor))	{
		std::cout << "Failed to get the GLX version!" << std::endl;
		return;
	}
	if (glxVersionMajor < 1 || (glxVersionMajor == 1 && glxVersionMinor < 3)) {
		std::cout << "GLX version 1.3 or higher needed!" << std::endl;
		return;
	}
	//try to create an OpenGL <= 2.1 context first
	GLXContext gl2Context = glXCreateNewContext(display, fbConfig, GLX_RGBA_TYPE, nullptr, GL_TRUE);
	if (gl2Context == nullptr) {
		std::cout << "Failed to create a render context!" << std::endl;
		return;
	}
    else {
        std::cout << "Created default OpenGL context." << std::endl;
    }
	//get function bindings now. we need this for glXCreateContextAttribs
	if (!getBindings()) {
		std::cout << "Failed to get all function bindings!" << std::endl;
	}
	//check if glXCreateContextAttribs is available
	if (glXCreateContextAttribs != nullptr) {
        //sync to Xserver first
        XSync(display, False);
        //set up an X error handler, because glXCreateContextAttribs might b0rk out
        X11OriginalHandler = XSetErrorHandler(X11ErrorHandler);
		//create a GL >= 3.0 context
        //TODO: GLX_CONTEXT_DEBUG_BIT_ARB
		int attribs[] = { GLX_CONTEXT_MAJOR_VERSION_ARB, 3, GLX_CONTEXT_MINOR_VERSION_ARB, 0, None};
		GLXContext gl3Context = glXCreateContextAttribs(display, fbConfig, nullptr, True, &attribs[0]);
        //reset error handler first
        XSetErrorHandler(X11OriginalHandler);
		//if that worked, destroy the 2.x context
		if (gl3Context != nullptr) {
            std::cout << "Created OpenGL 3.x context. Destroying old one." << std::endl;
			glXDestroyContext(display, gl2Context); //We can destroy the GL 2.x context once the 3.0 one has been created
            context = gl3Context;
		}
        else {
            std::cout << "Failed to create an OpenGL 3.x context. Using OpenGL 2.x context." << std::endl;
            context = gl2Context;
        }
		//get function bindings again now, as we have a new context
		if (!getBindings()) {
			std::cout << "Failed to get all function bindings!" << std::endl;
		}
	}
	else {
		context = gl2Context;
	}
	//try to make render context current
	if (!glXMakeCurrent(display, window, context)) {
        std::cout << "Failed to make render context current!" << std::endl;
		destroy();
		return;
	}
	//get extensions
	getExtensions();
	//setup members
	xDisplay = display;
	xWindow = window;
}

int GLContext::X11ErrorHandler(Display * display, XErrorEvent * event)
{
    switch (event->error_code) {
        case GLXBadContext:
        case GLXBadFBConfig:
        case GLXBadProfile:
        case BadRequest:
        case BadMatch:
        case BadValue:
        case BadAlloc:
            //ignore those errors as the came from glXCreateContextAttribs and can be ignored...
            return 0;
        default:
            //delegate all other errors to the original handler...
            return 0;//(X11OriginalHandler(display, event));
    }
}
#endif

bool GLContext::makeCurrent()
{
#if defined(WIN32) || defined(_WIN32)
	if(!wglMakeCurrent(hDC, hRC)) {
#elif defined(__linux__)
	if (!glXMakeCurrent(xDisplay, xWindow, context)) {
#endif
		std::cout << "Failed to make render context current!" << std::endl;
		return false;
	}
	return true;
}

void GLContext::destroy()
{
#if defined(WIN32) || defined(_WIN32)
	if (hRC != nullptr) {
		//Try to release the rendering context. HDC is ignored in this case
		wglMakeCurrent(nullptr, nullptr);
		//Try to delete the rendering context
		wglDeleteContext(hRC);
		hRC = nullptr;
	}
#elif defined(__linux__)
	if (context != nullptr) {
		//make no context current
		glXMakeCurrent(xDisplay, None, nullptr);
		//destroy OpenGL context
		glXDestroyContext(xDisplay, context);
		context = nullptr;
	}
#endif
}

bool GLContext::isDirect() const
{
#if defined(WIN32) || defined(_WIN32)
	return true;
#elif defined(__linux__)
	return glXIsDirect(xDisplay, context);
#endif
}

bool GLContext::isValid() const
{
#if defined(WIN32) || defined(_WIN32)
	return (hDC != nullptr && hRC != nullptr);
#elif defined(__linux__)
	return (context != nullptr);
#endif
}

GLContext::~GLContext()
{
	destroy();
}
