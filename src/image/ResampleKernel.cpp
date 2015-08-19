#include "ResampleKernel.h"


ResampleKernel::ResampleKernel()
    : m_weightStorage(nullptr)
    , m_weights(nullptr)
{
}

ResampleKernel::~ResampleKernel()
{
    delete [] m_weights;
    delete [] m_weightStorage;
}

const KernelWeights * ResampleKernel::getWeigths(ResampleFilter kernel, size_t destSize, size_t srcSize)
{
    //check if we need to re-calculate
    if (m_weights == nullptr || m_kernel != kernel || m_destSize != destSize || m_srcSize != srcSize)
    {
        //yes. store new values
        m_kernel = kernel;
        m_destSize = destSize;
        m_srcSize = srcSize;
        //setup some helpers
        const float scale = (float)srcSize / (float)destSize;
        const float pixelScale = scale > 1.0f ? scale : 1.0f;
        const float sampleSize = ceil(pixelScale * m_kernel.support);
        //now allocate weights and weights storage arrays
        m_weightStorage = new float[destSize * ((size_t)sampleSize + 2) * 2];
        size_t weightStorageIndex = 0;
        //we need one set of weights for every pixel in destination image
        m_weights = new KernelWeights[destSize];
        //now calculate weights
        for (size_t x = 0; x < destSize; ++x) {
            //calculate center of destination pixel in source image
            const float sourceX = ((float)x + 0.5f) * scale - 0.5f;
            //calculate start and end of current sampling range
            const size_t startX = ceil(sourceX - sampleSize) > 0.0 ? (size_t)ceil(sourceX - sampleSize) : 0;
            const size_t endX = floor(sourceX + sampleSize) <= (srcSize - 1) ? (size_t)floor(sourceX + sampleSize) : (srcSize - 1);
            //put into structure
            m_weights[x].start = startX;
            m_weights[x].end = endX;
            //store to start of weights in global weight storage array
            m_weights[x].weights = &m_weightStorage[weightStorageIndex];
            //calculate individual weights and sum
            float weightSum = 0.0f;
            for (size_t weightX = startX; weightX <= endX; ++weightX) {
                const float w = m_kernel.function(((float)weightX - sourceX) / pixelScale);
                //store weigth and update sum
                m_weightStorage[weightStorageIndex++] = w;
                weightSum += w;
            }
            //store inverse sum
            m_weights[x].invSum = 1.0f / weightSum;
        }
    }
    //else values are already calculated and cached.
    return (const KernelWeights *)m_weights;
}
