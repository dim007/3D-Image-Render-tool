#pragma once
#include "utilities.h"
#include <glm/glm.hpp>

using namespace glm;
class Primitive
{
public:
	enum Type { triangle, sphere };

	Type type;
	mat4 transf;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
public:
	Primitive();
	virtual float intersect(Ray ray, vec4 * intersection, vec4 * normHit);
	~Primitive();
};

class Sphere : public Primitive
{
public:
	vec3 pos;
	float radius;

	float intersect(Ray ray, vec4 * intersection, vec4 * normHit);
	Sphere(vec3 pos1, float radius1);
	~Sphere();
};

class Triangle : public Primitive
{
public:
	vec3 v1, v2, v3;

	float intersect(Ray ray, vec4 * intersection, vec4 * normHit);
	Triangle(vec3 vv1, vec3 vv2, vec3 vv3);
	~Triangle();
};
class TriangleNorm : public Primitive
{
public:
	vec3 v1, v2, v3;
	vec3 v1n, v2n, v3n;

	float intersect(Ray ray, vec4 * intersection, vec4 * normHit);
	TriangleNorm(vec3 v11, vec3 v22, vec3 v33, vec3 v1_n, vec3 v2_n, vec3 v3_n);
	~TriangleNorm();
};