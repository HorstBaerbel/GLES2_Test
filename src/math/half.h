#pragma once

#include <stdint.h>
#include <stdlib.h>


//"Half" float value conversion routines.
//Based on this: http://stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion

typedef uint16_t half;


class FastHalfCompressor
{
	union Bits {
		float f;
		int32_t si;
		uint32_t ui;
	};

	static int const shift = 13;
	static int const shiftSign = 16;

	static int32_t const infN = 0x7F800000; // flt32 infinity
	static int32_t const maxN = 0x477FE000; // max flt16 normal as a flt32
	static int32_t const minN = 0x38800000; // min flt16 normal as a flt32
	static int32_t const signN = 0x80000000; // flt32 sign bit

	static int32_t const infC = infN >> shift;
	static int32_t const nanN = (infC + 1) << shift; // minimum flt16 nan as a flt32
	static int32_t const maxC = maxN >> shift;
	static int32_t const minC = minN >> shift;
	static int32_t const signC = signN >> shiftSign; // flt16 sign bit

	static int32_t const mulN = 0x52000000; // (1 << 23) / minN
	static int32_t const mulC = 0x33800000; // minN / (1 << (23 - shift))

	static int32_t const subC = 0x003FF; // max flt32 subnormal down shifted
	static int32_t const norC = 0x00400; // min flt32 normal down shifted

	static int32_t const maxD = infC - maxC - 1;
	static int32_t const minD = minC - subC - 1;

public:
    /*!
    Convert float value to half-float value (GL_HALF / IEEE 754).
    */
	static inline half toHalf(const float & value)
	{
		Bits v, s;
		v.f = value;
		uint32_t sign = v.si & signN;
		v.si ^= sign;
		sign >>= shiftSign; // logical shift
		s.si = mulN;
		s.si = (uint32_t)(s.f * v.f); // correct subnormals
		v.si ^= (s.si ^ v.si) & -(minN > v.si);
		v.si ^= (infN ^ v.si) & -((infN > v.si) & (v.si > maxN));
		v.si ^= (nanN ^ v.si) & -((nanN > v.si) & (v.si > infN));
		v.ui >>= shift; // logical shift
		v.si ^= ((v.si - maxD) ^ v.si) & -(v.si > maxC);
		v.si ^= ((v.si - minD) ^ v.si) & -(v.si > subC);
		return v.ui | sign;
	}

    /*!
    Convert half-float value to float value (GL_HALF / IEEE 754).
    */
	static inline float toFloat(const half & value)
	{
		Bits v;
		v.ui = value;
		int32_t sign = v.si & signC;
		v.si ^= sign;
		sign <<= shiftSign;
		v.si ^= ((v.si + minD) ^ v.si) & -(v.si > subC);
		v.si ^= ((v.si + maxD) ^ v.si) & -(v.si > maxC);
		Bits s;
		s.si = mulC;
		s.f *= v.si;
		int32_t mask = -(norC > v.si);
		v.si <<= shift;
		v.si ^= (s.si ^ v.si) & mask;
		v.si |= sign;
		return v.f;
	}
};

/*!
Convert array of float values to array of half-float values (GL_HALF / IEEE 754).
*/
inline void convert(half * destination, const float * source, const size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		destination[i] = FastHalfCompressor::toHalf(source[i]);
	}
}

/*!
Convert array of half-float values to array of float values (GL_HALF / IEEE 754).
*/
inline void convert(float * destination, const half * source, const size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		destination[i] = FastHalfCompressor::toFloat(source[i]);
	}
}


class PreciseHalfCompressor
{
	union Bits {
		float f;
		int32_t si;
		uint32_t ui;
	};

	bool hasNegatives;
	bool noLoss;
	int32_t _maxF;
	int32_t _minF;
	int32_t _epsF;
	int32_t _maxC;
	int32_t _zeroC;
	int32_t _pDelta;
	int32_t _nDelta;
	int _shift;

	static int32_t const signF = 0x80000000;
	static int32_t const absF = ~signF;

public:
	PreciseHalfCompressor(float min, float epsilon, float max, int precision);
	float clamp(float value);
	uint32_t toHalf(float value);
	float toFloat(uint32_t value);
};

//Turn off warning C4293, else we get hundreds of warnings
#pragma warning(disable : 4293)

/*Table based-approach
/* These macros implement a finite iterator useful to build lookup
 * tables. For instance, S64(0) will call S1(x) for all values of x
 * between 0 and 63.
 * Due to the exponential behaviour of the calls, the stress on the
 * compiler may be important. */
#define S4(x)    S1((x)),   S1((x)+1),     S1((x)+2),     S1((x)+3)
#define S16(x)   S4((x)),   S4((x)+4),     S4((x)+8),     S4((x)+12)
#define S64(x)   S16((x)),  S16((x)+16),   S16((x)+32),   S16((x)+48)
#define S256(x)  S64((x)),  S64((x)+64),   S64((x)+128),  S64((x)+192)
#define S1024(x) S256((x)), S256((x)+256), S256((x)+512), S256((x)+768)

/* Lookup table-based algorithm from “Fast Half Float Conversions”
 * by Jeroen van der Zijp, November 2008. No rounding is performed,
 * and some NaN values may be incorrectly converted to Inf. */
static inline uint16_t float_to_half_nobranch(uint32_t x)
{
    static uint16_t const basetable[512] =
    {
#define S1(i) (((i) < 103) ? 0x0000 : \
               ((i) < 113) ? 0x0400 >> (113 - (i)) : \
               ((i) < 143) ? ((i) - 112) << 10 : 0x7c00)
        S256(0),
#undef S1
#define S1(i) (0x8000 | (((i) < 103) ? 0x0000 : \
                         ((i) < 113) ? 0x0400 >> (113 - (i)) : \
                         ((i) < 143) ? ((i) - 112) << 10 : 0x7c00))
        S256(0),
#undef S1
    };

    static uint8_t const shifttable[512] =
    {
#define S1(i) (((i) < 103) ? 24 : \
               ((i) < 113) ? 126 - (i) : \
               ((i) < 143 || (i) == 255) ? 13 : 24)
        S256(0), S256(0),
#undef S1
    };

    uint16_t bits = basetable[(x >> 23) & 0x1ff];
    bits |= (x & 0x007fffff) >> shifttable[(x >> 23) & 0x1ff];
    return bits;
}

//Turn warning C4293 on again
#pragma warning(default : 4293)

/*
/* This method is faster than the OpenEXR implementation (very often
 * used, eg. in Ogre), with the additional benefit of rounding, inspired
 * by James Tursa’s half-precision code. */
static inline uint16_t float_to_half_branch(uint32_t x)
{
    uint16_t bits = (x >> 16) & 0x8000; /* Get the sign */
    uint16_t m = (x >> 12) & 0x07ff; /* Keep one extra bit for rounding */
    unsigned int e = (x >> 23) & 0xff; /* Using int is faster here */

    /* If zero, or denormal, or exponent underflows too much for a denormal
     * half, return signed zero. */
    if (e < 103)
        return bits;

    /* If NaN, return NaN. If Inf or exponent overflow, return Inf. */
    if (e > 142)
    {
        bits |= 0x7c00u;
        /* If exponent was 0xff and one mantissa bit was set, it means NaN,
         * not Inf, so make sure we set one mantissa bit too. */
        bits |= e == 255 && (x & 0x007fffffu);
        return bits;
    }

    /* If exponent underflows but not too much, return a denormal */
    if (e < 113)
    {
        m |= 0x0800u;
        /* Extra rounding may overflow and set mantissa to 0 and exponent
         * to 1, which is OK. */
        bits |= (m >> (114 - e)) + ((m >> (113 - e)) & 1);
        return bits;
    }

    bits |= ((e - 112) << 10) | (m >> 1);
    /* Extra rounding. An overflow will set mantissa to 0 and increment
     * the exponent, which is OK. */
    bits += m & 1;
    return bits;
}
