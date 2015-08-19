#include "Image.h"

void main()
{
    Image png;
    png.load("lena_512_24.png");
    Image image1 = png.scaled(64, 77);
	image1.save("1.png");
    Image image2(0, 0, PixelInfo::R5G6B5);
    image2 = image1;
	image2.save("2.png");
}