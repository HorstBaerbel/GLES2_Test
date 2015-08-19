#include "Image.h"

#include <stdio.h>
#include <iostream>
#include <FreeImage.h>


Image::Image(size_t width, size_t height, PixelInfo::FormatType formatType, uint8_t * source, uint8_t * palette, bool takeOwnership)
    : m_width(width)
    , m_height(height)
    , m_formatType(formatType)
    , m_data(nullptr)
    , m_palette(nullptr)
    , m_resampler(formatType)
{
    if (source == nullptr) {
        //if we have a width given, allocate memory
        if (width > 0 && height > 0 && formatType != PixelInfo::BAD_FORMAT) {
            m_data = new uint8_t[width * height * PixelInfo::pixelInfo(m_formatType).bytesPerPixel];
            if (PixelInfo::pixelInfo(m_formatType).paletteEntries > 0) {
                m_palette = new uint8_t[PixelInfo::pixelInfo(m_formatType).paletteEntries * 4];
            }
        }
    }
    else {
        if (takeOwnership) {
            m_data = source;
            m_palette = palette;
        }
        else if (width > 0 && height > 0 && formatType != PixelInfo::BAD_FORMAT) {
            m_data = new uint8_t[width * height * PixelInfo::pixelInfo(m_formatType).bytesPerPixel];
            memcpy(m_data, source, width * height * PixelInfo::pixelInfo(m_formatType).bytesPerPixel); 
            if (PixelInfo::pixelInfo(m_formatType).paletteEntries > 0) {
                m_palette = new uint8_t[PixelInfo::pixelInfo(m_formatType).paletteEntries * 4];
                memcpy(m_palette, palette, PixelInfo::pixelInfo(m_formatType).paletteEntries * 4);
            }
        }
    }
}

Image::Image(size_t width, size_t height, PixelInfo::FormatType formatType, const uint8_t * source, uint8_t * palette, PixelInfo::FormatType sourceType)
    : m_width(width)
    , m_height(height)
    , m_formatType(formatType)
    , m_data(nullptr)
    , m_palette(nullptr)
    , m_resampler(formatType)
{   
    if (formatType == PixelInfo::BAD_FORMAT) {
        throw ImageException("Image::Image() - Invalid image format!");
    }
    else if (sourceType == PixelInfo::BAD_FORMAT) {
        throw ImageException("Image::Image() - Invalid source image format!");
    }
    if (width > 0 && height > 0 && formatType != PixelInfo::BAD_FORMAT) {
        m_data = new uint8_t[width * height * PixelInfo::pixelInfo(m_formatType).bytesPerPixel];
        if (PixelInfo::pixelInfo(m_formatType).paletteEntries > 0) {
            m_palette = new uint8_t[PixelInfo::pixelInfo(m_formatType).paletteEntries * 4];
        }
        if (source != nullptr && sourceType != PixelInfo::BAD_FORMAT) {
            convertFormat(m_data, m_palette, m_formatType, source, palette, sourceType, width * height);
        }
    }
}

Image::Image(const Image & source)
    : m_width(0)
    , m_height(0)
    , m_formatType(PixelInfo::BAD_FORMAT)
    , m_data(nullptr)
    , m_palette(nullptr)
    , m_resampler(PixelInfo::BAD_FORMAT)
{
    *this = source;
}

void Image::copyToInternal(size_t width, size_t height, const uint8_t * source, const uint8_t * palette, PixelInfo::FormatType sourceType)
{
    if (width <= 0 || height <= 0) {
        throw ImageException("Image::copyToInternal() - Invalid image dimensions!");
    }
    else if (sourceType == PixelInfo::BAD_FORMAT) {
        throw ImageException("Image::copyToInternal() - Invalid image format!");
    }
    else if (source == nullptr) {
        throw ImageException("Image::copyToInternal() - Invalid image data!");
    }
    if (m_width != width || m_height != height) {
        m_width = width;
        m_height = height;
        if (m_data != nullptr) {
            delete [] m_data;
            m_data = nullptr;
        }
        if (m_palette != nullptr) {
            delete [] m_palette;
            m_palette = nullptr;
        }
        //if our internal image format is bad we can use the source format as it doesn't matter
        if (m_formatType == PixelInfo::BAD_FORMAT) {
            m_formatType = sourceType;
        }
        if (width > 0 && height > 0 && m_formatType != PixelInfo::BAD_FORMAT) {
            m_data = new uint8_t[m_width * m_height * PixelInfo::pixelInfo(m_formatType).bytesPerPixel];
            if (PixelInfo::pixelInfo(m_formatType).paletteEntries > 0) {
                m_palette = new uint8_t[PixelInfo::pixelInfo(m_formatType).paletteEntries * 4];
            }
        }
    }
    convertFormat(m_data, m_palette, m_formatType, source, palette, sourceType, width * height);
}

Image & Image::operator=(const Image & source)
{
    copyToInternal(source.width(), source.height(), source.pixels(), source.palette(), source.formatType());
    return *this;
}

Image::~Image()
{
    delete [] m_data;
}


Image Image::scaled(size_t width, size_t height, Image::AspectRatioMode aspectMode, const ResampleFilter & filter) const
{
    if (width <= 0 || height <= 0) {
        throw ImageException("Image::scaled() - Invalid image dimensions!");
    }
    //calculate scale ratios
    const float scaleW = (float)m_width / (float)width;
    const float scaleH = (float)m_height / (float)height;
    //if we need to keep aspect ratio, check how
    if (fabs(scaleW - scaleH) > 0.01)
    {
        if (aspectMode == KEEP_WIDTH)
        {
            //keep width, adjust height accordingly
            height = (size_t)((float)m_height / scaleW);
        }
        else if (aspectMode == KEEP_HEIGHT)
        {
            //keep height, adjust width accordingly
            width = (size_t)((float)m_width / scaleH);
        }
    }
    //create empty destination image
    Image destImage(width, height, m_formatType);
    //scale image and return it
    m_resampler.scaleImage(destImage.pixels(), width, height, m_data, m_width, m_height, filter);
    return destImage;
}

void Image::scaleTo(Image & destImage, const ResampleFilter & filter) const
{
    if (destImage.width() <= 0 || destImage.height() <= 0) {
        throw ImageException("Image::scaleTo() - Invalid image dimensions!");
    }
    else if (destImage.formatType() != m_formatType) {
        throw ImageException("Image::scaleTo() - Image format types must match!");
    }
    //scale image using destImage data pointer
    m_resampler.scaleImage(destImage.pixels(), destImage.width(), destImage.height(), m_data, m_width, m_height, filter);
}

void Image::flipVertical()
{
    //buffer for swapping
    uint8_t * lineBuffer = new uint8_t[m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel];
    //swap lines
    for (size_t i = 0; i < m_height / 2; ++i)
    {
        //index runs top to half, other one bottom to top
        size_t j = (m_height - 1) - i;
        //buffer top line
        memcpy(lineBuffer, &m_data[m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel * i], m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel);
        //swap lines, bottom to top first (we've buffered top)
        memcpy(&m_data[m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel * i], &m_data[m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel * j], m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel);
        //now copy buffer to bottom
        memcpy(&m_data[m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel * j], lineBuffer, m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel);
    }
    delete [] lineBuffer;
}

bool Image::load(const std::string & path)
{
    //clear current data
    m_width = 0;
    m_height = 0;
    if (m_data != nullptr) {
        delete [] m_data;
        m_data = nullptr;
    }
    if (m_palette != nullptr) {
        delete [] m_palette;
        m_palette = nullptr;
    }
    m_formatType = PixelInfo::BAD_FORMAT;
    //check the file signature and deduce its format
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path.c_str(), 0);
    if (fif == FIF_UNKNOWN) {
        //try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilename(path.c_str());
    }
    //format ok? check that the plugin has reading capabilities ...
    if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
        //ok, let's load the file
        FIBITMAP * fiBitmap = FreeImage_Load(fif, path.c_str());
        if (fiBitmap != nullptr)
        {
            //convert to one of the formats we support
            if (FreeImage_GetImageType(fiBitmap) == FIT_BITMAP)
            {
                if (FreeImage_GetColorType(fiBitmap) == FIC_PALETTE)
                {
                    if (FreeImage_GetBPP(fiBitmap) == 8)
                    {
                        m_formatType = PixelInfo::I8;
                    }
                    else
                    {
                        throw ImageException("Image::load() - Unsupported palette depth!");
                    }
                }
                else if (FreeImage_GetColorType(fiBitmap) == FIC_RGB)
                {
                    if (FreeImage_GetBPP(fiBitmap) == 24) 
                    {
                        m_formatType = PixelInfo::R8G8B8;
                    }
                    else if (FreeImage_GetBPP(fiBitmap) == 16)
                    {
                        if (FreeImage_GetRedMask(fiBitmap) != FreeImage_GetGreenMask(fiBitmap))
                        {
                            m_formatType = PixelInfo::R5G6B5;
                        }
                        else 
                        {
                            m_formatType = PixelInfo::X1R5G5B5;
                        }
                    }
                    else {
                        throw ImageException("Image::load() - Unsupported RGB format!");
                    }
                }
                else if (FreeImage_GetColorType(fiBitmap) == FIC_RGBALPHA)
                {
                    if (FreeImage_GetBPP(fiBitmap) == 32)
                    {
                        m_formatType = PixelInfo::R8G8B8A8;
                    }
                    else
                    {
                        throw ImageException("Image::load() - Unsupported RGBA format!");
                    }
                }
                else {
                    throw ImageException("Image::load() - Unsupported color type!");
                }
            }
            else if (FreeImage_GetImageType(fiBitmap) == FIT_UINT16)
            {
                m_formatType = PixelInfo::I16;
            }
            else {
                //free bitmap data
                FreeImage_Unload(fiBitmap);
                throw ImageException("Image::load() - Unsupported image format!");
            }
            //convert image to raw data. set up members
            m_width = FreeImage_GetWidth(fiBitmap);
            m_height = FreeImage_GetHeight(fiBitmap);
            const size_t pitch = m_width * PixelInfo::pixelInfo(m_formatType).bytesPerPixel;
            //const unsigned int pitch = FreeImage_GetPitch(fiBitmap);
            //allocate memory
            m_data = new uint8_t[m_height * pitch];
            //copy scanlines to image memory
            for (size_t i = 0; i < m_height; i++)
            {
                const BYTE * scanLine = FreeImage_GetScanLine(fiBitmap, (int)i);
                memcpy(m_data + (i * pitch), scanLine, pitch);
            }
            //if the image has a palette, copy that too
            if (FreeImage_GetPalette(fiBitmap) != nullptr && PixelInfo::pixelInfo(m_formatType).paletteEntries > 0) {
                m_palette = new uint8_t[PixelInfo::pixelInfo(m_formatType).paletteEntries * 4];
                memcpy(m_palette, FreeImage_GetPalette(fiBitmap), PixelInfo::pixelInfo(m_formatType).paletteEntries * 4);
            }
            //free bitmap data
            FreeImage_Unload(fiBitmap);
            return true;
        }
        else
        {
            throw ImageException("Image::load() - Failed to load image!");
        }
    }
    else
    {
        throw ImageException("Image::load() - File type unknown/unsupported!");
    }
    return false;
}

bool Image::save(const std::string & path)
{
	//check the file signature and deduce its format
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path.c_str(), 0);
	if (fif == FIF_UNKNOWN) {
		//try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	}
	//format ok? check that the plugin has writing capabilities ...
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsWriting(fif)) {
		//create bitmap for saving
		const PixelInfo info = PixelInfo::pixelInfo(m_formatType);
		FIBITMAP * bitmap = FreeImage_Allocate(m_width, m_height, info.bitsPerPixel);
		if (bitmap)
		{
			//ok, let's save
			const bool worked = FreeImage_Save(fif, bitmap, path.c_str());
			FreeImage_Unload(bitmap);
			return worked;
		}
		else
		{
			throw ImageException("Image::save() - Failed to create bitmap!");
		}
	}
	else
	{
		throw ImageException("Image::save() - File type unknown/unsupported!");
	}
	return false;
}

//-------------------------------------------------------------------------------------------------

ImageException::ImageException(const char * errorString) throw()
    : InternalException(errorString)
{
}

ImageException::ImageException(const std::string & errorString) throw()
    : InternalException(errorString)
{
}
