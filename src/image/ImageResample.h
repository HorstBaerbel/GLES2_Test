#pragma once

#include "PixelFormat.h"
#include "ResampleKernel.h"


class ImageResample
{
    ResampleKernel m_horizontalKernel;
    ResampleKernel m_verticalKernel;
    uint8_t * m_scaleBuffer;
    size_t m_scaleBufferWidth;
    size_t m_scaleBufferHeight;
    PixelInfo::FormatType m_formatType;

public:
    /*!
    Constructor.
    \param[in] kernel Resampling kernel structure. See ResampleKernels.h for information.
    */
    ImageResample(PixelInfo::FormatType formatType = PixelInfo::BAD_FORMAT);

    /*!
    Copy constructor. Does NOT copy the scalebuffer atm.
    */
    ImageResample(const ImageResample & b);

    /*!
    Copy operator. Does NOT copy the scalebuffer atm.
    */
    ImageResample & operator=(const ImageResample & b);

    /*!
    Scale image data from one size to another.
    \param[in] dest Output data.
    \param[in] destWidth Output width.
    \param[in] destHeight Output height.
    \param[in] src Input data.
    \param[in] srcWidth Input width.
    \param[in] srcHeight Input height.
    \param[in] formatType Input color pixel format.
    \param[in] scaleBuffer Temporary scale buffer of size destWidth x srcHeight. If nullptr it will be allocated internally.
    */
    void scaleImage(uint8_t * dest, size_t destWidth, size_t destHeight, const uint8_t * src, size_t srcWidth, size_t srcHeight, const ResampleFilter & filter);
};

//-------------------------------------------------------------------------------------------------

/*!
Vertically accumulate pixel colors to pixel while multiplying the new color with a factor. dest += src[i] * srcWeight[i];
\param[in] dest Destination data pointer.
\param[in] src Source data pointer.
\param[in] srcStride Byte-stride to start of next pixel. Set to PixelFormat<INTYPE>::bytesPerPixel for horizontal accumulation
\param[in] srcWeights Array of weights for source colors to multiply before adding.
*/
template <int INTYPE>
static inline void accumulatePixel(uint8_t * dest, const uint8_t * src, size_t srcStride, const KernelWeights * srcWeights)
{
    const uint8_t * srcStart = src + srcWeights->start * srcStride;
    const size_t srcCount =  srcWeights->end - srcWeights->start + 1;
    PixelFormat<INTYPE>::Pixel outPixel;
    //accumulate pixels and write to destination
    if (PixelFormat<INTYPE>::nrOfComponents == 1) {
        float value = 0.0f;
        for (size_t i = 0; i < srcCount; ++i) {
            //get pixel and color(s) from src
            PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src);
            value += PixelFormat<INTYPE>::getR(inPixel) * srcWeights->weights[i];
            //next pixel
            src += srcStride;
        }
        PixelFormat<INTYPE>::setR(outPixel, (PixelFormat<INTYPE>::Color)(value * srcWeights->invSum));
    }
    else if (PixelFormat<INTYPE>::nrOfComponents == 3) {
        float values[3] = {0.0f, 0.0f, 0.0f};
        for (size_t i = 0; i < srcCount; ++i) {
            //get pixel and color(s) from src
            PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src);
            values[0] += PixelFormat<INTYPE>::getR(inPixel) * srcWeights->weights[i];
            values[1] += PixelFormat<INTYPE>::getG(inPixel) * srcWeights->weights[i];
            values[2] += PixelFormat<INTYPE>::getB(inPixel) * srcWeights->weights[i];
            //next pixel
            src += srcStride;
        }
        PixelFormat<INTYPE>::setR(outPixel, (PixelFormat<INTYPE>::Color)(values[0] * srcWeights->invSum));
        PixelFormat<INTYPE>::setG(outPixel, (PixelFormat<INTYPE>::Color)(values[1] * srcWeights->invSum));
        PixelFormat<INTYPE>::setB(outPixel, (PixelFormat<INTYPE>::Color)(values[2] * srcWeights->invSum));
    }
    else if (PixelFormat<INTYPE>::nrOfComponents == 4) {
        float values[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        for (size_t i = 0; i < srcCount; ++i) {
            //get pixel and color(s) from src
            PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src);
            values[0] += PixelFormat<INTYPE>::getR(inPixel) * srcWeights->weights[i];
            values[1] += PixelFormat<INTYPE>::getG(inPixel) * srcWeights->weights[i];
            values[2] += PixelFormat<INTYPE>::getB(inPixel) * srcWeights->weights[i];
            values[3] += PixelFormat<INTYPE>::getA(inPixel) * srcWeights->weights[i];
            //next pixel
            src += srcStride;
        }
        PixelFormat<INTYPE>::setR(outPixel, (PixelFormat<INTYPE>::Color)(values[0] * srcWeights->invSum));
        PixelFormat<INTYPE>::setG(outPixel, (PixelFormat<INTYPE>::Color)(values[1] * srcWeights->invSum));
        PixelFormat<INTYPE>::setB(outPixel, (PixelFormat<INTYPE>::Color)(values[2] * srcWeights->invSum));
        PixelFormat<INTYPE>::setA(outPixel, (PixelFormat<INTYPE>::Color)(values[3] * srcWeights->invSum));
    }
    PixelFormat<INTYPE>::setPixel(dest, outPixel);
}

template <int INTYPE>
static inline void accumulatePixel(uint8_t * dest, size_t destStride, size_t count, const uint8_t * src, size_t srcStride, const KernelWeights * srcWeights)
{
    for (size_t i = 0; i < count; ++i) {
        //accumulate pixels and write to destination
        accumulatePixel<INTYPE>(dest, src, srcStride, srcWeights);
        //next scanline in destination image
        dest += destStride;
        srcWeights++;
    }
}

void accumulateWeighted(uint8_t * dest, size_t destStride, size_t count, const uint8_t * src, size_t srcStride, const KernelWeights * srcWeights, PixelInfo::FormatType m_formatType);
