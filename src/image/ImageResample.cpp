#include "ImageResample.h"


ImageResample::ImageResample(PixelInfo::FormatType formatType)
    : m_formatType(formatType)
    , m_scaleBuffer(nullptr)
    , m_scaleBufferWidth(0)
    , m_scaleBufferHeight(0)
{
}

ImageResample::ImageResample(const ImageResample & b)
{
    *this = b;
}

ImageResample & ImageResample::operator=(const ImageResample & b)
{
    m_formatType = b.m_formatType;
    m_scaleBuffer = nullptr;
    m_scaleBufferWidth = b.m_scaleBufferWidth;
    m_scaleBufferHeight = b.m_scaleBufferHeight;
    return *this;
}

void ImageResample::scaleImage(uint8_t * dest, size_t destWidth, size_t destHeight, const uint8_t * src, size_t srcWidth, size_t srcHeight, const ResampleFilter & filter)
{
    //check if we have data
    if (dest == nullptr || src == nullptr || m_formatType == PixelInfo::BAD_FORMAT ) {
        return;
    }
    //check if we need to re-allocate the scale buffer
    if (m_scaleBufferWidth != destWidth || m_scaleBufferHeight != srcHeight)
    {
        //re-allocate temporary space for initial vertical rescale
        delete [] m_scaleBuffer;
        m_scaleBuffer = new uint8_t[destWidth * srcHeight * PixelInfo::pixelInfo(m_formatType).bytesPerPixel];
        m_scaleBufferWidth = destWidth;
        m_scaleBufferHeight = srcHeight;
    }
    //get weights for horizontal rescale
    const KernelWeights * horizontalWeights = m_horizontalKernel.getWeigths(filter, destWidth, srcWidth);
    const size_t horizontalStride = PixelInfo::pixelInfo(m_formatType).bytesPerPixel;
    //first rescale horizontally
#pragma omp parallel for
    for (size_t y = 0; y < srcHeight; ++y) {
        //calculate new scanlines
        uint8_t * destScanLine = (uint8_t *)(m_scaleBuffer + y * destWidth);
        const uint8_t * srcScanLine = (uint8_t *)(src + y * srcWidth);
        //loop horizontally
        accumulateWeighted(destScanLine, horizontalStride, destWidth, srcScanLine, srcWidth, horizontalWeights, m_formatType);
    }
    //get weights for vertical rescale
    const KernelWeights * verticalWeights = m_verticalKernel.getWeigths(filter, destHeight, srcHeight);
    const size_t verticalStride = PixelInfo::pixelInfo(m_formatType).bytesPerPixel * destWidth;
    //now rescale vertically
#pragma omp parallel for
    for (size_t x = 0; x < destWidth; ++x) {
        //calculate new scanlines
        uint8_t * destPixel = (uint8_t *)(dest + x);
        const uint8_t * srcPixel = (uint8_t *)(m_scaleBuffer + x);
        //loop vertically
        accumulateWeighted(destPixel, verticalStride, destHeight, srcPixel, verticalStride, verticalWeights, m_formatType);
    }
}

inline void accumulateWeighted(uint8_t * dest, size_t destStride, size_t count, const uint8_t * src, size_t srcStride, const KernelWeights * srcWeights, PixelInfo::FormatType m_formatType)
{
    //check what the destination format is
    switch (m_formatType) {
        case PixelInfo::R8G8B8A8:
            accumulatePixel<PixelInfo::R8G8B8A8>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::A8R8G8B8:
            accumulatePixel<PixelInfo::A8R8G8B8>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::R8G8B8X8:
            accumulatePixel<PixelInfo::R8G8B8X8>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::X8R8G8B8:
            accumulatePixel<PixelInfo::X8R8G8B8>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::R8G8B8:
            accumulatePixel<PixelInfo::R8G8B8>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::X1R5G5B5:
            accumulatePixel<PixelInfo::X1R5G5B5>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::R5G6B5:
            accumulatePixel<PixelInfo::R5G6B5>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::I8:
            accumulatePixel<PixelInfo::I8>(dest, destStride, count, src, srcStride, srcWeights); break;
        case PixelInfo::I16:
            accumulatePixel<PixelInfo::I16>(dest, destStride, count, src, srcStride, srcWeights); break;
    }
}
