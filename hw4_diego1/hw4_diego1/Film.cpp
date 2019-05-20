#include "film.h"
#include <iostream>
#include <string>
#include <math.h>
#include "include\FreeImage.h"

typedef unsigned char Byte;
void Film::commit(int i, int j, Color& color) {
	int jj = width - j;
	int ii = height - i;

	float f2 = std::fmax(0.0, std::fmin(1.0, color.col.z));
	pixels[3*(i*width + (width-1-j))] = Byte(floor(f2 == 1.0 ? 255 : f2 * 256.0));
	f2 = std::fmax(0.0, std::fmin(1.0, color.col.y));
	pixels[3*(i*width + (width - 1 - j)) + 1] = Byte(floor(f2 == 1.0 ? 255 : f2 * 256.0));
	f2 = std::fmax(0.0, std::fmin(1.0, color.col.x));
	pixels[3*(i*width + (width - 1 - j)) + 2] = Byte(floor(f2 == 1.0 ? 255 : f2 * 256.0));
	

}
void Film::writeImage(string filename, int width, int height) {

	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	std::cout << "Saving screenshot: " << filename << "\n";

	FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
	delete pixels;
}

Film::Film(int w, int h)
{
	pix = w*h;
	width = w;
	height = h;
	pixels = new Byte[3 * pix];
}


Film::~Film()
{
}
