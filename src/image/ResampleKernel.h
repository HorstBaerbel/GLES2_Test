#pragma once

#include <math.h>


inline float FunctionNearest(const float & x) { return x; }
inline float FunctionLinear(const float & x) { float value = fabs(x); return value < 1.0f ? 1.0f - value : 0.0f; }
inline float FunctionBox(const float & x) { float value = fabs(x); return value <= 0.5f ? 1.0f : 0.0f; }

struct ResampleFilter
{
    float support;
    float (*function)(const float &);

    ResampleFilter() : support(0.0f), function(nullptr) {}
    ResampleFilter(float (*func)(const float &), float supp) : support(supp), function(func) {}
    bool operator==(const ResampleFilter & b) const { return (support == b.support && function == b.function); }
    bool operator!=(const ResampleFilter & b) const { return (support != b.support || function != b.function); }
};

struct ResampleNearest : public ResampleFilter
{
    ResampleNearest() : ResampleFilter(FunctionNearest, 0.0f) {}
};

struct ResampleLinear : public ResampleFilter
{
    ResampleLinear() : ResampleFilter(FunctionLinear, 1.0f) {}
};

struct ResampleBox : public ResampleFilter
{
    ResampleBox() : ResampleFilter(FunctionBox, 0.5f) {}
};

//-------------------------------------------------------------------------------------------------

struct KernelWeights
{
    float * weights; //!< Weigths for each pixel for a block of pixels. See size.
    float invSum; //!< Inverse sum of all weights.
    size_t start; //!< Start of range to calculate weights. This is always >= 0.
    size_t end; //!< end of range to calculate weights. This is always < \sa destSize passed in constructor.
};

//-------------------------------------------------------------------------------------------------

class ResampleKernel
{
private:
    ResampleFilter m_kernel;
    size_t m_destSize;
    size_t m_srcSize;
    float * m_weightStorage;
    KernelWeights * m_weights;

public:
    /*!
    Constructor.
    */
    ResampleKernel();

    /*!
    Destructor. Frees all stuff.
    */
    ~ResampleKernel();

    /*!
    Retrieve weigths for kernel and sizes set. The weights will be cached and re-calculated if need be.
    \param[in] filter Resampling filter structure.
    \param[in] destSize Destination size of image. Either horizontal or vertical.
    \param[in] srcSize Source size of image. Either horizontal or vertical.
    \return Returns \sa destSize filled \sa Weights structures for resampling.
    */
    const KernelWeights * getWeigths(ResampleFilter filter, size_t destSize, size_t srcSize);
};
