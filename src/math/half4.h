#pragma once

#include "math.h"
#include "half.h"


struct vec4;


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

    inline half4(const half _x, const half _y, const float _z, const float _w)
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

    inline half2(const float * fv)
    {
        n[0] = FastHalfCompressor::toHalf(fv[0]);
        n[1] = FastHalfCompressor::toHalf(fv[1]);
		n[2] = FastHalfCompressor::toHalf(fv[2]);
		n[3] = FastHalfCompressor::toHalf(fv[3]);
    }

    inline half4(const vec4 & right);

    inline half4(const half4 & right)
    {
		n = right.n;
    }

	//Cast operator
	inline operator half * () { return reinterpret_cast<half *>(this); }
	//Const cast operator
	inline operator const half * () const { return reinterpret_cast<const half *>(this); }

	//array conversion
	static void tohalf4(half4 * destination, const vec4 * source, const size_t n);
	
	//Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const half4 & right);
    friend std::istream & operator>>(std::istream & is, half4 & right);
};