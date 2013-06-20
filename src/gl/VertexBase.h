#pragma once

#include <map>
#include <memory>

#include "GLBase.h"

//------------------------------------------------------------------------------------------------------

class VertexAttributeBase : public IChangeableObject, public IGLObject
{
public:
	enum AttributeRole {VERTEX0, VERTEX1, TEXCOORD0, TEXCOORD1, TEXCOORD2, TEXCOORD3, NORMAL0, NORMAL1, COLOR0, COLOR1, DATA0, DATA1, INDEX};
	static const std::string AttributeName[];

	struct ElementTypeInfo {
		GLint nrOfComponents;
		GLboolean normalize;
		GLenum glType;
	};

protected:
	AttributeRole attributeRole;
	GLenum glUsage;
	ElementTypeInfo elementTypeInfo;

	VertexAttributeBase(std::shared_ptr<ContextBase> & context, AttributeRole role, GLenum usagePattern = GL_DYNAMIC_DRAW);
	virtual const ElementTypeInfo & getElementTypeInfo() const;

public:
	AttributeRole getAttributeRole() const;
	const std::string & getAttributeName() const;

	virtual size_t getElementSize() const = 0;
	virtual size_t getElementCount() const = 0;
	virtual GLenum getElementGLType() const = 0;
	virtual size_t getElementNrOfComponents() const = 0;

	virtual const void * getRawData() const = 0;
	virtual size_t getRawSize() const = 0;

	virtual void bind(GLuint index) = 0;
	virtual void unbind(GLuint index) = 0;
};

//------------------------------------------------------------------------------------------------------

class VertexAttributeMap : public IChangeableObject
{
public:
	struct AttributeInfo
	{
		bool enabled;
		GLuint index;

		AttributeInfo() : enabled(false), index(0) {};
	};

private:
	std::map<VertexAttributeBase::AttributeRole, AttributeInfo> attributeMap;

public:
	AttributeInfo getAttributeInfo(VertexAttributeBase::AttributeRole role) const;
	AttributeInfo getAttributeInfo(const std::string & roleName) const;

	void enableAttribute(VertexAttributeBase::AttributeRole role, bool enable);
	void enableAttribute(const std::string & roleName, GLuint index);
	void setAttributeIndex(VertexAttributeBase::AttributeRole role, GLuint index, bool enable = true);
	void setAttributeIndex(const std::string & roleName, GLuint index, bool enable = true);
};

//------------------------------------------------------------------------------------------------------

class VertexAttributeException : public GLException
{
public:
	VertexAttributeException(const char * errorString) throw();
	VertexAttributeException(const std::string & errorString) throw();
};
