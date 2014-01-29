#pragma once

#include <map>
#include <memory>

#include "GLBase.h"

//------------------------------------------------------------------------------------------------------

/*!
Type-independent vertex attribute base class to make storing and casting via shared_ptrs possible.
\todo Implement reading back attributes from GPU.
*/
class GLVertexAttributeBase : public IChangeableObject, public IBindableObject, public IGLObject
{
public:
    //TODO: Improve. Use statics or map and C++11 to initialize the below

    /*!The different vertex attribute roles.*/
	enum AttributeRole {VERTEX0, VERTEX1, TEXCOORD0, TEXCOORD1, TEXCOORD2, TEXCOORD3, NORMAL0, NORMAL1, TANGENT0, TANGENT1, BITANGENT0, BITANGENT1, COLOR0, COLOR1, DATA0, DATA1, INDEX};
	static const std::string AttributeName[]; //!<Names of the vertex attribute roles.

	struct ElementTypeInfo {
		GLint nrOfComponents; //!<Number of components of element (e.g. 2 for a 2-component vector).
		GLboolean normalize; //!<If the values should be normalized to the 0-1 range by OpenGL.
		GLenum glType; //!<The basic OpenGL type of one element component, e.g. GL_FLOAT
	}; //!<Information about the elements contained in a vertex buffer. Primarily used for rendering.

protected:
	AttributeRole attributeRole; //!<The role of the vertex attribute. Can be used to check for duplicates etc.
	GLenum glUsage; //!<Usage pattern of the vertex attribute.
	ElementTypeInfo elementTypeInfo; //!<Information about the binary data contained in the vertex attribute.

    /*!
    Constructor.
    \param[in] context The OpenGL context this attribute will belong to.
    \param[in] role The role of the attribute.
    \param[in] usagePattern Optional. How the attribute will be used (read/write, static/dynamic).
    */
	GLVertexAttributeBase(std::shared_ptr<ContextBase> & context, AttributeRole role, GLenum usagePattern = GL_DYNAMIC_DRAW);

    /*!
    Return information about the binary data contained in the attribute.
    \return Returns information about the binary data contained in the attribute.
    \note This needs to be specialized for every OpenGL/basic type to be used as an attribute.
    */
	virtual const ElementTypeInfo & getElementTypeInfo() const;

public:
    /*!
    Return role of attribute.
    \return Returns the attribute role.
    */
	AttributeRole getAttributeRole() const;

    /*!
    Return the name of the role of the attribute.
    \return Returns the attribute role name, e.g. "vertex".
    */
	const std::string & getAttributeName() const;

    //---------------------------------------------------------------------------------------

    /*!
    Get the size of one element of the attribute in bytes.
    \return Returns the binary size of one attribute element.
    */
	virtual size_t getElementSize() const = 0;

    /*!
    Get the size of one element of the attribute in bytes.
    \return Returns the binary size of one attribute element.
    */
	virtual size_t getElementCount() const = 0;

    /*!
    Get the OpenGL type of one component of the attribute elements.
    \return Returns the OpenGL type of one component of the attribute elements
    */
	virtual GLenum getElementGLType() const = 0;

    /*!
    Get the number of components per attribute element.
    \return Returns the number of components per attribute element.
    */
	virtual size_t getElementNrOfComponents() const = 0;

    /*!
    Return a pointer to the raw binary data of the attribute.
    \returns A pointer to the raw binary data of the attribute.
    */
	virtual const void * getRawData() const = 0;

    /*!
    Return the size of the raw binary data of the attribute.
    \returns The size of the raw binary data of the attribute.
    */
	virtual size_t getRawSize() const = 0;

    //Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const GLVertexAttributeBase & attribute);
    friend std::istream & operator>>(std::istream & is, GLVertexAttributeBase & attribute);

    //virtual ~GLVertexAttributeBase();
};

//------------------------------------------------------------------------------------------------------

class GLVertexAttributeMap : public IChangeableObject
{
public:
	struct AttributeInfo
	{
		bool enabled; //!<If the vertex attribute will be enabled when redering.
		std::shared_ptr<Parameter<GLuint>> index; //!<The vertex attribute index used when rendering.

		AttributeInfo() : enabled(false), index(nullptr) {};
	};

private:
    //!<Map from vertex attribute role to attribute information (for rendering).
	std::map<GLVertexAttributeBase::AttributeRole, AttributeInfo> attributeMap;

public:
    /*!
    Retrieve information about the attribute with \role.
    \param[in] role Role of the attribute to get information about.
    \not Throws a GLVertexAttributeException if the attribute does not exist.
    */
	AttributeInfo getAttributeInfo(GLVertexAttributeBase::AttributeRole role) const;

    /*!
    Retrieve information about the attribute with \roleName.
    \param[in] roleName Role name of the attribute to get information about.
    \not Throws a GLVertexAttributeException if the attribute does not exist.
    */
	AttributeInfo getAttributeInfo(const std::string & roleName) const;

    /*!
    Toggle attribute rendering on/off.
    \param[in] role The role of the attribute to toggle.
    \param[in] enable Set to true to enable attribute.
    */
	void enableAttribute(GLVertexAttributeBase::AttributeRole role, bool enable);

    /*!
    Toggle attribute rendering on/off.
    \param[in] roleName Role name of the attribute to toggle.
    \param[in] enable Set to true to enable attribute.
    */
	void enableAttribute(const std::string & roleName, bool enable);

    /*!
    Set attribute slot index.
    \param[in] role The role of the attribute to set.
    \param[in] index The new slot index for the attribute.
    \param[in] enable Optional. Set to true to enable attribute.
    \todo Make sure this works for big parameters like matrices.
    */
	void setAttributeIndex(GLVertexAttributeBase::AttributeRole role, const Parameter<GLuint> & index, bool enable = true);

    /*!
    Set attribute slot index.
    \param[in] roleName Role name of the attribute to set.
    \param[in] index The new slot index for the attribute.
    \param[in] enable Optional. Set to true to enable attribute.
    \todo Make sure this works for big parameters like matrices.
    */
	void setAttributeIndex(const std::string & roleName, const Parameter<GLuint> & index, bool enable = true);

	bool hasChanged() const;

    //Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const GLVertexAttributeMap & map);
    friend std::istream & operator>>(std::istream & is, GLVertexAttributeMap & map);
};

//------------------------------------------------------------------------------------------------------

class GLVertexAttributeException : public GLException
{
public:
	GLVertexAttributeException(const char * errorString) throw();
	GLVertexAttributeException(const std::string & errorString) throw();
};
