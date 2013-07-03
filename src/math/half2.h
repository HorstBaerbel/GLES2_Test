#pragma once

#include "half.h"


struct half2
{
public:
	//The vector values in different formats
    union {
		struct {
			half x, y;
		};
		struct {
			half s, t;
		};
		half n[2];
	};

	//Constructors and copy constructor
    inline half2()
    {
        n[0] = n[1] = 0;
    }

    inline half2(const half h)
    {
        n[0] = n[1] = h;
    }

    inline half2(const float f)
    {
		n[0] = n[1] = FastHalfCompressor::toHalf(f);
    }

    inline half2(const half _x, const half _y)
    {
        n[0] = _x;
        n[1] = _y;
    }

    inline half2(const float _x, const float _y)
    {
        n[0] = FastHalfCompressor::toHalf(_x);
        n[1] = FastHalfCompressor::toHalf(_y);
    }

    inline half2(const half * fv)
    {
        n[0] = fv[0];
        n[1] = fv[1];
    }

    inline half2(const float * fv)
    {
        n[0] = FastHalfCompressor::toHalf(fv[0]);
        n[1] = FastHalfCompressor::toHalf(fv[1]);
    }

	inline half2(const vec2 & right)
	{
		n[0] = FastHalfCompressor::toHalf(right(0));
		n[1] = FastHalfCompressor::toHalf(right(1));
	}

	//Cast operator
	inline operator half * () { return reinterpret_cast<half *>(this); }
	//Const cast operator
	inline operator const half * () const { return reinterpret_cast<const half *>(this); }

    //Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const half2 & right);
    friend std::istream & operator>>(std::istream & is, half2 & right);
};

//------------------------------------------------------------------------------------------------------------------------

inline void toHalf(half2 * destination, const vec2 * source, const size_t n)
{
	FastHalfCompressor::toHalf((half *)destination, (float *)source, n*2);
}

inline std::ostream & operator<<(std::ostream & os, const half2 & right)
{
    for (int i = 0; i < 2; i++) {
        os << right.n[i] << " ";
    }
    return os;
}

inline std::istream & operator>>(std::istream & is, half2 & right)
{
    for (int i = 0; i < 2; i++) {
        is >> right.n[i];
    }
    return is;
}
