#pragma once

#include "half.h"


struct half3
{
public:
	//The vector values in different formats
    union {
		struct {
			half x, y, z;
		};
		struct {
			half s, t, r;
		};
		half n[3];
	};

	//Constructors and copy constructor
    inline half3()
    {
        n[0] = n[1] = n[2] = 0;
    }

    inline half3(const half h)
    {
        n[0] = n[1] = n[2] = h;
    }

    inline half3(const float f)
    {
		n[0] = n[1] = n[2] = FastHalfCompressor::toHalf(f);
    }

    inline half3(const half _x, const half _y, const half _z)
    {
        n[0] = _x;
        n[1] = _y;
		n[2] = _z;
    }

    inline half3(const float _x, const float _y, const float _z)
    {
        n[0] = FastHalfCompressor::toHalf(_x);
        n[1] = FastHalfCompressor::toHalf(_y);
		n[2] = FastHalfCompressor::toHalf(_z);
    }

    inline half3(const half * fv)
    {
        n[0] = fv[0];
        n[1] = fv[1];
		n[2] = fv[2];
    }

    inline half3(const float * fv)
    {
        n[0] = FastHalfCompressor::toHalf(fv[0]);
        n[1] = FastHalfCompressor::toHalf(fv[1]);
		n[2] = FastHalfCompressor::toHalf(fv[2]);
    }

	inline half3(const vec3 & right)
	{
		n[0] = FastHalfCompressor::toHalf(right(0));
		n[1] = FastHalfCompressor::toHalf(right(1));
		n[2] = FastHalfCompressor::toHalf(right(2));
	}

	//Cast operator
	inline operator half * () { return reinterpret_cast<half *>(this); }
	//Const cast operator
	inline operator const half * () const { return reinterpret_cast<const half *>(this); }
	
	//Stream stuff
    friend std::ostream & operator<<(std::ostream & os, const half3 & right);
    friend std::istream & operator>>(std::istream & is, half3 & right);
};

//------------------------------------------------------------------------------------------------------------------------

inline void toHalf(half3 * destination, const vec3 * source, const size_t n)
{
	FastHalfCompressor::toHalf((half *)destination, (float *)source, n*3);
}

inline std::ostream & operator<<(std::ostream & os, const half3 & right)
{
    for (int i = 0; i < 3; i++) {
        os << right.n[i] << " ";
    }
    return os;
}

inline std::istream & operator>>(std::istream & is, half3 & right)
{
    for (int i = 0; i < 3; i++) {
        is >> right.n[i];
    }
    return is;
}
