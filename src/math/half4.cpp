#include "half4.h"


inline half4::half4(const vec4 & right)
{
	n[0] = FastHalfCompressor::toHalf(right(0));
	n[1] = FastHalfCompressor::toHalf(right(1));
	n[2] = FastHalfCompressor::toHalf(right(2));
	n[3] = FastHalfCompressor::toHalf(right(3));
}

static void half4::tohalf4(half4 * destination, const vec4 * source, const size_t n)
{
	FastHalfCompressor::toHalf(destination, (float *)source, n*4);
}

//------------------------------------------------------------------------------------------------------------------------

std::ostream & operator<<(std::ostream & os, const half4 & right)
{
    for (int i = 0; i < 4; i++) {
        os << right.n[i] << " ";
    }
    return os;
}

std::istream & operator>>(std::istream & is, half4 & right)
{
    for (int i = 0; i < 4; i++) {
        is >> right.n[i];
    }
    return is;
}
