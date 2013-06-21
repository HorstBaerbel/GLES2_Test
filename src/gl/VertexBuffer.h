#pragma once

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <stdint.h>

#include "GLBase.h"
#include "VertexAttribute.h"

//------------------------------------------------------------------------------------------------------

class VertexBuffer : public IChangeableObject, public IRenderableObject, public IGLObject
{
	std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>> attributes;
	std::shared_ptr<VertexAttributeMap> attributeMap;
	std::shared_ptr<VertexAttributeBase> indices;
	GLsizei nrOfPrimitives;
	GLsizei nrOfIndices;
	GLenum primitiveMode;
	GLint nrOfVerticesPerPatch;
	GLuint glArrayId;

public:
	VertexBuffer(std::shared_ptr<ContextBase> & context, GLenum renderMode = GL_TRIANGLES, GLint verticesPerPatch = 0);

	void addAttribute(std::shared_ptr<VertexAttributeBase> attribute);
	GLuint getNrOfAttributes() const;
	void setAttributeMap(std::shared_ptr<VertexAttributeMap> attribMap);
	void setIndices(std::shared_ptr<VertexAttributeBase> newIndices);
	
	GLenum getRenderMode() const;
	void setRenderMode(GLenum renderMode);
	GLint getVerticesPerPatch() const;
	void setVerticesPerPatch(GLint verticesPerPatch);
	GLsizei getNrOfPrimitivesRendered() const;
	GLsizei getNrOfIndicesRendered() const;

	void prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr);
	void render(std::shared_ptr<ParameterBase> parameter = nullptr);
	void finishRender(std::shared_ptr<ParameterBase> parameter = nullptr);

	~VertexBuffer();
};

//------------------------------------------------------------------------------------------------------

class VertexBufferException : public GLException
{
public:
	VertexBufferException(const char * errorString) throw();
	VertexBufferException(const std::string & errorString) throw();
};
