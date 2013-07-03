#pragma once

#include "../Base.h"
#include "Math.h"


class DataProxyBase : public IChangeableObject
{
protected:
	const void * source;
	size_t count;

    DataProxyBase(const void * s, size_t c) : IChangeableObject(), source(s), count(c) {}

public:
    void setSource(const void * newSource, size_t newCount) { source = newSource; count = newCount; changed = true; }
	size_t getCount() const { return count; }
	virtual void copyTo(void * destination) const = 0;
};

template <typename TYPE>
class DataProxy : public DataProxyBase
{
	DataProxy(const TYPE * source, size_t count) : ConversionProxyBase(source, count) {}
	void copyTo(void * destination) const
    {
        memcpy(destination, source, sizeof(TYPE) * count);
    };
};

template <typename DESTINATION_TYPE, typename SOURCE_TYPE>
class ConversionProxy : public DataProxyBase
{
	ConversionProxy(const SOURCE_TYPE * source, size_t count) : ConversionProxyBase(source, count) {}
	void copyTo(void * destination) const
    {
        convert((DESTINATION_TYPE)destination, (SOURCE_TYPE)source, count);
    };
};
