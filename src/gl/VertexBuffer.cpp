#include "VertexBuffer.h"

//------------------------------------------------------------------------------------------------------

VertexBuffer::VertexBuffer(std::shared_ptr<ContextBase> & context, GLenum renderMode, GLint verticesPerPatch)
	: IChangeableObject(), IRenderableObject(), IGLObject(context), nrOfIndices(0), nrOfPrimitives(0), primitiveMode(renderMode), nrOfVerticesPerPatch(verticesPerPatch), glArrayId(0)
{
	//if vertex array objects are supported, use them
	if (glContext->glGenVertexArrays != nullptr) {
		glContext->glGenVertexArrays(1, &glArrayId);
	}
	if (primitiveMode == GL_PATCHES && glContext->glPatchParameteri == nullptr) {
		throw VertexBufferException("VertexBuffer - Render mode is GL_PATCHES, but glPatchParameteri is not available!");
	}
}

void VertexBuffer::addAttribute(std::shared_ptr<VertexAttributeBase> attribute)
{
	//check if type is ok
	if (attribute->getAttributeRole() == VertexAttributeBase::INDEX) {
		throw VertexBufferException("VertexBuffer::addAttribute - Attribute has role INDEX. Use setIndices!");
	}
	//check if attribute with same role already exists
	std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.find(attribute->getAttributeRole());
	if (ait != attributes.cend()) {
		throw VertexBufferException("VertexBuffer::addAttribute - Attribute with role " + std::to_string((_ULonglong)attribute->getAttributeRole()) + " already exists!");
	}
	attributes[attribute->getAttributeRole()] = attribute;
	//mark VBO as changed
	changed = true;
}

void VertexBuffer::setIndices(std::shared_ptr<VertexAttributeBase> newIndices)
{
	//check if the incides array has the proper role
	if (newIndices->getAttributeRole() != VertexAttributeBase::INDEX) {
		throw VertexBufferException("VertexBuffer::setIndices - Attribute doesn't have role INDEX!");
	}
	indices = newIndices;
	//mark VBO as changed
	changed = true;
}

GLuint VertexBuffer::getNrOfAttributes() const
{
	return attributes.size();
}

void VertexBuffer::setAttributeMap(std::shared_ptr<VertexAttributeMap> newAttributeMap)
{
	attributeMap = newAttributeMap;
	changed = true;
}

GLenum VertexBuffer::getRenderMode() const
{
	return primitiveMode;
}

void VertexBuffer::setRenderMode(GLenum renderMode)
{
	primitiveMode = renderMode;
	//update number of rendered indices and primitives in current render mode
	nrOfPrimitives = getNrOfPrimitivesRendered();
}

GLint VertexBuffer::getVerticesPerPatch() const
{
	return nrOfVerticesPerPatch;
}

void VertexBuffer::setVerticesPerPatch(GLint verticesPerPatch)
{
	nrOfVerticesPerPatch = verticesPerPatch;
	//update number of rendered primitives in current render mode
	nrOfPrimitives = getNrOfPrimitivesRendered();
}

GLsizei VertexBuffer::getNrOfIndicesRendered() const
{
	if (indices) {
		//we have an indexed primitive, use indices
		return indices->getElementCount();
	}
	else {
		//we have no indices, use vertex count of attribute VERTEX0 or VERTEX1
		std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.find(VertexAttributeBase::VERTEX0);
		if (ait != attributes.cend() && attributeMap->getAttributeInfo(VertexAttributeBase::VERTEX0).enabled) {
			return ait->second->getElementCount();
		}
		else {
			//not found, try vertex 1
			std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.find(VertexAttributeBase::VERTEX1);
			if (ait != attributes.cend() && attributeMap->getAttributeInfo(VertexAttributeBase::VERTEX1).enabled) {
				return ait->second->getElementCount();
			}
			else {
				throw VertexBufferException("VertexBuffer::getNrOfIndicesRendered - No attribute with role VERTEX0 or VERTEX1 found or none enabled!");
			}
		}
	}
	return 0;
}

GLsizei VertexBuffer::getNrOfPrimitivesRendered() const
{
	GLsizei count;
	if (indices) {
		//we have an indexed primitive, use indices
		count = indices->getElementCount();
	}
	else {
		//we have no indices, use vertex count of attribute VERTEX0 or VERTEX1
		std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.find(VertexAttributeBase::VERTEX0);
		if (ait != attributes.cend() && attributeMap->getAttributeInfo(VertexAttributeBase::VERTEX0).enabled) {
			count = ait->second->getElementCount();
		}
		else {
			//not found, try vertex 1
			std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.find(VertexAttributeBase::VERTEX1);
			if (ait != attributes.cend() && attributeMap->getAttributeInfo(VertexAttributeBase::VERTEX1).enabled) {
				count = ait->second->getElementCount();
			}
			else {
				throw VertexBufferException("VertexBuffer::getNrOfPrimitivesRendered - No attribute with role VERTEX0 or VERTEX1 found or none enabled!");
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

void VertexBuffer::prepareRender()
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
		std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.cbegin();
		while (ait != attributes.cend()) {
			if (ait->second->hasChanged()) {
				changed = true;
				break;
			}
			++ait;
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
		std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.cbegin();
		while (ait != attributes.cend()) {
			const VertexAttributeMap::AttributeInfo attributeInfo = attributeMap->getAttributeInfo(ait->second->getAttributeRole());
			if (attributeInfo.enabled) {
				ait->second->bind(attributeInfo.index);
			}
			++ait;
		}
		//mark the attribute map as not changed
		attributeMap->setChanged(false);
		//bind the index buffer
		if (indices) {
			indices->bind(0);
		}
		changed = false;
	}
}

void VertexBuffer::render()
{
#ifdef USE_OPENGL_DESKTOP
	//if we're doing tesselation, set the number of vertices per patch
	if (glContext->glPatchParameteri != nullptr && primitiveMode == GL_PATCHES) {
		glContext->glPatchParameteri(GL_PATCH_VERTICES, nrOfVerticesPerPatch);
	}
#endif
	//enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	if (indices) {
		glDrawElements(primitiveMode, nrOfPrimitives, indices->getElementGLType(), nullptr);
	}
	else {
		glContext->glDrawArrays(primitiveMode, 0, nrOfIndices);
	}
}

void VertexBuffer::finishRender()
{
	//check if we're actually using vertex array objects
	if (glContext->glBindVertexArray != nullptr && glArrayId != 0) {
		glContext->glBindVertexArray(0);
	}
	else {
		std::map<VertexAttributeBase::AttributeRole, std::shared_ptr<VertexAttributeBase>>::const_iterator ait = attributes.cbegin();
		while (ait != attributes.cend()) {
			const VertexAttributeMap::AttributeInfo attributeInfo = attributeMap->getAttributeInfo(ait->second->getAttributeRole());
			if (attributeInfo.enabled) {
				ait->second->unbind(attributeInfo.index);
			}
		}
		if (indices) {
			indices->unbind(0);
		}
	}
}

VertexBuffer::~VertexBuffer()
{
	//if vertex array objects are supported, use them
	if (glContext->glDeleteVertexArrays != nullptr && glArrayId != 0) {
		glContext->glDeleteVertexArrays(1, &glArrayId);
		glArrayId = 0;
	}
}

//------------------------------------------------------------------------------------------------------

VertexBufferException::VertexBufferException(const char * errorString) throw()
	: GLException(errorString)
{
}

VertexBufferException::VertexBufferException(const std::string & errorString) throw()
	: GLException(errorString)
{
}