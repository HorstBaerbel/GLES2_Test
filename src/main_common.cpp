#include <iostream>

#ifdef USE_OPENGL_DESKTOP
	#include "gl/GLWindow.h"
#else
    #include "gl/ESWindow.h"
#endif

#include "Timing.h"
#include "gl/GLVertexBuffer.h"


bool showFPS = false;
bool doMultisampling = false;
int frames = 0;

std::shared_ptr<WindowBase> window;
std::shared_ptr<ContextBase> context;

GLuint triangleShader;
Parameter<GLuint> triangleVertexAttribute;
Parameter<GLuint> triangleColorAttribute;
const char * vertexCode =
    "attribute vec4 vertex;\n"
	"attribute vec4 color;\n"
    "varying vec4 col;\n"
	"void main() {\n"
	"    col = color;\n"
    "    gl_Position = vertex;\n"
    "}\n";
const char * fragmentCode =
    "#ifdef GL_ES\n"
	"    precision mediump float;\n"
    "#endif\n"
    "varying vec4 col;\n"
	"void main() {\n"
	"    gl_FragColor = col;\n"
	"}\n";
float triangleVertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
float triangleColors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
uint16_t triangleIndices[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

std::shared_ptr<GLVertexAttribute<vec3>> vertices;
std::shared_ptr<GLVertexAttribute<vec3>> colors;
std::shared_ptr<GLVertexAttribute<uint16_t>> indices;
std::shared_ptr<GLVertexAttributeMap> triangleAttributeMap;
std::shared_ptr<GLVertexBuffer> vbo;


void createTriangle()
{
	context->makeCurrent();
	vertices = std::make_shared<GLVertexAttribute<vec3>>(context, GLVertexAttributeBase::VERTEX0);
	vertices->addElements((void *)triangleVertices, 3);
	colors = std::make_shared<GLVertexAttribute<vec3>>(context, GLVertexAttributeBase::COLOR0);
	colors->addElements((void *)triangleColors, 3);
	indices = std::make_shared<GLVertexAttribute<uint16_t>>(context, GLVertexAttributeBase::INDEX);
	indices->addElements(triangleIndices, 9);
	vbo = std::make_shared<GLVertexBuffer>(context);
	vbo->addAttribute(vertices);
	vbo->addAttribute(colors);
	//vbo->setIndices(indices);
	triangleAttributeMap = std::make_shared<GLVertexAttributeMap>();
	triangleAttributeMap->setAttributeIndex(GLVertexAttributeBase::VERTEX0, triangleVertexAttribute);
	triangleAttributeMap->setAttributeIndex(GLVertexAttributeBase::COLOR0, triangleColorAttribute);
	vbo->setAttributeMap(triangleAttributeMap);
}

void drawTriangle()
{
	//draw rgb triangle
    context->makeCurrent();
	context->glUseProgram(triangleShader);
	vbo->prepareRender();
	vbo->render();
	vbo->finishRender();
	/*context->glVertexAttribPointer(triangleVertexAttribute, 3, GL_FLOAT, GL_FALSE, 0, triangleVertices);
	context->glVertexAttribPointer(triangleColorAttribute, 3, GL_FLOAT, GL_FALSE, 0, triangleColors);
	context->glEnableVertexAttribArray(triangleVertexAttribute);
	context->glEnableVertexAttribArray(triangleColorAttribute);
	context->glDrawArrays(GL_TRIANGLES, 0, 3);*/
}

bool setup()
{
	//create opengl window
#if defined(WIN32) || defined(_WIN32)
	LRESULT CALLBACK wndProc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam); //this is a "forward declaration" for the function defined in main_windows.cpp
	window = std::shared_ptr<GLWindow>(new GLWindow(640, 480, "read_test", false, GLWindow::getDefaultFormat(), (WNDPROC)wndProc));
#elif defined(__linux__)
    #ifdef USE_OPENGL_DESKTOP
		window = std::shared_ptr<GLWindow>(new GLWindow(640, 480, "read_test", false, GLWindow::getDefaultFormat()));
    #else
        window = std::shared_ptr<ESWindow>(new ESWindow(640, 480, "read_test", false, ESWindow::getDefaultFormat()));
    #endif
#endif
	window->setSwapInterval(0);

	context = window->getContext();

	//check if we have the needed extensions
/*	if (!context->isExtensionAvailable("GL_EXT_framebuffer_object") && !context->isExtensionAvailable("GL_ARB_framebuffer_object")) {
		std::cout << "Your system is lacking the extension GL_EXT_framebuffer_object. Exiting." << std::endl;
		return false;
	}
	if (!context->isExtensionAvailable("GL_EXT_framebuffer_blit") && !context->isExtensionAvailable("GL_ARB_framebuffer_blit")) {
		std::cout << "Your system is lacking the extension GL_EXT_framebuffer_blit. Exiting." << std::endl;
		return false;
	}
#ifdef USE_OPENGL_DESKTOP
	if (!context->isExtensionAvailable("GL_EXT_pixel_buffer_object") && !context->isExtensionAvailable("GL_ARB_pixel_buffer_object")) {
		std::cout << "Your system is lacking the extension GL_EXT_pixel_buffer_object. Exiting." << std::endl;
		return false;
	}
#endif
	if (!context->isExtensionAvailable("GL_ARB_texture_rectangle")) {
		std::cout << "Your system is lacking the extension GL_ARB_texture_rectangle. Exiting." << std::endl;
		return false;
	}
	if (!context->isExtensionAvailable("GL_ARB_texture_non_power_of_two")) {
		std::cout << "Your system is lacking the extension GL_ARB_texture_non_power_of_two. Exiting." << std::endl;
		return false;
	}*/

	//create shaders
	std::string errors;
	triangleShader = context->createShader(vertexCode, fragmentCode, errors);
	if (triangleShader > 0) {
		//get locations of vertex attributes
		triangleVertexAttribute = (GLuint)context->glGetAttribLocation(triangleShader, "vertex");
		triangleColorAttribute = (GLuint)context->glGetAttribLocation(triangleShader, "color");
	}
	else {
		std::cout << "Failed to create the necessary shader. Exiting." << std::endl;
		return false;
	}

	createTriangle();

	return true;
}

void interactive()
{
	//make context current
	context->makeCurrent();

	//default: render to screen
	context->glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window->getWidth(), window->getHeight());

	//clear framebuffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawTriangle();

	//swap backbuffer to screen
	window->swap();
}
