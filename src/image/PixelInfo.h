#pragma once

#include <stdint.h>
#include <limits>
#include <string>
#include <type_traits>


struct PixelInfo
{
    enum FormatType { BAD_FORMAT, R8G8B8A8, A8R8G8B8, R8G8B8X8, X8R8G8B8, R4G4B4A4, R8G8B8, X1R5G5B5, R5G6B5, /*true color RGB, RGBA*/
        I8, I16, /*paletted types*/
        PVRTC1_R4G4B4, PVRTC1_R2G2B2, PVRTC1_R4G4B4A4, PVRTC1_R2G2B2A2, /*compressed RGB, RGBA formats*/
        MAX_FORMAT }; //!<The truecolor pixel formats we support.

    const FormatType type; //!< Type identifier of pixel format.
    const uint32_t bitsPerPixel; //!< Bits per pixel. This corresponds to the pixel data type, e.g. uint32_t = 32.
    const uint32_t bytesPerPixel; //!< Bytes per pixel. This corresponds to the pixel data type, e.g. uint32_t = 4.
    const uint32_t bytesPerColor; //!< Bytes per color component. This corresponds to the color data type, e.g. uint8_t = 1.
    const uint32_t nrOfComponents; //!< The number of color components this format has, e.g. 3 for RGB.
    const uint32_t bitsRed; //!< Bits in red color component.
    const uint32_t bitsGreen; //!< Bits in green color component.
    const uint32_t bitsBlue; //!< Bits in blue color component.
    const uint32_t bitsAlpha; //!< Bits in alpha component.
    const uint32_t shiftRed; //!< Bit shift of red color component in pixel data.
    const uint32_t shiftGreen; //!< Bit shift of green color component in pixel data.
    const uint32_t shiftBlue; //!< Bit shift of blue color component in pixel data.
    const uint32_t shiftAlpha; //!< Bit shift of alpha component in pixel data.
    const uint32_t paletteEntries; //!< Number of palette entries this format has.
    const bool compressed; //!< True if the image is in compressed format.
    const std::string name; //!< Name of pixel format.

    /*!
    Retrieve pixel format information at runtime.
    \param type Pixel format type to get information about.
    \return Returns a reference to a PixelInfo pixel format information structure.
    */
    static inline const PixelInfo & pixelInfo(const FormatType & type)
    {
        if (type <= BAD_FORMAT || type >= MAX_FORMAT) {
            return pixelInfos[BAD_FORMAT];
        }
        return pixelInfos[type];
    }

private:
    static const PixelInfo pixelInfos[]; //!< List of pixel format information structures.
}; //!< Pixel format information. This is for RUNTIME information. The PixelFormat<> template class is for STATIC information.

//-------------------------------------------------------------------------------------------

//Here we create typedefs for the pixel and color type and color/bit layout of every pixel format.
//This seems to be a duplicate, but is needed for static type information in the PixelFormat<> template class...
template <int PIXELTYPE>
struct TypeFactory
{
};

template <>
struct TypeFactory<PixelInfo::FormatType::R8G8B8A8>
{
    typedef uint32_t PixelType; //!< The data type one pixel of this format has.
    typedef uint8_t ColorType; //!< The data type one color component of this format has.
    typedef uint32_t TempColorType; //!< The "safe" data type one intermediate color component of this format has. Use this for interpolation.

    enum { bitsPerPixel = 32 };
    enum { bytesPerPixel = 4 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 4 };
    enum { bitsRed = 8 };
    enum { bitsGreen = 8 };
    enum { bitsBlue = 8 };
    enum { bitsAlpha = 8 };
    enum { shiftRed = 24 };
    enum { shiftGreen = 16 };
    enum { shiftBlue = 8 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::R8G8B8A8; }
    static const inline std::string name() { return "R8G8B8A8"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::A8R8G8B8>
{
    typedef uint32_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 32 };
    enum { bytesPerPixel = 4 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 4 };
    enum { bitsRed = 8 };
    enum { bitsGreen = 8 };
    enum { bitsBlue = 8 };
    enum { bitsAlpha = 8 };
    enum { shiftRed = 16 };
    enum { shiftGreen = 8 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 24 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::A8R8G8B8; }
    static const inline std::string name() { return "A8R8G8B8"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::R8G8B8X8>
{
    typedef uint32_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 32 };
    enum { bytesPerPixel = 4 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 3 };
    enum { bitsRed = 8 };
    enum { bitsGreen = 8 };
    enum { bitsBlue = 8 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 24 };
    enum { shiftGreen = 16 };
    enum { shiftBlue = 8 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::R8G8B8X8; }
    static const inline std::string name() { return "R8G8B8X8"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::X8R8G8B8>
{
    typedef uint32_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 32 };
    enum { bytesPerPixel = 4 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 3 };
    enum { bitsRed = 8 };
    enum { bitsGreen = 8 };
    enum { bitsBlue = 8 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 16 };
    enum { shiftGreen = 8 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 24 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::X8R8G8B8; }
    static const inline std::string name() { return "X8R8G8B8"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::R4G4B4A4>
{
    typedef uint32_t PixelType; //!< The data type one pixel of this format has.
    typedef uint8_t ColorType; //!< The data type one color component of this format has.
    typedef uint32_t TempColorType; //!< The "safe" data type one intermediate color component of this format has. Use this for interpolation.

    enum { bitsPerPixel = 16 };
    enum { bytesPerPixel = 2 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 4 };
    enum { bitsRed = 4 };
    enum { bitsGreen = 4 };
    enum { bitsBlue = 4 };
    enum { bitsAlpha = 4 };
    enum { shiftRed = 12 };
    enum { shiftGreen = 8 };
    enum { shiftBlue = 4 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::R4G4B4A4; }
    static const inline std::string name() { return "R4G4B4A4"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::R8G8B8>
{
    typedef uint32_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 24 };
    enum { bytesPerPixel = 3 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 3 };
    enum { bitsRed = 8 };
    enum { bitsGreen = 8 };
    enum { bitsBlue = 8 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 16 };
    enum { shiftGreen = 8 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::R8G8B8; }
    static const inline std::string name() { return "R8G8B8"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::X1R5G5B5>
{
    typedef uint16_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 16 };
    enum { bytesPerPixel = 2 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 4 };
    enum { bitsRed = 5 };
    enum { bitsGreen = 5 };
    enum { bitsBlue = 5 };
    enum { bitsAlpha = 1 };
    enum { shiftRed = 10 };
    enum { shiftGreen = 5 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 15 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::X1R5G5B5; }
    static const inline std::string name() { return "X1R5G5B5"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::R5G6B5>
{
    typedef uint16_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 16 };
    enum { bytesPerPixel = 2 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 3 };
    enum { bitsRed = 5 };
    enum { bitsGreen = 6 };
    enum { bitsBlue = 5 };
    enum { bitsAlpha = 1 };
    enum { shiftRed = 11 };
    enum { shiftGreen = 5 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::R5G6B5; }
    static const inline std::string name() { return "R5G6B5"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::I8>
{
    typedef uint8_t PixelType;
    typedef uint8_t ColorType;
    typedef uint32_t TempColorType;

    enum { bitsPerPixel = 8 };
    enum { bytesPerPixel = 1 };
    enum { bytesPerColor = 1 };
    enum { nrOfComponents = 1 };
    enum { bitsRed = 8 };
    enum { bitsGreen = 0 };
    enum { bitsBlue = 0 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 0 };
    enum { shiftGreen = 0 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 256 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::I8; }
    static const inline std::string name() { return "I8"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::I16>
{
    typedef uint16_t PixelType;
    typedef uint16_t ColorType;
    typedef uint64_t TempColorType;

    enum { bitsPerPixel = 16 };
    enum { bytesPerPixel = 2 };
    enum { bytesPerColor = 2 };
    enum { nrOfComponents = 1 };
    enum { bitsRed = 16 };
    enum { bitsGreen = 0 };
    enum { bitsBlue = 0 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 0 };
    enum { shiftGreen = 0 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 65536 };
    enum { compressed = 0 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::I16; }
    static const inline std::string name() { return "I16"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::PVRTC1_R4G4B4>
{
    typedef uint8_t PixelType;
    typedef uint8_t ColorType;
    typedef uint8_t TempColorType;

    enum { bitsPerPixel = 0 };
    enum { bytesPerPixel = 0 };
    enum { bytesPerColor = 0 };
    enum { nrOfComponents = 3 };
    enum { bitsRed = 4 };
    enum { bitsGreen = 4 };
    enum { bitsBlue = 4 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 8 };
    enum { shiftGreen = 4 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 1 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::PVRTC1_R4G4B4; }
    static const inline std::string name() { return "PVRTC1_R4G4B4"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::PVRTC1_R2G2B2>
{
    typedef uint8_t PixelType;
    typedef uint8_t ColorType;
    typedef uint8_t TempColorType;

    enum { bitsPerPixel = 0 };
    enum { bytesPerPixel = 0 };
    enum { bytesPerColor = 0 };
    enum { nrOfComponents = 3 };
    enum { bitsRed = 2 };
    enum { bitsGreen = 2 };
    enum { bitsBlue = 2 };
    enum { bitsAlpha = 0 };
    enum { shiftRed = 4 };
    enum { shiftGreen = 2 };
    enum { shiftBlue = 0 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 1 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::PVRTC1_R2G2B2; }
    static const inline std::string name() { return "PVRTC1_R2G2B2"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::PVRTC1_R4G4B4A4>
{
    typedef uint8_t PixelType;
    typedef uint8_t ColorType;
    typedef uint8_t TempColorType;

    enum { bitsPerPixel = 0 };
    enum { bytesPerPixel = 0 };
    enum { bytesPerColor = 0 };
    enum { nrOfComponents = 4 };
    enum { bitsRed = 4 };
    enum { bitsGreen = 4 };
    enum { bitsBlue = 4 };
    enum { bitsAlpha = 4 };
    enum { shiftRed = 12 };
    enum { shiftGreen = 8 };
    enum { shiftBlue = 4 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 1 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::PVRTC1_R4G4B4A4; }
    static const inline std::string name() { return "PVRTC1_R4G4B4A4"; }
};

template <>
struct TypeFactory<PixelInfo::FormatType::PVRTC1_R2G2B2A2>
{
    typedef uint8_t PixelType;
    typedef uint8_t ColorType;
    typedef uint8_t TempColorType;

    enum { bitsPerPixel = 0 };
    enum { bytesPerPixel = 0 };
    enum { bytesPerColor = 0 };
    enum { nrOfComponents = 4 };
    enum { bitsRed = 2 };
    enum { bitsGreen = 2 };
    enum { bitsBlue = 2 };
    enum { bitsAlpha = 2 };
    enum { shiftRed = 6 };
    enum { shiftGreen = 4 };
    enum { shiftBlue = 2 };
    enum { shiftAlpha = 0 };
    enum { paletteEntries = 0 };
    enum { compressed = 1 };

    static const inline PixelInfo::FormatType type() { return PixelInfo::FormatType::PVRTC1_R2G2B2A2; }
    static const inline std::string name() { return "PVRTC1_R2G2B2A2"; }
};
