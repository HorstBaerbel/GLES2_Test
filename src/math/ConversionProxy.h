#pragma once

#include "Math.h"


class ConversionProxyBase
{
protected:
	const void * source;
	size_t count;

public:
	ConversionProxyBase(const void * s, size_t c) : source(s), count(c) {}
	size_t getCount() const { return count; }
	virtual void convertTo(void * destination) const = 0;
};

template <typename DESTINATION, typename SOURCE>
class ConversionProxy : ConversionProxyBase
{
	ConversionProxy(const SOURCE * source, size_t count) : ConversionProxyBase(source, count) {}
	void convertTo(void * destination) const;
};

template <>
inline void ConversionProxy<half2, vec2>::convertTo(void * destination) const
{
	toHalf((half2 *)destination, (vec2 *)source, count);
}

template <>
inline void ConversionProxy<half3, vec3>::convertTo(void * destination) const
{
	toHalf((half3 *)destination, (vec3 *)source, count);
}

template <>
inline void ConversionProxy<half4, vec4>::convertTo(void * destination) const
{
	toHalf((half4 *)destination, (vec4 *)source, count);
}
