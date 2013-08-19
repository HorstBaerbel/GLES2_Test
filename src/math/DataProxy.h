#pragma once

#include "../Base.h"
#include "Math.h"

/*!
Abstract data proxy base class to make conversion and lazy copies between objects possible.
*/
class DataProxyBase : public IChangeableObject
{
protected:
	const void * source; //!<Source data pointer.
	size_t count; //!<Number of elements in source data

    DataProxyBase(const void * s, size_t c) : IChangeableObject(), source(s), count(c) {}

public:
    /*!
    Set new data source for this proxy.
    \param[in] newSource Pointer to the new raw source data.
    \param[in] newCount New number of elements in the source data.
    */
    void setSource(const void * newSource, size_t newCount) { source = newSource; count = newCount; changed = true; }

	size_t getCount() const { return count; }

    /*!
    Overrride this to copy or convert source data to a destination.
    \param[in] destination Pointer to destination data buffer.
    \note destination will be overwritten with whatever is in source. Make sure destination is big enough!
    */
	virtual void copyTo(void * destination) const = 0;
};

/*!
Actual data proxy class. This copies source verbatim to destination.
*/
template <typename TYPE>
class DataProxy : public DataProxyBase
{
	DataProxy(const TYPE * source, size_t count) : DataProxyBase(source, count) {}

    void setSource(const TYPE * newSource, size_t newCount) { source = static_cast<void *>(newSource); count = newCount; changed = true; }

	void copyTo(void * destination) const
    {
        memcpy(destination, source, sizeof(TYPE) * count);
    };
};

/*!
Actual data conversion proxy class. This converts from source type to destination type.
\note You need to make sure a convert(DESTINATION_TYPE *, SOURCE_TYPE *, size_t) function exists for the types you want to use!
*/
template <typename DESTINATION_TYPE, typename SOURCE_TYPE>
class ConversionProxy : public DataProxyBase
{
	ConversionProxy(const SOURCE_TYPE * source, size_t count) : DataProxyBase(source, count) {}

    void setSource(const SOURCE_TYPE * newSource, size_t newCount) { source = static_cast<void *>(newSource); count = newCount; changed = true; }

	void copyTo(void * destination) const
    {
        convert((DESTINATION_TYPE)destination, (SOURCE_TYPE)source, count);
    };
};
