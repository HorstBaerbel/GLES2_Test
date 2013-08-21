#pragma once

#include <string>

#include "ContextBase.h"


#if defined(WIN32) || defined(_WIN32)
	typedef HWND DisplayHandle;
#elif defined(__linux__)
	#ifdef USE_OPENGL_DESKTOP
		typedef Display * DisplayHandle;
	#else
		typedef EGLDisplay DisplayHandle;
	#endif
#endif


/*!
OpenGL window base class.
\todo Should probably be abstract.
*/
class WindowBase
{
public:
	struct Format {
		int redSize; //!<Number of bits of red color component in color buffer.
		int greenSize; //!<Number of bits of green color component in color buffer.
		int blueSize; //!<Number of bits of blue color component in color buffer.
		int alphaSize; //!<Number of bits of alpha component in color buffer.
		int depthSize; //!<Number of bits in depth buffer.
		int stencilSize; //!<Number of bits in stencil buffer.
		int samplesPerPixel; //!<Number of samples per pixel (> 1 for multisampling).
		bool doubleBuffering; //!<True if doubblebuffering is enabled.
	}; //!<Window color/depth/stencil buffer surface format.

protected:
    int w; //!<Width of window.
    int h; //!<Height of window.
    bool full; //!<if the window is fullscreen
	Format format; //!<the window surface format
    std::shared_ptr<ContextBase> context; //!<The corresponding rendering context for the window.

protected:
    WindowBase(const int width, const int height, std::string title, const bool fullScreen, const Format & windowFormat);

public:
	/*!
	Override this to set up basic OpenGL view/camera/lighting etc. stuff.
	*/
    virtual void setup();

    int getWidth() const;
    int getHeight() const;
    bool isFullscreen() const;
   
	/*!
	Returns the currently used OpenGL context.
	\return Returns the currently used OpenGL context.
	*/
    virtual std::shared_ptr<ContextBase> & getContext();

	/*!
	Returns the currently used window surface format.
	\return Returns the currently used window surface format.
	*/
	const Format & getFormat() const;

	/*!
	Returns the default window surface format used.
	\return Returns the default window surface format.
	*/
	static const Format getDefaultFormat();

	/*!
	Return OS-specific display handle.
	\return Returns the OS-specific display handle used.
	*/
	virtual DisplayHandle getDisplayHandle() const;

	/*!
	Set buffer swap interval.
	\param[in] interval Pass 1 to turn vsync on (default), 0 to turn if off and -1 to enable adaptive vsync.
	\note "WGL_EXT_swap_control"/"GLX_EXT_swap_control" must be present for this to work. "EXT_swap_control_tear" is needed for adaptive vsync.
	\note MIGHT work, depending on the day of the week. Also sacrificing a small animal might help, no idea...
	*/
	virtual void setSwapInterval(int interval) const;

	/*
	Swap back buffer to screen.
	*/
    virtual void swap() const;

	/*
	Destroy window. Override this with the necessary functions to tear down the window.
	*/
    virtual void destroy();

	/*
	Destructor calls \destroy by default.
	*/
    virtual ~WindowBase();
};
