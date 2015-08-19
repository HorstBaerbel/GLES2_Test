#include "PixelFormat.h"

const PixelInfo PixelInfo::pixelInfos[] = {
    { FormatType::BAD_FORMAT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, false, "bad format" },
    { FormatType::R8G8B8A8, 32, 4, 1, 4, 8, 8, 8, 8, 24, 16,  8,  0,     0, false, "R8G8B8A8" },
    { FormatType::A8R8G8B8, 32, 4, 1, 4, 8, 8, 8, 8, 16,  8,  0, 24,     0, false, "A8R8G8B8" },
    { FormatType::R8G8B8X8, 32, 4, 1, 3, 8, 8, 8, 0, 24, 16,  0,  0,     0, false, "R8G8B8X8" },
    { FormatType::X8R8G8B8, 32, 4, 1, 3, 8, 8, 8, 0, 16,  8,  0, 24,     0, false, "X8R8G8B8" },
    { FormatType::R4G4B4A4, 16, 2, 1, 4, 4, 4, 4, 4, 12,  8,  4,  0,     0, false, "R4G4B4A4" },
    { FormatType::R8G8B8,   24, 3, 1, 3, 8, 8, 8, 0, 16,  8,  0,  0,     0, false, "R8G8B8" },
    { FormatType::X1R5G5B5, 16, 2, 1, 4, 5, 5, 5, 1, 10,  5,  0, 15,     0, false, "X1R5G5B5" },
    { FormatType::R5G6B5,   16, 2, 1, 3, 5, 6, 5, 0, 11,  5,  0,  0,     0, false, "R5G6B5" },
    { FormatType::I8,        8, 1, 1, 1, 8, 8, 8, 8,  0,  0,  0,  0,   256, false, "I8" },
    { FormatType::I16,      16, 2, 2, 1, 8, 8, 8, 8,  0,  0,  0,  0, 65536, false, "I16" },
    { FormatType::PVRTC1_R4G4B4,   0, 0, 0, 3, 4, 4, 4, 0,  8, 4, 0, 0, 0, true, "PVR1_R4G4B4" },
    { FormatType::PVRTC1_R2G2B2,   0, 0, 0, 3, 2, 2, 2, 0,  4, 2, 0, 0, 0, true, "PVR1_R2G2B2" },
    { FormatType::PVRTC1_R4G4B4A4, 0, 0, 0, 4, 4, 4, 4, 4, 12, 8, 4, 0, 0, true, "PVR1_R4G4B4A4" },
    { FormatType::PVRTC1_R2G2B2A2, 0, 0, 0, 4, 2, 2, 2, 2,  6, 4, 2, 0, 0, true, "PVR1_R2G2B2A2" }
};

void convertFormat(uint8_t * dest, uint8_t * destPalette, PixelInfo::FormatType destType, const uint8_t * source, const uint8_t * sourcePalette, PixelInfo::FormatType sourceType, size_t count)
{
    //check if we have data
    if (dest == nullptr || source == nullptr || count == 0 || destType == PixelInfo::BAD_FORMAT || sourceType == PixelInfo::BAD_FORMAT ) {
        return;
    }
    const size_t destSize = count * PixelInfo::pixelInfo(destType).bytesPerPixel;
    //if source is destination format, just copy
    if (sourceType == destType) {
        memcpy(dest, source, destSize);
        return;
    }
    //else check what the destination format is
    switch (destType) {
        case PixelInfo::R8G8B8A8:
            convertToFormat_R8G8B8A8(dest, source, sourceType, count); break;
        case PixelInfo::A8R8G8B8:
            convertToFormat_A8R8G8B8(dest, source, sourceType, count); break;
        case PixelInfo::R8G8B8X8:
            convertToFormat_R8G8B8X8(dest, source, sourceType, count); break;
        case PixelInfo::X8R8G8B8:
            convertToFormat_X8R8G8B8(dest, source, sourceType, count); break;
        case PixelInfo::R4G4B4A4:
            convertToFormat_R4G4B4A4(dest, source, sourceType, count); break;
        case PixelInfo::R8G8B8:
            convertToFormat_R8G8B8(dest, source, sourceType, count); break;
        case PixelInfo::X1R5G5B5:
            convertToFormat_X1R5G5B5(dest, source, sourceType, count); break;
        case PixelInfo::R5G6B5:
            convertToFormat_R5G6B5(dest, source, sourceType, count); break;
        case PixelInfo::I8:
            convertToFormat_I8(dest, source, sourceType, count); break;
        case PixelInfo::I16:
            convertToFormat_I16(dest, source, sourceType, count); break;
        default:
            //TODO: Error handling.
            break;
    }
}

void convertToFormat_R8G8B8A8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::R8G8B8A8, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_A8R8G8B8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::A8R8G8B8, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_R8G8B8X8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::R8G8B8X8, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_X8R8G8B8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::X8R8G8B8, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_R4G4B4A4(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::R4G4B4A4, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_R8G8B8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::R8G8B8, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_X1R5G5B5(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::X1R5G5B5, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_R5G6B5(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::I8>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::R5G6B5, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_I8(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::I8, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::I8, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::I8, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::I8, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::I8, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::I8, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::I8, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::I8, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I16:
            convertPixel<PixelInfo::I8, PixelInfo::I16>(dest, source, count); break;
    }
}

void convertToFormat_I16(uint8_t * dest, const uint8_t * source, PixelInfo::FormatType sourceType, size_t count)
{
    switch (sourceType) {
        case PixelInfo::R8G8B8A8:
            convertPixel<PixelInfo::I16, PixelInfo::R8G8B8A8>(dest, source, count); break;
        case PixelInfo::A8R8G8B8:
            convertPixel<PixelInfo::I16, PixelInfo::A8R8G8B8>(dest, source, count); break;
        case PixelInfo::R8G8B8X8:
            convertPixel<PixelInfo::I16, PixelInfo::R8G8B8X8>(dest, source, count); break;
        case PixelInfo::X8R8G8B8:
            convertPixel<PixelInfo::I16, PixelInfo::X8R8G8B8>(dest, source, count); break;
        case PixelInfo::R4G4B4A4:
            convertPixel<PixelInfo::I16, PixelInfo::R4G4B4A4>(dest, source, count); break;
        case PixelInfo::R8G8B8:
            convertPixel<PixelInfo::I16, PixelInfo::R8G8B8>(dest, source, count); break;
        case PixelInfo::X1R5G5B5:
            convertPixel<PixelInfo::I16, PixelInfo::X1R5G5B5>(dest, source, count); break;
        case PixelInfo::R5G6B5:
            convertPixel<PixelInfo::I16, PixelInfo::R5G6B5>(dest, source, count); break;
        case PixelInfo::I8:
            convertPixel<PixelInfo::I16, PixelInfo::I8>(dest, source, count); break;
    }
}
