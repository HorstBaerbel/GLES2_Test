#pragma once

#include <memory>

#include "../math/Math.h"
#include "VertexBase.h"

//------------------------------------------------------------------------------------------------------

template <typename TYPE>
class VertexAttribute : public VertexAttributeBase
{
	std::vector<TYPE> data;
	size_t oldDataSize;
	GLuint glBufferId;

public:
	VertexAttribute(std::shared_ptr<ContextBase> & context, VertexAttributeBase::AttributeRole role, GLenum usagePattern = GL_DYNAMIC_DRAW);

	void addElement(const TYPE & element);
	void addElements(const TYPE * elements, size_t count);
	void addElements(const void * rawElements, size_t count);

	size_t getElementSize() const;
	size_t getElementCount() const;
	GLenum getElementGLType() const;
	size_t getElementNrOfComponents() const;

	const void * getRawData() const;
	size_t getRawSize() const;

	//Specific type trait functions for e.g. glVertexAttribPointer
	const ElementTypeInfo & getElementTypeInfo() const;

	/*!
	Bind attribute buffer to index and return following index.
	\param[in] index The index this vertex attribute should use.
	*/
	void bind(GLuint index);
	void unbind(GLuint index);

	~VertexAttribute();
};

//------------------------------------------------------------------------------------------------------

template <typename TYPE>
inline VertexAttribute<TYPE>::VertexAttribute(std::shared_ptr<ContextBase> & context, VertexAttributeBase::AttributeRole role,GLenum usagePattern)
	: VertexAttributeBase(context, role, usagePattern), glBufferId(-1), oldDataSize(0)
{
	elementTypeInfo = getElementTypeInfo();
	glContext->glGenBuffers(1, &glBufferId);
	if (glBufferId == 0) {
		throw VertexAttributeException("VertexAttribute() - failed to generate vertex buffer!");
	}
}

template <typename TYPE>
inline void VertexAttribute<TYPE>::addElement(const TYPE & element)
{
	data.push_back(element);
	changed = true;
}

template <typename TYPE>
inline void VertexAttribute<TYPE>::addElements(const TYPE * elements, size_t count)
{
	for (size_t i = 0; i < count; i++) {
		data.push_back(elements[i]);
	}
	changed = true;
}

//TODO: This sucks and will probably most break very soon...
template <typename TYPE>
inline void VertexAttribute<TYPE>::addElements(const void * rawElements, size_t count)
{
	const TYPE * elements = (const TYPE *)rawElements;
	for (size_t i = 0; i < count; i++) {
		data.push_back(elements[i]);
	}
	changed = true;
}

template <typename TYPE>
inline size_t VertexAttribute<TYPE>::getElementSize() const
{
	return sizeof(TYPE);
}

template <typename TYPE>
inline size_t VertexAttribute<TYPE>::getElementCount() const
{
	return data.size();
}

template <typename TYPE>
size_t VertexAttribute<TYPE>::getElementNrOfComponents() const
{
	return elementTypeInfo.nrOfComponents;
}

template <typename TYPE>
GLenum VertexAttribute<TYPE>::getElementGLType() const
{
	return elementTypeInfo.glType;
}

template <typename TYPE>
inline const void * VertexAttribute<TYPE>::getRawData() const
{
	return data.data();
}

template <typename TYPE>
inline size_t VertexAttribute<TYPE>::getRawSize() const
{
	return data.size() * sizeof(TYPE);
}

template <typename TYPE>
inline void VertexAttribute<TYPE>::bind(GLuint index)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	//check if this is an index array to an actual vertex attribute
	if (INDEX == attributeRole) {
		glContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glBufferId);
		if (changed) {
			//(re-)allocate buffer if necessary
			const size_t dataSize = getRawSize();
			if (dataSize > oldDataSize) {
				glContext->glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, nullptr, glUsage);
				oldDataSize = dataSize;
			}
			//map buffer to user space
			GLvoid * buffer = glContext->glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
			if (buffer != nullptr) {
				//now add data
				memcpy(buffer, getRawData(), dataSize);
				glContext->glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
				changed = false;
			}
		}
	}
	else {
		//enable this attribute array
		glContext->glEnableVertexAttribArray(index);
		//bind buffer to it
		glContext->glBindBuffer(GL_ARRAY_BUFFER, glBufferId);
		if (changed) {
			//(re-)allocate buffer if necessary
			const size_t dataSize = getRawSize();
			if (dataSize > oldDataSize) {
				glContext->glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, nullptr, glUsage);
				oldDataSize = dataSize;
			}
			//map buffer to user space
			GLvoid * buffer = glContext->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			if (buffer != nullptr) {
				//now add data
				memcpy(buffer, getRawData(), dataSize);
				glContext->glUnmapBuffer(GL_ARRAY_BUFFER);
				changed = false;
			}
		}
		//set up attribute pointer
		glContext->glVertexAttribPointer(index, elementTypeInfo.nrOfComponents, elementTypeInfo.glType, elementTypeInfo.normalize, 0, nullptr);
		//unbid buffer again. this saves us a call later
		glContext->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

template <typename TYPE>
inline void VertexAttribute<TYPE>::unbind(GLuint index)
{
	//check if this is an index array to an actual vertex attribute
	if (INDEX == attributeRole) {
		glContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glContext->glDisableVertexAttribArray(index);
		//glContext->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

template <typename TYPE>
inline VertexAttribute<TYPE>::~VertexAttribute()
{
	glContext->glDeleteBuffers(1, &glBufferId);
	glBufferId = 0;
}

//------------------------------------------------------------------------------------------------------
//Type traits

template <> inline const VertexAttribute<float>::ElementTypeInfo & VertexAttribute<float>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_FLOAT}; return info;
}

#ifdef USE_OPENGL_DESKTOP
	template <> inline const VertexAttribute<double>::ElementTypeInfo & VertexAttribute<double>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_DOUBLE}; return info;
	}
#endif

template <> inline const VertexAttribute<int8_t>::ElementTypeInfo & VertexAttribute<int8_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_BYTE}; return info;
}

template <> inline const VertexAttribute<uint8_t>::ElementTypeInfo & VertexAttribute<uint8_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_UNSIGNED_BYTE}; return info;
}

template <> inline const VertexAttribute<int16_t>::ElementTypeInfo & VertexAttribute<int16_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_SHORT}; return info;
}

template <> inline const VertexAttribute<uint16_t>::ElementTypeInfo & VertexAttribute<uint16_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_UNSIGNED_SHORT}; return info;
}

#ifdef USE_OPENGL_DESKTOP
	template <> inline const VertexAttribute<int32_t>::ElementTypeInfo & VertexAttribute<int32_t>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_INT}; return info;
	}

	template <> inline const VertexAttribute<uint32_t>::ElementTypeInfo & VertexAttribute<uint32_t>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_UNSIGNED_INT}; return info;
	}
#else
	template <> inline const VertexAttribute<int32_t>::ElementTypeInfo & VertexAttribute<int32_t>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_FIXED}; return info;
	}
#endif

template <> inline const VertexAttribute<vec2>::ElementTypeInfo & VertexAttribute<vec2>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {2, GL_FALSE, GL_FLOAT}; return info;
}

template <> inline const VertexAttribute<vec3>::ElementTypeInfo & VertexAttribute<vec3>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {3, GL_FALSE, GL_FLOAT}; return info;
}

template <> inline const VertexAttribute<vec4>::ElementTypeInfo & VertexAttribute<vec4>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {4, GL_FALSE, GL_FLOAT}; return info;
}

//TODO: make this work for textures