#pragma once

#include "../Base.h"
#include "PixelFormat.h"
#include "ImageResample.h"

#include <string>
#include <stdint.h>

//TODO: Variable palette depth.
//TODO: Conversion from/to paletted images.
//TODO: Rescaling of paletted images.
//TODO: Thread-safety.

class Image
{
public:
    enum AspectRatioMode { DONT_CARE, //!<Dont't care about aspect.
                           KEEP_WIDTH, //!<Keep width, adjust height to fit.
                           KEEP_HEIGHT, //!<Keep height, adjust width to fit.
    };

    /*!
    Create a new empty image or use already allocated image data in same format.
    \param[in] width Width of image.
    \param[in] height Height of image.
    \param[in] source Source data for image. Needs to have the same correct and size!
    \param[in] palette Source palette for image in R8G8B8A8 format. Needs to have the correct format and size!
    \param[in] takeOwnerShip Pass bool for the image to take ownership of the source and palette pointer, else it'll make a copy.
    */
    Image(size_t width = 0, size_t height = 0, PixelInfo::FormatType formatType = PixelInfo::BAD_FORMAT, uint8_t * source = nullptr, uint8_t * palette = nullptr, bool takeOwnership = false);

    /*!
    Create a new image from already allocated image data in different format.
    \param[in] width Width of image.
    \param[in] height Height of image.
    \param[in] source Source data for image. Needs to have the proper size!
    \param[in] palette Source palette for image in R8G8B8A8 format. Needs to have the proper size!
    \param[in] sourceFormat Format of source data.
    */
    Image(size_t width, size_t height, PixelInfo::FormatType formatType, const uint8_t * source, uint8_t * palette, PixelInfo::FormatType sourceFormat);

    /*!
    Copy constructors. This makes a deep copy of the image data and does a conversion if neccessary.
    */
    Image(const Image & source);

    /*!
    Copy operator. This makes a deep copy of the image data and does a conversion if neccessary.
    */
    Image & operator=(const Image & source);

    /*!
    Destructor. Frees image data.
    */
    ~Image();

    /*!
    Try loading an image from path.
    \param[in] path Path to image to load.
    \return Returns true if the image could be loaded.
    \note All image content will be replaced and the image will be in the format resembling the file the most.
    */
    bool load(const std::string & path);

    /*!
    Try loading an image from raw data.
    \param[in] data Pointer to raw data in memory.
    \param[in] size Size of raw data in memory.
    \return Returns true if the image could be loaded.
    \note All image content will be replaced and the image will be in the format resembling the file data the most.
    */
    //bool load(const uint8_t * data, const size_t size = 0);

	/*!
	Try saving image from current data.
	\param[in] path Path to image to save. Make sure the file ending states the file type, e.g. ".png".
	\return Returns true if the image could be saved.
	*/
	bool save(const std::string & path);

    /*!
    Get pointer to image data.
    \return Returns a pointer to the raw image data.
    */
    const uint8_t * pixels() const { return (const uint8_t *)m_data; }

    /*!
    Get pointer to image data.
    \return Returns a pointer to the raw image data.
    */
    uint8_t * pixels() { return m_data; }

    /*!
    Get pointer to palette data in R8G8B8A8 format.
    \return Returns a pointer to the raw palette data.
    */
    const uint8_t * palette() const { return (const uint8_t *)m_palette; }

    /*!
    Get pointer to image data in R8G8B8A8 format.
    \return Returns a pointer to the raw image data.
    */
    uint8_t * palette() { return m_palette; }

    /*!
    Get the pixel type.
    \return Returns the pixel type.
    */
    PixelInfo::FormatType formatType() const { return m_formatType; }

    /*!
    Get image width.
    \return Returns the width of the image.
    */
    size_t width() const { return m_width; }

    /*!
    Get image height.
    \return Returns the height of the image.
    */
    size_t height() const { return m_height; }

    /*!
    Return image scaled to new dimensions.
    \param[in] width New width.
    \param[in] height New height.
    \param[in] aspectMode Optional. Pass aspect ration mode to honour.
    \param[in] fastMode. Do not reallocate memory. Faster, but uses more memory for the result image.
    */
    Image scaled(size_t width, size_t height, AspectRatioMode aspectMode = DONT_CARE, const ResampleFilter & filter = ResampleLinear()) const;

    /*!
    Scale image to destImage dimensions.
    \param[in] destImage Target image. This image will be scaled to destImage.width() x destImage.height().
    \note Should theoretically be faster, because it does not need to copy the data.
    */
    void scaleTo(Image & destImage, const ResampleFilter & filter = ResampleLinear()) const;

    /*!
    Flip image vertical.
    */
    void flipVertical();

    /*!
    Calculate color range of image. Delivers the min and max pixel color value.
    \return Returns the color range of the data.
    */
    //ColorRange colorRange() const;

protected:
    /*!
    INTERNAL. Copy or convert another image to internal data.
    */
    void copyToInternal(size_t width, size_t height, const uint8_t * source, const uint8_t * palette, PixelInfo::FormatType formatType);

private:
    uint8_t * m_data;
    uint8_t * m_palette; //!< Palette data in R8G8B8A8 format.
    size_t m_width;
    size_t m_height;
    PixelInfo::FormatType m_formatType;
    mutable ImageResample m_resampler;
};

//-------------------------------------------------------------------------------------------------

class ImageException: public InternalException
{
public:
    ImageException(const char * errorString) throw();
    ImageException(const std::string & errorString) throw();
};
