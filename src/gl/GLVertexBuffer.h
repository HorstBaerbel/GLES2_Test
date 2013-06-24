#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdint.h>

#include "GLBase.h"
#include "ContextBase.h"
#include "GLVertexAttribute.h"

//------------------------------------------------------------------------------------------------------

class GLVertexBuffer : public IChangeableObject, public IRenderableObject, public IGLObject
{
	std::map<GLVertexAttributeBase::AttributeRole, std::shared_ptr<GLVertexAttributeBase>> attributes;
	std::shared_ptr<GLVertexAttributeMap> attributeMap;
	std::shared_ptr<GLVertexAttributeBase> indices;
	GLsizei nrOfPrimitives;
	GLsizei nrOfIndices;
	GLenum primitiveMode;
	GLint nrOfVerticesPerPatch;
	GLuint glArrayId;

public:
	GLVertexBuffer(std::shared_ptr<ContextBase> & context, GLenum renderMode = GL_TRIANGLES, GLint verticesPerPatch = 0);

	void addAttribute(std::shared_ptr<GLVertexAttributeBase> attribute);
	GLuint getNrOfAttributes() const;
	void setAttributeMap(std::shared_ptr<GLVertexAttributeMap> attribMap);
	void setIndices(std::shared_ptr<GLVertexAttributeBase> newIndices);
	
	GLenum getRenderMode() const;
	void setRenderMode(GLenum renderMode);
	GLint getVerticesPerPatch() const;
	void setVerticesPerPatch(GLint verticesPerPatch);
	GLsizei getNrOfPrimitivesRendered() const;
	GLsizei getNrOfIndicesRendered() const;

	void prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr);
	void render(std::shared_ptr<ParameterBase> parameter = nullptr);
	void finishRender(std::shared_ptr<ParameterBase> parameter = nullptr);

	~GLVertexBuffer();
};

//------------------------------------------------------------------------------------------------------

class GLVertexBufferException : public GLException
{
public:
	GLVertexBufferException(const char * errorString) throw();
	GLVertexBufferException(const std::string & errorString) throw();
};
