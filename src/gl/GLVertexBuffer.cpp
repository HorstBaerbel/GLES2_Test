#include "GLVertexBuffer.h"

//------------------------------------------------------------------------------------------------------

GLVertexBuffer::GLVertexBuffer(std::shared_ptr<ContextBase> & context, GLenum renderMode, GLint verticesPerPatch)
	: IChangeableObject(), IRenderableObject(), IGLObject(context), nrOfIndices(0), nrOfPrimitives(0), primitiveMode(renderMode), nrOfVerticesPerPatch(verticesPerPatch), glArrayId(0)
{
	//if vertex array objects are supported, use them
	if (glContext->glGenVertexArrays != nullptr) {
		glContext->glGenVertexArrays(1, &glArrayId);
	}
#ifdef USE_OPENGL_DESKTOP
	if (primitiveMode == GL_PATCHES && glContext->glPatchParameteri == nullptr) {
		throw GLVertexBufferException("VertexBuffer - Render mode is GL_PATCHES, but glPatchParameteri is not available!");
	}
#endif
}

void GLVertexBuffer::addAttribute(std::shared_ptr<GLVertexAttributeBase> attribute)
{
	//check if type is ok
	if (attribute->getAttributeRole() == GLVertexAttributeBase::INDEX) {
		throw GLVertexBufferException("VertexBuffer::addAttribute - Attribute has role INDEX. Use setIndices!");
	}
	//check if attribute with same role already exists
	auto ait = attributes.find(attribute->getAttributeRole());
	if (ait != attributes.cend()) {
		throw GLVertexBufferException("VertexBuffer::addAttribute - Attribute with role " + std::to_string((long long)attribute->getAttributeRole()) + " already exists!");
	}
	attributes[attribute->getAttributeRole()] = attribute;
	//mark VBO as valid once we have some vertices
	if (attribute->getAttributeRole() == GLVertexAttributeBase::VERTEX0 || attribute->getAttributeRole() == GLVertexAttributeBase::VERTEX1) {
		valid = true;
	}
	//mark VBO as changed
	changed = true;
}

void GLVertexBuffer::setIndices(std::shared_ptr<GLVertexAttributeBase> newIndices)
{
	//check if the incides array has the proper role
	if (newIndices->getAttributeRole() != GLVertexAttributeBase::INDEX) {
		throw GLVertexBufferException("VertexBuffer::setIndices - Attribute doesn't have role INDEX!");
	}
	indices = newIndices;
	//mark VBO as changed
	changed = true;
}

GLuint GLVertexBuffer::getNrOfAttributes() const
{
	return attributes.size();
}

void GLVertexBuffer::setAttributeMap(std::shared_ptr<GLVertexAttributeMap> newAttributeMap)
{
	attributeMap = newAttributeMap;
	changed = true;
}

GLenum GLVertexBuffer::getRenderMode() const
{
	return primitiveMode;
}

void GLVertexBuffer::setRenderMode(GLenum renderMode)
{
	primitiveMode = renderMode;
	//update number of rendered indices and primitives in current render mode
	nrOfPrimitives = getNrOfPrimitivesRendered();
}

GLint GLVertexBuffer::getVerticesPerPatch() const
{
	return nrOfVerticesPerPatch;
}

void GLVertexBuffer::setVerticesPerPatch(GLint verticesPerPatch)
{
	nrOfVerticesPerPatch = verticesPerPatch;
	//update number of rendered primitives in current render mode
	nrOfPrimitives = getNrOfPrimitivesRendered();
}

GLsizei GLVertexBuffer::getNrOfIndicesRendered() const
{
	if (indices) {
		//we have an indexed primitive, use indices
		return indices->getElementCount();
	}
	else {
		//we have no indices, use vertex count of attribute VERTEX0 or VERTEX1
		auto ait = attributes.find(GLVertexAttributeBase::VERTEX0);
		if (ait != attributes.cend() && attributeMap->getAttributeInfo(GLVertexAttributeBase::VERTEX0).enabled) {
			return ait->second->getElementCount();
		}
		else {
			//not found, try vertex 1
			auto ait = attributes.find(GLVertexAttributeBase::VERTEX1);
			if (ait != attributes.cend() && attributeMap->getAttributeInfo(GLVertexAttributeBase::VERTEX1).enabled) {
				return ait->second->getElementCount();
			}
			else {
				throw GLVertexBufferException("VertexBuffer::getNrOfIndicesRendered - No attribute with role VERTEX0 or VERTEX1 found or none enabled!");
			}
		}
	}
	return 0;
}

GLsizei GLVertexBuffer::getNrOfPrimitivesRendered() const
{
	GLsizei count;
	if (indices) {
		//we have an indexed primitive, use indices
		count = indices->getElementCount();
	}
	else {
		//we have no indices, use vertex count of attribute VERTEX0 or VERTEX1
		auto ait = attributes.find(GLVertexAttributeBase::VERTEX0);
		if (ait != attributes.cend() && attributeMap->getAttributeInfo(GLVertexAttributeBase::VERTEX0).enabled) {
			count = ait->second->getElementCount();
		}
		else {
			//not found, try vertex 1
			auto ait = attributes.find(GLVertexAttributeBase::VERTEX1);
			if (ait != attributes.cend() && attributeMap->getAttributeInfo(GLVertexAttributeBase::VERTEX1).enabled) {
				count = ait->second->getElementCount();
			}
			else {
				throw GLVertexBufferException("VertexBuffer::getNrOfPrimitivesRendered - No attribute with role VERTEX0 or VERTEX1 found or none enabled!");
			}
		}
	}

	switch(primitiveMode) {
		case GL_POINTS:
			return count;
			break;
		case GL_LINES:
#ifdef USE_OPENGL_DESKTOP
		case GL_LINES_ADJACENCY:
#endif
			return count / 2;
			break;
		case GL_LINE_LOOP:
			return count;
			break;
		case GL_LINE_STRIP:
#ifdef USE_OPENGL_DESKTOP
		case GL_LINE_STRIP_ADJACENCY:
#endif
			return count - 1;
			break;
		case GL_TRIANGLES:
#ifdef USE_OPENGL_DESKTOP
		case GL_TRIANGLES_ADJACENCY:
#endif
			return count / 3;
			break;
		case GL_TRIANGLE_STRIP:
		case GL_TRIANGLE_FAN:
#ifdef USE_OPENGL_DESKTOP
		case GL_TRIANGLE_STRIP_ADJACENCY:
#endif
			return count - 2;
			break;
/* These are deprecated in desktop OpenGL and not available in ES
		case GL_QUADS:
			return count / 4;
			break;
		case GL_QUAD_STRIP:
			return count / 2 - 1;
			break;
		case GL_POLYGON:
			return 1;
			break;
			*/
#ifdef USE_OPENGL_DESKTOP
		case GL_PATCHES:
			return count / nrOfVerticesPerPatch;
			break;
#endif
	};
	return 0;
}

bool GLVertexBuffer::prepareRender(std::shared_ptr<ParameterBase> parameter)
{
	//check if we're actually using vertex array objects
	if (glContext->glBindVertexArray != nullptr && glArrayId != 0) {
		glContext->glBindVertexArray(glArrayId);
	}
	else {
		//if we get here, either VAOs aren't working, something changed in the VBO or we need to re-initialize
		changed = true;
	}
	//no change in vertex buffer or initialization, but the VBOs or indices or attribute map may have changed
	if (!changed) {
		//check if any of the vertex buffers has changed
		for (auto ait = attributes.cbegin(); ait != attributes.cend(); ++ait) {
			if (ait->second->hasChanged()) {
				changed = true;
				break;
			}
		}
		//check if the vertex indices have changed
		if (!changed && indices && indices->hasChanged()) {
			changed = true;
		}
		//check if the attribute map has changed
		if (!changed && attributeMap->hasChanged()) {
			changed = true;
		}
	}
	//check if something has changed or we need to (re-)initialize
	if (changed) {
		//get the new number of indices and primitives rendered
		nrOfIndices = getNrOfIndicesRendered();
		nrOfPrimitives = getNrOfPrimitivesRendered();
		//bind all vertex attribute buffers to the correpsonding index in the attribute map
		for (auto ait = attributes.cbegin(); ait != attributes.cend(); ++ait) {
			const GLVertexAttributeMap::AttributeInfo attributeInfo = attributeMap->getAttributeInfo(ait->second->getAttributeRole());
			if (attributeInfo.enabled) {
				ait->second->bind(attributeInfo.index);
			}
		}
		//mark the attribute map as not changed
		attributeMap->setChanged(false);
		//bind the index buffer
		if (indices) {
			indices->bind();
		}
		changed = false;
	}
	return true;
}

bool GLVertexBuffer::render(std::shared_ptr<ParameterBase> parameter)
{
#ifdef USE_OPENGL_DESKTOP
	//if we're doing tesselation, set the number of vertices per patch
	if (glContext->glPatchParameteri != nullptr && primitiveMode == GL_PATCHES) {
		glContext->glPatchParameteri(GL_PATCH_VERTICES, nrOfVerticesPerPatch);
	}
	//enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
#endif
	if (indices) {
		glDrawElements(primitiveMode, nrOfPrimitives, indices->getElementGLType(), nullptr);
	}
	else {
		glContext->glDrawArrays(primitiveMode, 0, nrOfIndices);
	}
	return true;
}

bool GLVertexBuffer::finishRender(std::shared_ptr<ParameterBase> parameter)
{
	//check if we're actually using vertex array objects
	if (glContext->glBindVertexArray != nullptr && glArrayId != 0) {
		glContext->glBindVertexArray(0);
	}
	else {
		for (auto aIt = attributes.cbegin(); aIt != attributes.cend(); ++aIt) {
			const GLVertexAttributeMap::AttributeInfo attributeInfo = attributeMap->getAttributeInfo(aIt->second->getAttributeRole());
			if (attributeInfo.enabled) {
				aIt->second->unbind(attributeInfo.index);
			}
		}
		if (indices) {
			indices->unbind();
		}
	}
	return true;
}

GLVertexBuffer::~GLVertexBuffer()
{
	//if vertex array objects are supported, use them
	if (glContext->glDeleteVertexArrays != nullptr && glArrayId != 0) {
		glContext->glDeleteVertexArrays(1, &glArrayId);
		glArrayId = 0;
	}
}

//------------------------------------------------------------------------------------------------------

GLVertexBufferException::GLVertexBufferException(const char * errorString) throw()
	: GLException(errorString)
{
}

GLVertexBufferException::GLVertexBufferException(const std::string & errorString) throw()
	: GLException(errorString)
{
}
