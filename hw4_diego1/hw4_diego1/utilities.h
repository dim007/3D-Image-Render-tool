#pragma once
#include <glm/glm.hpp>

using namespace glm;

typedef unsigned char Byte;
class Ray {
public:
	vec3 pos;
	vec3 dirn;
	float t_min, t_max;
	Ray(vec3 p, vec3 d) {
		dirn = d;
		pos = p;
	};
	Ray() {};
};
class Color {
public:
	vec3 col;
	Color(vec3 color) {
		col = color;
	}
	Color() {};
};
class Point {
	float x, y, z;
	Point(float x1, float y1, float z1) {
		x = x1;
		y = y1;
		z = z1;
	}
};
