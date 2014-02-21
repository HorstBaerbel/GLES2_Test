#pragma once


#if defined(WIN32) || defined(_WIN32)
	#include <Windows.h>
	//Add defines missing in VS-provided default OpenGL headers
	#ifndef GLAPIENTRY
		#define GLAPIENTRY APIENTRY
	#endif
	#ifndef GLAPIENTRYP
		#define GLAPIENTRYP GLAPIENTRY*
	#endif
#endif

#ifdef USE_OPENGL_DESKTOP
	#include <GL/gl.h>
	#include <GL/glext.h>
#else
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#ifdef USE_OPENGL_GLX
	#ifndef GLXBadContext
		#define GLXBadContext 0
	#endif
	#ifndef GLXBadFBConfig
		#define GLXBadFBConfig 9
	#endif
	#if !defined(GLXBadProfile)
		#define GLXBadProfile 13
	#endif
#endif

#ifdef USE_OPENGL_ES
	//Some GL headers lack some defines we use here...
	#define GL_READ_FRAMEBUFFER 0x8CA8
	#define GL_DRAW_FRAMEBUFFER 0x8CA9
	#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
	#define GL_BGRA 0x80E0
	#define GL_BGR 0x80E1
	#define GLAPIENTRY GL_APIENTRY
	#define GLAPIENTRYP GLAPIENTRY*
	#define GL_WRITE_ONLY GL_WRITE_ONLY_OES
	#define GL_HALF_FLOAT 0x8D61
#endif
