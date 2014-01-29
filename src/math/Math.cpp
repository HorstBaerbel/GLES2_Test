#include "Math.h"

#include <math.h>


float Math::acos(float x)
{
#ifdef USE_LIBRARY_FUNCTIONS
	return ::acosf(x);
#else
	return sqrtf(1.0f-x) * (1.5707963267948966192313216916398f + x*(-0.213300989f + x*(0.077980478f + x*-0.02164095f)));
#endif
}

float Math::asin(float x)
{
#ifdef USE_LIBRARY_FUNCTIONS
	return ::asinf(x);
#else
	return sqrtf(1.0f-x) * (1.5707963267948966192313216916398f + x*(-0.213300989f + x*(0.077980478f + x*-0.02164095f))) + (float)M_PI_2;
#endif
}

float Math::atan(float x)
{
	return ::atanf(x);
}

float Math::atan2(float y, float x)
{
	return ::atan2f(y, x);
	//public double aTan2(double y, double x) {	double coeff_1 = Math.PI / 4d;	double coeff_2 = 3d * coeff_1;	double abs_y = Math.abs(y);	double angle;	if (x >= 0d) {		double r = (x - abs_y) / (x + abs_y);		angle = coeff_1 - coeff_1 * r;	} else {		double r = (x + abs_y) / (abs_y - x);		angle = coeff_2 - coeff_1 * r;	}	return y < 0d ? -angle : angle;}
}

//from: http://www.devmaster.net/forums/showthread.php?t=5784
float Math::cos(float x)
{
#ifdef USE_LIBRARY_FUNCTIONS
	return ::cosf(x);
#else
	//bring into range [-1,1]
	x = (x + (float)M_PI_2) * (float)M_1_PI;
	//wrap around
	volatile float z = (x + 25165824.0f); //must be volatile else it will be optimized out
	x -= (z - 25165824.0f);
	//low precision approximation: return 4.0f * (x - x * fabs(x));
	//higher precision path
    float y = x - x * abs(x);
	//next iteration for higher precision
	const float Q = 3.1f;
	const float P = 3.6f;
	return y * (Q + P * abs(y));
#endif
}

float Math::sin(float x)
{
#ifdef USE_LIBRARY_FUNCTIONS
	return ::sinf(x);
#else
	//bring into range [-1,1]
	x *= (float)M_1_PI;
	//wrap around
	volatile float z = (x + 25165824.0f); //must be volatile else it will be optimized out
	x -= (z - 25165824.0f);
	//low precision approximation: return 4.0f * (x - x * fabs(x));
	//higher precision path
    float y = x - x * abs(x);
	//next iteration for higher precision
	const float Q = 3.1f;
	const float P = 3.6f;
	return y * (Q + P * abs(y));
#endif
}

float Math::tan(float x)
{
	return ::tanhf(x);
}

float Math::cosh(float x)
{
	return ::coshf(x);
}

float Math::sinh(float x)
{
	return ::sinhf(x);
}

float Math::tanh(float x)
{
	return ::tanhf(x);
}

float Math::exp(float x)
{
	return ::expf(x);
}

float Math::log(float x)
{
	return ::logf(x);
}

float Math::log10(float x)
{
	return ::log10f(x);
}

float Math::modf(float x, float * y)
{
	return ::modff(x, y);
}

float Math::pow(float x, float y)
{
	return ::powf(x, y);
}

float Math::sqrt(float x)
{
	return ::sqrtf(x);
}

float Math::invsqrt(float x)
{
#ifdef USE_LIBRARY_FUNCTIONS
	return 1.0f / ::sqrtf(x);
#else
   float xhalf = 0.5f * x;
   int i=*(int*)&x;
   i = 0x5f3759df - (i >> 1);
   x = *(float*)&i;
   x *= (1.5f - xhalf * x * x);
   //x *= (1.5f - xhalf * x * x); repeat Newton-Raphson step for higher precision
   return x;
#endif
}

float Math::abs(float x)
{
#ifdef USE_LIBRARY_FUNCTIONS
	return ::fabs(x);
#else
	int f = ((*(int *)(&x)) & 0x7FFFFFFF);
	return *(float *)(&f);
#endif
}

float Math::ceil(float x)
{
	return ::ceilf(x);
}

int Math::ceili(float x)
{
	return (int)::ceilf(x);
}

float Math::floor(float x)
{
	return ::floorf(x);
}

int Math::floori(float x)
{
	return (int)::floorf(x);
}

float Math::trunc(float x)
{
	return (float)((int)x);
}

int Math::trunci(float x)
{
	return (int)x;
}

float Math::fmod(float x, float y)
{
	return ::fmodf(x, y);
}

//From: http://www.iquilezles.org/www/articles/sfrand/sfrand.htm
float Math::rand(int * seed)
{
    float res;
    seed[0] *= 16807;
    *((unsigned int *) &res) = ( ((unsigned int)seed[0])>>9 ) | 0x40000000;
    return(res-3.0f);
}
