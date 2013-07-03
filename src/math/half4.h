#pragma once

#include "half.h"


struct half4
{
public:
	//The vector values in different formats
    union {
		struct {
			half x, y, z, w;
		};
		struct {
			half r, g, b, a;
		};
		half n[4];
	};

	//Constructors and copy constructor
    inline half4()
    {
        n[0] = n[1] = n[2] = n[3] = 0;
    }

    inline half4(const half h)
    {
        n[0] = n[1] = n[2] = n[3] = h;
    }

    inline half4(const float f)
    {
		n[0] = n[1] = n[2] = n[3] = FastHalfCompressor::toHalf(f);
    }

    inline half4(const half _x, const half _y, const half _z, const half _w)
    {
        n[0] = _x;
        n[1] = _y;
		n[2] = _z;
		n[3] = _w;
    }

    inline half4(const float _x, const float _y, const float _z, const float _w)
    {
        n[0] = FastHalfCompressor::toHalf(_x);
        n[1] = FastHalfCompressor::toHalf(_y);
		n[2] = FastHalfCompressor::toHalf(_z);
		n[3] = FastHalfCompressor::toHalf(_w);
    }

    inline half4(const half * fv)
    {
        n[0] = fv[0];
        n[1] = fv[1];
		n[2] = fv[2];
		n[3] = fv[3];
    }

    inline half4(const float * fv)
    {
        n[0] = FastHalfCompressor::toHalf(fv[0]);
        n[1] = FastHalfCompressor::toHalf(fv[1]);
		n[2] = FastHalfCompressor::toHalf(fv[2]);
		n[3] = FastHalfCompressor::toHalf(fv[3]);
    }

	inline half4(const vec4 & right)
	{
		n[0] = FastHalfCompressor::toHalf(right(0));
		n[1] = FastHalfCompressor::toHalf(right(1));
		n[2] = FastHalfCompressor::toHalf(right(2));
		n[3] = FastHalfCompressor::toHalf(right(3));
	}

	//Cast operator
	inline operator half * () { return reinterpret_cast<half *>(this); }
	//Const cast operator
	inline operator const half * () const { return reinterpret_cast<const half *>(this); }

	//array conversion
	static void toHalf4(half4 * destination, const vec4 * source, const size_t n)
	{
		FastHalfCompressor::toHalf((half *)destination, (float *)source, n*4);
	}
	
	//Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const half4 & right);
    friend std::istream & operator>>(std::istream & is, half4 & right);
};

//------------------------------------------------------------------------------------------------------------------------

inline std::ostream & operator<<(std::ostream & os, const half4 & right)
{
    for (int i = 0; i < 4; i++) {
        os << right.n[i] << " ";
    }
    return os;
}

inline std::istream & operator>>(std::istream & is, half4 & right)
{
    for (int i = 0; i < 4; i++) {
        is >> right.n[i];
    }
    return is;
}