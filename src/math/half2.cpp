#include "half2.h"


inline half2::half2(const vec2 & right)
{
	n[0] = FastHalfCompressor::toHalf(right(0));
	n[1] = FastHalfCompressor::toHalf(right(1));
}

static void half2::toHalf3(half2 * destination, const vec2 * source, const size_t n)
{
	FastHalfCompressor::toHalf(destination, (float *)source, n*2);
}

//------------------------------------------------------------------------------------------------------------------------

std::ostream & operator<<(std::ostream & os, const half2 & right)
{
    for (int i = 0; i < 2; i++) {
        os << right.n[i] << " ";
    }
    return os;
}

std::istream & operator>>(std::istream & is, half2 & right)
{
    for (int i = 0; i < 2; i++) {
        is >> right.n[i];
    }
    return is;
}
