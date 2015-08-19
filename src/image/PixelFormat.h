#pragma once

#include <stdint.h>
#include <limits>
#include <string>
#include <type_traits>

#include "PixelInfo.h"

//we do this to prevent compiler warning, because the macro "max" is already defined...
#ifdef max
	#undef max
#endif


template <int FORMATTYPE>
struct ColorRange
{
    typename TypeFactory<FORMATTYPE>::PixelType min;
    typename TypeFactory<FORMATTYPE>::PixelType max;
};

#define BIT_MASK(bits) ((1 << (bits)) - 1)

template <int FORMATTYPE>
struct PixelFormat
{
    typedef typename TypeFactory<FORMATTYPE>::PixelType Pixel; //!< The data type one pixel of this format has.
    typedef typename TypeFactory<FORMATTYPE>::ColorType Color; //!< The data type one color component of this format has.
    typedef typename TypeFactory<FORMATTYPE>::TempColorType TempColor; //!< The smallest data type that should be used for color calculations.

    enum { bitsPerPixel = TypeFactory<FORMATTYPE>::bitsPerPixel };
    enum { bytesPerPixel = TypeFactory<FORMATTYPE>::bytesPerPixel };
    enum { bytesPerColor = TypeFactory<FORMATTYPE>::bytesPerColor };
    enum { nrOfComponents = TypeFactory<FORMATTYPE>::nrOfComponents };
    enum { bitsRed = TypeFactory<FORMATTYPE>::bitsRed };
    enum { bitsGreen = TypeFactory<FORMATTYPE>::bitsGreen };
    enum { bitsBlue = TypeFactory<FORMATTYPE>::bitsBlue };
    enum { bitsAlpha = TypeFactory<FORMATTYPE>::bitsAlpha };
    enum { shiftRed = TypeFactory<FORMATTYPE>::shiftRed };
    enum { shiftGreen = TypeFactory<FORMATTYPE>::shiftGreen };
    enum { shiftBlue = TypeFactory<FORMATTYPE>::shiftBlue };
    enum { shiftAlpha = TypeFactory<FORMATTYPE>::shiftAlpha };
    enum { paletteEntries = TypeFactory<FORMATTYPE>::paletteEntries };
    enum { compressed = TypeFactory<FORMATTYPE>::compressed };

    static const inline PixelInfo::FormatType type() { return TypeFactory<FORMATTYPE>::type(); }
    static const inline std::string name() { return TypeFactory<FORMATTYPE>::name(); }

    static inline Pixel getPixel(const uint8_t * src) { return *((const Pixel *)src); }
    static inline Color getR(const Pixel & pixel) { return (bitsRed   > 0 ? ((pixel >> shiftRed)   & BIT_MASK(bitsRed))   : 0); }
    static inline Color getG(const Pixel & pixel) { return (bitsGreen > 0 ? ((pixel >> shiftGreen) & BIT_MASK(bitsGreen)) : 0); }
    static inline Color getB(const Pixel & pixel) { return (bitsBlue  > 0 ? ((pixel >> shiftBlue)  & BIT_MASK(bitsBlue))  : 0); }
    static inline Color getA(const Pixel & pixel) { return (bitsAlpha > 0 ? ((pixel >> shiftAlpha) & BIT_MASK(bitsAlpha)) : BIT_MASK(bitsAlpha)); }

    static inline void setPixel(uint8_t * dest, const Pixel & pixel) { (*((Pixel *)dest)) = pixel; }
    static inline void setR(Pixel & pixel, const Color & r) { pixel = (bitsRed   > 0 ? pixel | ((((Pixel)r) & BIT_MASK(bitsRed))   << shiftRed)   : pixel); }
    static inline void setG(Pixel & pixel, const Color & g) { pixel = (bitsGreen > 0 ? pixel | ((((Pixel)g) & BIT_MASK(bitsGreen)) << shiftGreen) : pixel); }
    static inline void setB(Pixel & pixel, const Color & b) { pixel = (bitsBlue  > 0 ? pixel | ((((Pixel)b) & BIT_MASK(bitsBlue))  << shiftBlue)  : pixel); }
    static inline void setA(Pixel & pixel, const Color & a) { pixel = (bitsAlpha > 0 ? pixel | ((((Pixel)a) & BIT_MASK(bitsAlpha)) << shiftAlpha) : pixel); }

    static inline Color toGrey(const Pixel & pixel) { return (0.2126f * getR(pixel) + 0.7152f * getG(pixel) + 0.0722f * getB(pixel)); }
    static inline void fromGrey(Pixel & pixel, const Color & gray) { setR(pixel, gray); setG(pixel, gray); setB(pixel, gray); }
};

//-------------------------------------------------------------------------------------------------

//We need to specialize here for R8G8B8, because it's a 3-byte format
template <>
static inline void PixelFormat<PixelInfo::FormatType::R8G8B8>::setPixel(uint8_t * dest, const PixelFormat<PixelInfo::FormatType::R8G8B8>::Pixel & pixel)
{
    dest[0] = pixel >> 16;
    dest[1] = pixel >> 8;
    dest[2] = pixel;
}

//-------------------------------------------------------------------------------------------------

//Disable divide by 0 warning. The preprocessor and compiler optimizations should fix this anyway...
#pragma warning( push )
#pragma warning( disable : 4723)

/*!
Scale a color value from the value read from the pixel up to the full range of the native type, e.g. 5 bits to uint8_t.
\param value Value to upscale to full range.
\return Returns value scaled up to the full range of the data type.
*/
template <class TYPE, int BITS>
static inline TYPE scaleUp(const TYPE & value)
{
    return (std::numeric_limits<typename TYPE>::max() * (uint32_t)value) / BIT_MASK(BITS);
}

/*!
Scale a color value from the native type range to the value the color uses, e.g. uint8_t to 5 bits.
\param value Value to downscale to BITS color bit range.
\return Returns value scaled down to the allowed color range.
*/
template <class TYPE, int BITS>
static inline TYPE scaleDown(const TYPE & value)
{
    return (BIT_MASK(BITS) * (uint32_t)value) / std::numeric_limits<TYPE>::max();
}

/*!
Scale a color value from source to destination type bit range while also converting types.
\param value Value to scale from INBITS to OUTBITS color bit range.
\return Returns value scaled from INBITS to OUTBITS color range.
*/
template <class OUTTYPE, int OUTBITS, class INTYPE, int INBITS>
static inline OUTTYPE scaleInOut(const INTYPE & value)
{
    return (BIT_MASK(OUTBITS) * (uint32_t)value) / BIT_MASK(INBITS);
}

#pragma warning( pop )

/*!
Convert between two pixel formats with proper scaling of colors.
\param[in] dest Destination data pointer.
\param[in] src Source data pointer.
*/
template <int OUTTYPE, int INTYPE>
static inline void convertPixel(uint8_t * dest, const uint8_t * src)
{
    PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src);
    PixelFormat<OUTTYPE>::Pixel outPixel;
    if (PixelFormat<INTYPE>::nrOfComponents == 1) {
        PixelFormat<OUTTYPE>::Color r = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsRed, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsRed>(PixelFormat<INTYPE>::getR(inPixel));
        if (PixelFormat<OUTTYPE>::nrOfComponents == 1) {
            PixelFormat<OUTTYPE>::setR(outPixel, r);
        }
        else if (PixelFormat<OUTTYPE>::nrOfComponents == 3) {
            PixelFormat<OUTTYPE>::setR(outPixel, r);
            PixelFormat<OUTTYPE>::setG(outPixel, r);
            PixelFormat<OUTTYPE>::setB(outPixel, r);
        }
        else if (PixelFormat<OUTTYPE>::nrOfComponents == 4) {
            PixelFormat<OUTTYPE>::setR(outPixel, r);
            PixelFormat<OUTTYPE>::setG(outPixel, r);
            PixelFormat<OUTTYPE>::setB(outPixel, r);
            PixelFormat<OUTTYPE>::setA(outPixel, std::numeric_limits<PixelFormat<OUTTYPE>::Color>::max());
        }
    }
    else if (PixelFormat<INTYPE>::nrOfComponents == 3) {
        PixelFormat<OUTTYPE>::Color r = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsRed, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsRed>(PixelFormat<INTYPE>::getR(inPixel));
        PixelFormat<OUTTYPE>::Color g = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsGreen, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsGreen>(PixelFormat<INTYPE>::getG(inPixel));
        PixelFormat<OUTTYPE>::Color b = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsBlue, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsBlue>(PixelFormat<INTYPE>::getB(inPixel));
        if (PixelFormat<OUTTYPE>::nrOfComponents == 1) {
            PixelFormat<OUTTYPE>::setR(outPixel, (PixelFormat<OUTTYPE>::Color)(0.2126f * r + 0.7152f * g + 0.0722f * b));
        }
        else if (PixelFormat<OUTTYPE>::nrOfComponents == 3) {
            PixelFormat<OUTTYPE>::setR(outPixel, r);
            PixelFormat<OUTTYPE>::setG(outPixel, g);
            PixelFormat<OUTTYPE>::setB(outPixel, b);
        }
        else if (PixelFormat<OUTTYPE>::nrOfComponents == 4) {
            PixelFormat<OUTTYPE>::setR(outPixel, r);
            PixelFormat<OUTTYPE>::setG(outPixel, g);
            PixelFormat<OUTTYPE>::setB(outPixel, b);
            PixelFormat<OUTTYPE>::setA(outPixel, std::numeric_limits<PixelFormat<OUTTYPE>::Color>::max());
        }
    }
    else if (PixelFormat<INTYPE>::nrOfComponents == 4) {
        PixelFormat<OUTTYPE>::Color r = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsRed, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsRed>(PixelFormat<INTYPE>::getR(inPixel));
        PixelFormat<OUTTYPE>::Color g = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsGreen, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsGreen>(PixelFormat<INTYPE>::getG(inPixel));
        PixelFormat<OUTTYPE>::Color b = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsBlue, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsBlue>(PixelFormat<INTYPE>::getB(inPixel));
        if (PixelFormat<OUTTYPE>::nrOfComponents == 1) {
            PixelFormat<OUTTYPE>::setR(outPixel, (PixelFormat<OUTTYPE>::Color)(0.2126f * r + 0.7152f * g + 0.0722f * b));
        }
        else if (PixelFormat<OUTTYPE>::nrOfComponents == 3) {
            PixelFormat<OUTTYPE>::setR(outPixel, r);
            PixelFormat<OUTTYPE>::setG(outPixel, g);
            PixelFormat<OUTTYPE>::setB(outPixel, b);
        }
        else if (PixelFormat<OUTTYPE>::nrOfComponents == 4) {
            PixelFormat<OUTTYPE>::Color a = scaleInOut<PixelFormat<OUTTYPE>::Color, PixelFormat<OUTTYPE>::bitsAlpha, PixelFormat<INTYPE>::Color, PixelFormat<INTYPE>::bitsAlpha>(PixelFormat<INTYPE>::getA(inPixel));
            PixelFormat<OUTTYPE>::setR(outPixel, r);
            PixelFormat<OUTTYPE>::setG(outPixel, g);
            PixelFormat<OUTTYPE>::setB(outPixel, b);
            PixelFormat<OUTTYPE>::setA(outPixel, a);
        }
    }
    PixelFormat<OUTTYPE>::setPixel(dest, outPixel);
}

/*!
Convert between two pixel formats with proper scaling of colors.
\param[in] dest Destination data pointer.
\param[in] src Source data pointer.
\param[in] count Number of pixels to convert.
*/
template <int OUTTYPE, int INTYPE>
static inline void convertPixel(uint8_t * dest, const uint8_t * src, size_t count)
{
    //check if we have data
    if (dest == nullptr || src == nullptr || count == 0) {
        return;
    }
#pragma omp parallel for
    for (int i = 0; i < count; ++i) {
        convertPixel<OUTTYPE, INTYPE>(dest + i * PixelFormat<OUTTYPE>::bytesPerPixel, src + i * PixelFormat<INTYPE>::bytesPerPixel);
    }
}

/*!
Convert colors from one pixel format to another.
\param[in] dest Output color destination pointer.
\param[in] destPalette Output color palette pointer or nullptr.
\param[in] destFormat Output color pixel format.
\param[in] source Input color source pointer.
\param[in] sourcePalette Input color palette pointer or nullptr.
\param[in] sourceFormat Input color pixel format.
\param[in] count Number of consecutive pixels to convert.
*/
void convertFormat(uint8_t * dest, uint8_t * destPalette, PixelInfo::FormatType destType, const uint8_t * source, const uint8_t * sourcePalette, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_R8G8B8A8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_A8R8G8B8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_R8G8B8X8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_X8R8G8B8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_R4G4B4A4(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_R8G8B8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_X1R5G5B5(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_R5G6B5(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_I8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);
void convertToFormat_I16(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count);

//-------------------------------------------------------------------------------------------------

/*!
Calculate range of colors in data. Returns the min/max value of all color components wrapped up as a pixel.
\param[in] src Source data pointer.
\param[in] count Number of pixels to check.
*/
template <int INTYPE>
static inline typename ColorRange<INTYPE> calculateColorRange(const uint8_t * src, size_t count)
{
    ColorRange<INTYPE> range;
    range.min = 0;
    range.max = 0;
    //check if we have data
    if (src == nullptr || count == 0) {
        return range;
    }
    //check hoiw many color components we have
    if (PixelFormat<INTYPE>::nrOfComponents == 1) {
        //set up overall min/max
        PixelFormat<INTYPE>::Color rmin; PixelFormat<INTYPE>::Color rmax;
        rmin = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        rmax = 0;
        //and min/max per thread
        PixelFormat<INTYPE>::Color rtmin; PixelFormat<INTYPE>::Color rtmax;
#pragma omp parallel private (rtmin, rtmax)
        {
            //set up per-thread values for min/max
            rtmin = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            rtmax = 0;
#pragma omp for
            for (int i = 0; i < count; ++i) {
                //get pixel and color from pixel
                PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src + i * PixelFormat<INTYPE>::bytesPerPixel);
                PixelFormat<INTYPE>::Color r = PixelFormat<INTYPE>::getR(inPixel);
                //check if smaller/larger than current min/max
                rtmin = r < rtmin ? r : rtmin; rtmax = r > rtmax ? r : rtmax;
            }
#pragma omp critical
            rmin = rtmin < rmin ? rtmin : rmin; rmax = rtmax > rmax ? rtmax : rmax;
        }
        //store min/max in range
        PixelFormat<INTYPE>::setR(range.min, rmin); PixelFormat<INTYPE>::setR(range.max, rmax);
    }
    else if (PixelFormat<INTYPE>::nrOfComponents == 3) {
        //set up overall min/max
        PixelFormat<INTYPE>::Color min[3]; PixelFormat<INTYPE>::Color max[3];
        min[0] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        min[1] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        min[2] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        max[0] = 0;
        max[1] = 0;
        max[2] = 0;
        //and min/max per thread
        PixelFormat<INTYPE>::Color tmin[3]; PixelFormat<INTYPE>::Color tmax[3];
#pragma omp parallel private (tmin, tmax)
        {
            //set up per-thread values for min/max
            tmin[0] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmin[1] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmin[2] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmax[0] = 0;
            tmax[1] = 0;
            tmax[2] = 0;
#pragma omp for
            for (int i = 0; i < count; ++i) {
                //get pixel and color from pixel
                PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src + i * PixelFormat<INTYPE>::bytesPerPixel);
                PixelFormat<INTYPE>::Color color[3];
                color[0] = PixelFormat<INTYPE>::getR(inPixel);
                color[1] = PixelFormat<INTYPE>::getG(inPixel);
                color[2] = PixelFormat<INTYPE>::getB(inPixel);
                //check if smaller/larger than current min/max
                tmin[0] = color[0] < tmin[0] ? color[0] : tmin[0]; tmax[0] = color[0] > tmax[0] ? color[0] : tmax[0];
                tmin[1] = color[1] < tmin[1] ? color[1] : tmin[1]; tmax[1] = color[1] > tmax[1] ? color[1] : tmax[1];
                tmin[2] = color[2] < tmin[2] ? color[2] : tmin[2]; tmax[2] = color[2] > tmax[2] ? color[2] : tmax[2];
            }
#pragma omp critical
            min[0] = tmin[0] < min[0] ? tmin[0] : min[0]; max[0] = tmax[0] > max[0] ? tmax[0] : max[0];
            min[1] = tmin[1] < min[1] ? tmin[1] : min[1]; max[1] = tmax[1] > max[1] ? tmax[1] : max[1];
            min[2] = tmin[2] < min[2] ? tmin[2] : min[2]; max[2] = tmax[2] > max[2] ? tmax[2] : max[2];
        }
        //store min/max in range
        PixelFormat<INTYPE>::setR(range.min, min[0]); PixelFormat<INTYPE>::setR(range.max, max[0]);
        PixelFormat<INTYPE>::setG(range.min, min[1]); PixelFormat<INTYPE>::setG(range.max, max[1]);
        PixelFormat<INTYPE>::setB(range.min, min[2]); PixelFormat<INTYPE>::setB(range.max, max[2]);
    }
    else if (PixelFormat<INTYPE>::nrOfComponents == 4) {
        //set up overall min/max
        PixelFormat<INTYPE>::Color min[4]; PixelFormat<INTYPE>::Color max[4];
        min[0] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        min[1] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        min[2] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        min[3] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
        max[0] = 0;
        max[1] = 0;
        max[2] = 0;
        max[3] = 0;
        //and min/max per thread
        PixelFormat<INTYPE>::Color tmin[4]; PixelFormat<INTYPE>::Color tmax[4];
#pragma omp parallel private (tmin, tmax)
        {
            //set up per-thread values for min/max
            tmin[0] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmin[1] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmin[2] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmin[3] = std::numeric_limits<PixelFormat<INTYPE>::Color>::max();
            tmax[0] = 0;
            tmax[1] = 0;
            tmax[2] = 0;
            tmax[3] = 0;
#pragma omp for
            for (int i = 0; i < count; ++i) {
                //get pixel and color from pixel
                PixelFormat<INTYPE>::Pixel inPixel = PixelFormat<INTYPE>::getPixel(src + i * PixelFormat<INTYPE>::bytesPerPixel);
                PixelFormat<INTYPE>::Color color[4];
                color[0] = PixelFormat<INTYPE>::getR(inPixel);
                color[1] = PixelFormat<INTYPE>::getG(inPixel);
                color[2] = PixelFormat<INTYPE>::getB(inPixel);
                color[3] = PixelFormat<INTYPE>::getA(inPixel);
                //check if smaller/larger than current min/max
                tmin[0] = color[0] < tmin[0] ? color[0] : tmin[0]; tmax[0] = color[0] > tmax[0] ? color[0] : tmax[0];
                tmin[1] = color[1] < tmin[1] ? color[1] : tmin[1]; tmax[1] = color[1] > tmax[1] ? color[1] : tmax[1];
                tmin[2] = color[2] < tmin[2] ? color[2] : tmin[2]; tmax[2] = color[2] > tmax[2] ? color[2] : tmax[2];
                tmin[3] = color[3] < tmin[3] ? color[3] : tmin[3]; tmax[3] = color[3] > tmax[3] ? color[3] : tmax[3];
            }
#pragma omp critical
            min[0] = tmin[0] < min[0] ? tmin[0] : min[0]; max[0] = tmax[0] > max[0] ? tmax[0] : max[0];
            min[1] = tmin[1] < min[1] ? tmin[1] : min[1]; max[1] = tmax[1] > max[1] ? tmax[1] : max[1];
            min[2] = tmin[2] < min[2] ? tmin[2] : min[2]; max[2] = tmax[2] > max[2] ? tmax[2] : max[2];
            min[3] = tmin[3] < min[3] ? tmin[3] : min[3]; max[3] = tmax[3] > max[3] ? tmax[3] : max[3];
        }
        //store min/max in range
        PixelFormat<INTYPE>::setR(range.min, min[0]); PixelFormat<INTYPE>::setR(range.max, max[0]);
        PixelFormat<INTYPE>::setG(range.min, min[1]); PixelFormat<INTYPE>::setG(range.max, max[1]);
        PixelFormat<INTYPE>::setB(range.min, min[2]); PixelFormat<INTYPE>::setB(range.max, max[2]);
        PixelFormat<INTYPE>::setA(range.min, min[3]); PixelFormat<INTYPE>::setA(range.max, max[3]);
    }
    return range;
}
