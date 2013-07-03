#include "half.h"


inline half FastHalfCompressor::toHalf(const float & value)
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

void FastHalfCompressor::toHalf(half * destination, const float * source, const size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		destination[i] = toHalf(source[i]);
	}
}

inline float FastHalfCompressor::toFloat(const half & value)
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

void FastHalfCompressor::toFloat(float * destination, const half * source, const size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		destination[i] = toFloat(source[i]);
	}
}


PreciseHalfCompressor::PreciseHalfCompressor(float min, float epsilon, float max, int precision)
{
	// legal values
	// min <= 0 < epsilon < max
	// 0 <= precision <= 23
	_shift = 23 - precision;
	Bits v;
	v.f = min;
	_minF = v.si;
	v.f = epsilon;
	_epsF = v.si;
	v.f = max;
	_maxF = v.si;
	hasNegatives = _minF < 0;
	noLoss = _shift == 0;
	int32_t pepsU, nepsU;
	if(noLoss) {
		nepsU = _epsF;
		pepsU = _epsF ^ signF;
		_maxC = _maxF ^ signF;
		_zeroC = signF;
	} else {
		nepsU = uint32_t(_epsF ^ signF) >> _shift;
		pepsU = uint32_t(_epsF) >> _shift;
		_maxC = uint32_t(_maxF) >> _shift;
		_zeroC = 0;
	}
	_pDelta = pepsU - _zeroC - 1;
	_nDelta = nepsU - _maxC - 1;
}

float PreciseHalfCompressor::clamp(float value)
{
	Bits v;
	v.f = value;
	int32_t max = _maxF;
	if(hasNegatives)
		max ^= (_minF ^ _maxF) & -(0 > v.si);
	v.si ^= (max ^ v.si) & -(v.si > max);
	v.si &= -(_epsF <= (v.si & absF));
	return v.f;
}

uint32_t PreciseHalfCompressor::toHalf(float value)
{
	Bits v;
	v.f = clamp(value);
	if(noLoss)
		v.si ^= signF;
	else
		v.ui >>= _shift;
	if(hasNegatives)
		v.si ^= ((v.si - _nDelta) ^ v.si) & -(v.si > _maxC);
	v.si ^= ((v.si - _pDelta) ^ v.si) & -(v.si > _zeroC);
	if(noLoss)
		v.si ^= signF;
	return v.ui;
}

float PreciseHalfCompressor::toFloat(uint32_t value)
{
	Bits v;
	v.ui = value;
	if(noLoss)
		v.si ^= signF;
	v.si ^= ((v.si + _pDelta) ^ v.si) & -(v.si > _zeroC);
	if(hasNegatives)
		v.si ^= ((v.si + _nDelta) ^ v.si) & -(v.si > _maxC);
	if(noLoss)
		v.si ^= signF;
	else
		v.si <<= _shift;
	return v.f;
}
