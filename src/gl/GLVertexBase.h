#pragma once

#include <map>
#include <memory>

#include "GLBase.h"

//------------------------------------------------------------------------------------------------------

class GLVertexAttributeBase : public IBindableObject, public IChangeableObject, public IGLObject
{
public:
	enum AttributeRole {VERTEX0, VERTEX1, TEXCOORD0, TEXCOORD1, TEXCOORD2, TEXCOORD3, NORMAL0, NORMAL1, TANGENT0, TANGENT1, BITANGENT0, BITANGENT1, COLOR0, COLOR1, DATA0, DATA1, INDEX};
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

	GLVertexAttributeBase(std::shared_ptr<ContextBase> & context, AttributeRole role, GLenum usagePattern = GL_DYNAMIC_DRAW);
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
};

//------------------------------------------------------------------------------------------------------

class GLVertexAttributeMap : public IChangeableObject
{
public:
	struct AttributeInfo
	{
		bool enabled;
		std::shared_ptr<Parameter<GLuint>> index;

		AttributeInfo() : enabled(false), index(nullptr) {};
	};

private:
	std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo> attributeMap;

public:
	AttributeInfo getAttributeInfo(GLVertexAttributeBase::AttributeRole role) const;
	AttributeInfo getAttributeInfo(const std::string & roleName) const;

	void enableAttribute(GLVertexAttributeBase::AttributeRole role, bool enable);
	void enableAttribute(const std::string & roleName, const Parameter<GLuint> & index);
	void setAttributeIndex(GLVertexAttributeBase::AttributeRole role, const Parameter<GLuint> & index, bool enable = true);
	void setAttributeIndex(const std::string & roleName, const Parameter<GLuint> & index, bool enable = true);

	bool hasChanged() const;
};

//------------------------------------------------------------------------------------------------------

class GLVertexAttributeException : public GLException
{
public:
	GLVertexAttributeException(const char * errorString) throw();
	GLVertexAttributeException(const std::string & errorString) throw();
};
