#pragma once

#include "math.h"
#include "half.h"


struct vec2;


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

    inline half2(const vec2 & right);

    inline half2(const half2 & right)
    {
        n = right.n;
    }

	//Cast operator
	inline operator half * () { return reinterpret_cast<half *>(this); }
	//Const cast operator
	inline operator const half * () const { return reinterpret_cast<const half *>(this); }

	//array conversion
	static void toHalf2(half2 * destination, const vec2 * source, const size_t n);

    //Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const half2 & right);
    friend std::istream & operator>>(std::istream & is, half2 & right);
};