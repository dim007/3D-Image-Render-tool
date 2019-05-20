#pragma once
#include <glm\glm.hpp>
#include "utilities.h"
enum LightType {
	Directional, Point
};
class Lights
{
public:
	LightType type;
	vec3 lpos;
	vec3 lcolor;
	vec3 ldirn;

	Lights();
	~Lights();
};
class LightD : public Lights 
{
public:
	LightD(vec3 dir, vec3 col) {
		ldirn = dir;
		lcolor = col;
		type = Directional;
	}
};
class LightP : public Lights
{
public:
	LightP(vec3 posn, vec3 col) {
		lpos = posn;
		lcolor = col;
		type = Point;
	}
};

