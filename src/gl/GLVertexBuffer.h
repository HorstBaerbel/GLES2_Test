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

/*!
Bundles vertex buffer objects with vertex attributes into a vertex array object.
Holds an attribute map to enable/disable vertex attributes and map them to a specific slot index for a shader.
Also and index buffer can be added for indexed rendering.
*/
class GLVertexBuffer : public IChangeableObject, public IRenderableObject, public IGLObject
{
	std::map<GLVertexAttributeBase::AttributeRole, std::shared_ptr<GLVertexAttributeBase>> attributes; //!<Map from vertex attribute role to attribute data.
	std::shared_ptr<GLVertexAttributeMap> attributeMap; //!<Map from vertex attribute role to attribute index (for rendering).
	std::shared_ptr<GLVertexAttributeBase> indices; //!<Mesh indices if any.
	GLsizei nrOfPrimitives; //!<Number of primitives rendered in \primitiveMode.
	GLsizei nrOfIndices; //!<Number of indices rendered.
	GLenum primitiveMode; //!<Primitive mode used for rendering.
	GLint nrOfVerticesPerPatch; //!<Number of vertices per patch. Only if tesselation is enabled (\primitiveMode is GL_PATCH).
	GLuint glArrayId; //!<OpenGL identifier for VAO.

public:
    /*!
    Constructor.
    \param[in] context The OpenGL context this buffer is created in.
    \param[in] renderMode The mode the primitves are renderer in (e.g. GL_TRIANGLES).
    \param[in] verticesPerPatch The number of vertices per patch, if \renderMode is GL_PATCH.
    */
	GLVertexBuffer(std::shared_ptr<ContextBase> & context, GLenum renderMode = GL_TRIANGLES, GLint verticesPerPatch = 0);

    /*!
    Add a vertex attribute.
    \param[in] attribute The attribute to add.
    \note The function will throw a GLVertexBufferException if the attribute already exists.
    */
	void addAttribute(std::shared_ptr<GLVertexAttributeBase> attribute);

    /*!
    Return number of attributes currently in vertex buffer.
    \return Returns the number of attributes currently in vertex buffer.
    */
	GLuint getNrOfAttributes() const;

    /*!
    Set a new attribute map. Use this when e.h. using a new shader with the same vertex buffer.
    \param[in] attribMap New attribute map.
    */
	void setAttributeMap(std::shared_ptr<GLVertexAttributeMap> attribMap);

    /*!
    Set VBO indices. These need to be handled a bit differently.
    \param[in] newIndices New VBO indices.
    */
	void setIndices(std::shared_ptr<GLVertexAttributeBase> newIndices);
	
    /*!
    Get the current render mode for this VBO.
    \return Returns the current OpenGL render mode.
    */
	GLenum getRenderMode() const;

    /*!
    Set a new render mode for this VBO.
    \param[in] renderMode The new OpenGL render mode for the next render call.
    */
	void setRenderMode(GLenum renderMode);

    /*!
    Vertices per patch when tesselation is used.
    \return Returns the number of vertices passed to the geometry shader when tesselation is used.
    */
	GLint getVerticesPerPatch() const;

    /*!
    Set the vertices per patch when tesselation is used.
    \param[in] verticesPerPatch The new number of vertices passed to the geometry shader when tesselation is used.
    */
	void setVerticesPerPatch(GLint verticesPerPatch);

    /*!
    The number of primitives that will be rendered regarding to the current number of vertices or indices and the render mode.
    \return The number of primitves that will be rendered with the current settings.
    */
	GLsizei getNrOfPrimitivesRendered() const;

    /*!
    The number of indices that will be rendered regarding to the current number render mode.
    \return The number of indices that will be rendered with the current settings.
    */
	GLsizei getNrOfIndicesRendered() const;

	bool prepareRender(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	bool render(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	bool finishRender(std::shared_ptr<ParameterBase> parameter = nullptr) override;

	~GLVertexBuffer();
};

//------------------------------------------------------------------------------------------------------

class GLVertexBufferException : public GLException
{
public:
	GLVertexBufferException(const char * errorString) throw();
	GLVertexBufferException(const std::string & errorString) throw();
};
