#pragma once
#include <string>
#include "utilities.h"
#include <glm/glm.hpp>

using namespace std;
class Film
{
public:
	int pix, width, height;
	Byte *pixels;
	int commit_count;

	// Will write the color to (sample.x, sample.y) on the image
	void commit(int i, int j, Color& color);
		// Output image to a file
	void writeImage(string filename, int width, int height);
	Film(int w, int h);
	~Film();
};

