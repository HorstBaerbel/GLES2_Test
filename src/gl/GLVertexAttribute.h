#pragma once

#include <memory>

#include "../math/Math.h"
#include "../math/DataProxy.h"
#include "GLVertexBase.h"

//------------------------------------------------------------------------------------------------------

/*!
Type-dependent vertex attribute class.
\note Don't forget to re-implment getElementTypeInfo for new template values...
*/
template <typename TYPE>
class GLVertexAttribute : public GLVertexAttributeBase
{
	std::vector<TYPE> data; //!<Raw data contained in the buffer (either this or the \dataProxy must be filled).
    std::shared_ptr<DataProxyBase> dataProxy; //!<Proxy class for copying/converting data (either this or \data must be filled).
	size_t oldDataSize; //!<old raw size of data copied to the GPU last time. For optimization only.
	GLuint boundIndex; //!<The slot index this attribute was last bound to.
	GLuint glBufferId;  //!<The OpenGL VBO id.

public:
	GLVertexAttribute(std::shared_ptr<ContextBase> & context, GLVertexAttributeBase::AttributeRole role, GLenum usagePattern = GL_DYNAMIC_DRAW);

    /*!
    Append single element to VBO data.
    \param[in] element Element to append.
    */
	void addElement(const TYPE & element);

    /*!
    Append elements to VBO data.
    \param[in] elements Pointer to elements to append.
    \param[in] count Number of elements to append from \elements.
    */
	void addElements(const TYPE * elements, size_t count);
	//void addElements(const void * rawElements, size_t count);

    /*!
    Set single element in VBO data.
    \param[in] element Element to append.
    \param[in] index Index of element to set.
    */
	void setElement(const TYPE & element, const size_t index);

    /*!
    Set elements in VBO.
    \param[in] elements Pointer to elements to set in VBO data.
    \param[in] count Number of element to set from \elements.
    */
	void setElements(const TYPE * elements, size_t count);

    /*!
    Set elements in VBO.
    \param[in] proxy Proxy object to get data from.
    \note The whole VBO will be overwritten with whatever comes from \proxy!
    */
    void setElements(const DataProxyBase & proxy);

    /*!
    Set proxy object that this VBO will get its data from.
    \param[in] proxy Proxy object to get data from.
    \note Data will be pulled in when \bind is called and the proxy or its data has been changed.
    */
    void setProxy(std::shared_ptr<DataProxyBase> & proxy);

	size_t getElementSize() const override;
	size_t getElementCount() const override;
	GLenum getElementGLType() const override;
	size_t getElementNrOfComponents() const override;

	const void * getRawData() const override;
	size_t getRawSize() const override;

	const ElementTypeInfo & getElementTypeInfo() const override;

	bool bind(std::shared_ptr<ParameterBase> parameter = nullptr) override;
	bool unbind(std::shared_ptr<ParameterBase> parameter = nullptr) override;

	~GLVertexAttribute();
};

//------------------------------------------------------------------------------------------------------

template <typename TYPE>
inline GLVertexAttribute<TYPE>::GLVertexAttribute(std::shared_ptr<ContextBase> & context, GLVertexAttributeBase::AttributeRole role,GLenum usagePattern)
	: GLVertexAttributeBase(context, role, usagePattern), oldDataSize(0), boundIndex(0), glBufferId(-1)
{
	elementTypeInfo = getElementTypeInfo();
	glContext->makeCurrent();
	glContext->glGenBuffers(1, &glBufferId);
	if (glBufferId == 0) {
		throw GLVertexAttributeException("VertexAttribute() - failed to generate vertex buffer!");
	}
	valid = true;
}

template <typename TYPE>
inline void GLVertexAttribute<TYPE>::addElement(const TYPE & element)
{
	data.push_back(element);
	changed = true;
}

template <typename TYPE>
inline void GLVertexAttribute<TYPE>::addElements(const TYPE * elements, size_t count)
{
	const size_t currentSize = data.size();
	data.resize(currentSize + count);
	memcpy(data.data() + sizeof(TYPE) * currentSize, elements, sizeof(TYPE) * count);
	changed = true;
}

//TODO: This sucks and will probably break very soon...
/*template <typename TYPE>
inline void GLVertexAttribute<TYPE>::addElements(const void * rawElements, size_t count)
{
	const TYPE * elements = (const TYPE *)rawElements;
	for (size_t i = 0; i < count; i++) {
		data.push_back(elements[i]);
	}
	changed = true;
}*/

template <typename TYPE>
inline void GLVertexAttribute<TYPE>::setElement(const TYPE & element, const size_t index)
{
	data[index] = element;
	changed = true;
}

template <typename TYPE>
inline void GLVertexAttribute<TYPE>::setElements(const TYPE * elements, size_t count)
{
	if (data.size() < count) {
		//only resize if count is bigger than current size
		data.resize(count);
	}
	memcpy(data.data(), elements, sizeof(TYPE) * count);
	changed = true;
}

template <typename TYPE>
inline void GLVertexAttribute<TYPE>::setElements(const DataProxyBase & proxy)
{
	const size_t newCount = proxy.getCount();
	if (data.size() < newCount) {
		//only resize if count is bigger than current size
		data.resize(newCount);
	}
	proxy.copyTo(data.data());
    //do not proxy change status
	changed = true;
}

template <typename TYPE>
inline void GLVertexAttribute<TYPE>::setProxy(std::shared_ptr<DataProxyBase> & proxy)
{
    dataProxy = proxy;
}

template <typename TYPE>
inline size_t GLVertexAttribute<TYPE>::getElementSize() const
{
	return sizeof(TYPE);
}

template <typename TYPE>
inline size_t GLVertexAttribute<TYPE>::getElementCount() const
{
    if (dataProxy) {
        return dataProxy->getCount();
    }
    else {
	    return data.size();
    }
}

template <typename TYPE>
size_t GLVertexAttribute<TYPE>::getElementNrOfComponents() const
{
	return elementTypeInfo.nrOfComponents;
}

template <typename TYPE>
GLenum GLVertexAttribute<TYPE>::getElementGLType() const
{
	return elementTypeInfo.glType;
}

template <typename TYPE>
inline const void * GLVertexAttribute<TYPE>::getRawData() const
{
    if (dataProxy) {
        //we can only directly copy from a data proxy, not get a pointer to its memory
        return nullptr;
    }
    else {
	    return data.data();
    }
}

template <typename TYPE>
inline size_t GLVertexAttribute<TYPE>::getRawSize() const
{
    if (dataProxy) {
        return dataProxy->getCount() * sizeof(TYPE);
    }
    else {
	    return data.size() * sizeof(TYPE);
    }
}

template <typename TYPE>
inline bool GLVertexAttribute<TYPE>::bind(std::shared_ptr<ParameterBase> parameter)
{
    if (dataProxy) {
        changed = dataProxy->hasChanged();
    }
	//enable VBOs
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
                if (dataProxy) {
                    dataProxy->copyTo(buffer);
                }
                else {
				    memcpy(buffer, getRawData(), dataSize);
                }
				glContext->glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
				changed = false;
			}
            else {
                throw GLVertexAttributeException("VertexAttribute::bind - glMapBuffer(GL_ELEMENT_ARRAY_BUFFER) failed: " + ContextBase::glErrorToString(glGetError()) + "!" );
            }
		}
	}
	else {
		//clear bound index
		boundIndex = 0;
		//try reading new attribute index parameter
		if (parameter) {
			//try to cast to GLuint
			std::shared_ptr<Parameter<GLuint>> index = std::dynamic_pointer_cast<Parameter<GLuint>>(parameter);
			if (index) {
				//ok. set new index
				boundIndex = *index;
			}
			else {
				throw GLVertexAttributeException("VertexAttribute::bind - Parameter must be attribute index passed as Parameter<GLuint>!");
			}
		}
		else {
			throw GLVertexAttributeException("VertexAttribute::bind - Parameter cannot be empty!");
		}
		//enable this attribute array
		glContext->glEnableVertexAttribArray(boundIndex);
		//bind buffer to it
		glContext->glBindBuffer(GL_ARRAY_BUFFER, glBufferId);
		if (changed) {
			//(re-)allocate buffer if necessary
			const size_t dataSize = getRawSize();
			if (dataSize > oldDataSize) {
				glContext->glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, glUsage);
				oldDataSize = dataSize;
			}
			//map buffer to user space
			GLvoid * buffer = glContext->glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			if (buffer != nullptr) {
				//now add data
				if (dataProxy) {
                    dataProxy->copyTo(buffer);
                }
                else {
				    memcpy(buffer, getRawData(), dataSize);
                }
				glContext->glUnmapBuffer(GL_ARRAY_BUFFER);
				changed = false;
			}
            else {
                throw GLVertexAttributeException("VertexAttribute::bind - glMapBuffer(GL_ARRAY_BUFFER) failed: " + ContextBase::glErrorToString(glGetError()) + "!" );
            }
		}
		//set up attribute pointer
		glContext->glVertexAttribPointer(boundIndex, elementTypeInfo.nrOfComponents, elementTypeInfo.glType, elementTypeInfo.normalize, 0, nullptr);
		//unbid buffer again. this saves us a call later
		glContext->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
    if (dataProxy) {
        dataProxy->setChanged(changed);
    }
	return true;
}

template <typename TYPE>
inline bool GLVertexAttribute<TYPE>::unbind(std::shared_ptr<ParameterBase> parameter)
{
	//check if this is an index array to an actual vertex attribute
	if (INDEX == attributeRole) {
		glContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else {
		glContext->glDisableVertexAttribArray(boundIndex);
		glContext->glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	return true;
}

template <typename TYPE>
inline GLVertexAttribute<TYPE>::~GLVertexAttribute()
{
	glContext->glDeleteBuffers(1, &glBufferId);
	glBufferId = 0;
}

//------------------------------------------------------------------------------------------------------
//Type traits for element info

template <> inline const GLVertexAttribute<float>::ElementTypeInfo & GLVertexAttribute<float>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_FLOAT}; return info;
}

#ifdef USE_OPENGL_DESKTOP
	template <> inline const GLVertexAttribute<double>::ElementTypeInfo & GLVertexAttribute<double>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_DOUBLE}; return info;
	}
#endif

template <> inline const GLVertexAttribute<int8_t>::ElementTypeInfo & GLVertexAttribute<int8_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_BYTE}; return info;
}

template <> inline const GLVertexAttribute<uint8_t>::ElementTypeInfo & GLVertexAttribute<uint8_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_UNSIGNED_BYTE}; return info;
}

template <> inline const GLVertexAttribute<int16_t>::ElementTypeInfo & GLVertexAttribute<int16_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_SHORT}; return info;
}

template <> inline const GLVertexAttribute<uint16_t>::ElementTypeInfo & GLVertexAttribute<uint16_t>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {1, GL_FALSE, GL_UNSIGNED_SHORT}; return info;
}

#ifdef USE_OPENGL_DESKTOP
	template <> inline const GLVertexAttribute<int32_t>::ElementTypeInfo & GLVertexAttribute<int32_t>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_INT}; return info;
	}

	template <> inline const GLVertexAttribute<uint32_t>::ElementTypeInfo & GLVertexAttribute<uint32_t>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_UNSIGNED_INT}; return info;
	}
#else
	template <> inline const GLVertexAttribute<int32_t>::ElementTypeInfo & GLVertexAttribute<int32_t>::getElementTypeInfo() const
	{
		static const ElementTypeInfo info = {1, GL_FALSE, GL_FIXED}; return info;
	}
#endif

template <> inline const GLVertexAttribute<vec2>::ElementTypeInfo & GLVertexAttribute<vec2>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {2, GL_FALSE, GL_FLOAT}; return info;
}

template <> inline const GLVertexAttribute<vec3>::ElementTypeInfo & GLVertexAttribute<vec3>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {3, GL_FALSE, GL_FLOAT}; return info;
}

template <> inline const GLVertexAttribute<vec4>::ElementTypeInfo & GLVertexAttribute<vec4>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {4, GL_FALSE, GL_FLOAT}; return info;
}

template <> inline const GLVertexAttribute<half2>::ElementTypeInfo & GLVertexAttribute<half2>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {2, GL_FALSE, GL_HALF_FLOAT}; return info;
}

template <> inline const GLVertexAttribute<half3>::ElementTypeInfo & GLVertexAttribute<half3>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {3, GL_FALSE, GL_HALF_FLOAT}; return info;
}

template <> inline const GLVertexAttribute<half4>::ElementTypeInfo & GLVertexAttribute<half4>::getElementTypeInfo() const
{
	static const ElementTypeInfo info = {4, GL_FALSE, GL_HALF_FLOAT}; return info;
}

//TODO: make this work for textures