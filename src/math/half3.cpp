#include "half3.h"


inline half3::half3(const vec3 & right)
{
	n[0] = FastHalfCompressor::toHalf(right(0));
	n[1] = FastHalfCompressor::toHalf(right(1));
	n[2] = FastHalfCompressor::toHalf(right(2));
}

static void half3::toHalf3(half3 * destination, const vec3 * source, const size_t n)
{
	FastHalfCompressor::toHalf(destination, (float *)source, n*3);
}

//------------------------------------------------------------------------------------------------------------------------

std::ostream & operator<<(std::ostream & os, const half3 & right)
{
    for (int i = 0; i < 3; i++) {
        os << right.n[i] << " ";
    }
    return os;
}

std::istream & operator>>(std::istream & is, half3 & right)
{
    for (int i = 0; i < 3; i++) {
        is >> right.n[i];
    }
    return is;
}
