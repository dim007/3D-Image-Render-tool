#include "primitive.h"
#include <iostream>

float Primitive::intersect(Ray ray, vec4 * intersection, vec4 * normHit) { return false; };
Triangle::Triangle(vec3 vv1, vec3 vv2, vec3 vv3)
{
	v1 = vv1;
	v2 = vv2;
	v3 = vv3;
	type = triangle;
}
Sphere::Sphere(vec3 pos1, float radius1) {
	pos = pos1;
	radius = radius1;
}
TriangleNorm::TriangleNorm(vec3 v11, vec3 v22, vec3 v33, vec3 v1_n, vec3 v2_n, vec3 v3_n) {
	v1 = v11;
	v2 = v22;
	v3 = v33;
	v1n = v1_n;
	v2n = v2_n;
	v3n = v3_n;
}
float Sphere::intersect(Ray ray, vec4 * intersection, vec4 * normHit) {
	//must use inverse for ray transf
	vec4 transfPos = inverse(transf) * vec4(ray.pos, 1); //keep homogenous for translation 
	transfPos = vec4(transfPos.x / transfPos.w, transfPos.y / transfPos.w, transfPos.z / transfPos.w, transfPos.w / transfPos.w);
	vec4 transfDirn = inverse(transf) * vec4(ray.dirn, 0); //not needed for translation
	vec4 spherePos = vec4(pos, 1);
	
	// Ray - Center
	vec4 RminusC = transfPos - spherePos;
	//t^2 * (P1.P1) + 2t(P1.(P0-C)) + (P0-C).(P0-C) - r^2 = 0
	float a = dot(transfDirn, transfDirn);
	float b = 2.f * dot(transfDirn, RminusC);
	float c = dot(RminusC, RminusC) - (radius * radius);
	//discriminant = no intersection
	float d = (b*b) - (4.f * a * c);
	if (d < 0)
		return -1;
	float t1 = (-b + sqrt(d)) / (2.f * a);
	float t2 = (-b - sqrt(d)) / (2.f * a);
	//Check 3 other cases: 2 real pos = pick small, both same = tangent to sphere
	//one pos one neg = choose pos
	if (t1 >= 0 && t2 >= 0) { //both pos
		if (t1 == t2) {
			*intersection = transf * (transfPos + transfDirn*t1);
			vec4 n = vec4((*intersection).x - spherePos.x, (*intersection).y - spherePos.y, (*intersection).z - spherePos.z, 1.f);
			*normHit = transpose(inverse(transf)) * n;
			return t1;
		}
		if (t1 < t2) {
			*intersection = transf * (transfPos + transfDirn*t1);
			vec4 n = vec4((*intersection).x - spherePos.x, (*intersection).y - spherePos.y, (*intersection).z - spherePos.z, 1.f);
			*normHit = transpose(inverse(transf)) * n;
			return t1;
		}
		if (t2 < t1) {
			*intersection = transf * (transfPos + transfDirn*t2);
			vec4 n = vec4((*intersection).x - spherePos.x, (*intersection).y - spherePos.y, (*intersection).z - spherePos.z, 1.f);
			*normHit = transpose(inverse(transf)) * n;
			return t2;
		}
	}
	else if (t1 < 0 || t2 < 0) {
		//if ray inside sphere negate to point norm out of sphere
		if (t1 < 0) {
			*intersection = transf * (transfPos + transfDirn*t2);
			vec4 n = vec4((*intersection).x - spherePos.x, (*intersection).y - spherePos.y, (*intersection).z - spherePos.z, 1.f);
			*normHit = transpose(inverse(transf)) * n;
			return t2;
		}
		else {
			*intersection = transf * (transfPos + transfDirn*t1);
			vec4 n = vec4((*intersection).x - spherePos.x, (*intersection).y - spherePos.y, (*intersection).z - spherePos.z, 1.f);
			*normHit = transpose(inverse(transf)) * n;
			return t1;
		}
	}
	return -1;
}
float TriangleNorm::intersect(Ray ray, vec4 * intersection, vec4 * normHit) {
	//todo
	return false;
}
float Triangle::intersect(Ray ray, vec4 * intersection, vec4 * normHit) {
	vec4 transfPos = inverse(transf) * vec4(ray.pos, 1); //needs w coord
	transfPos = vec4(transfPos.x / transfPos.w, transfPos.y / transfPos.w, transfPos.z / transfPos.w, transfPos.w / transfPos.w);
	vec4 transfDirn = inverse(transf) * vec4(ray.dirn, 0); //w coord irrelevnt
	//norm = C-A x B-A / || C-A x B-A ||
	vec3 norm = normalize(cross((v2 - v1), (v3 - v1)));
	*normHit = transpose(inverse(transf)) * vec4(norm,0.f);
	if (dot(vec3(transfDirn.x, transfDirn.y, transfDirn.z), norm) == 0)
		return -1;
	//plane equation = P.n - A.n = 0
	//plug in Ray equation for P, P0 + P1t
	//solve for t
	// t = A.n - P0.n / P1.n
	float t = (dot(v1, norm) - dot(vec3(transfPos.x, transfPos.y, transfPos.z), norm)) / dot(vec3(transfDirn.x, transfDirn.y, transfDirn.z), norm);
	//plug t back into Ray
	vec3 rayT = vec3(transfDirn.x, transfDirn.y, transfDirn.z)*t + vec3(transfPos.x, transfPos.y, transfPos.z);
	float a = dot(cross((v3 - v2), (rayT - v2)), norm);
	float b = dot(cross((v1 - v3), (rayT - v3)), norm);
	float g = dot(cross((v2 - v1), (rayT - v1)), norm);
	float tri = dot(cross((v2 - v1), (v3 - v1)), norm);
	//norm
	a = a / tri;
	b = b / tri;
	g = g / tri;
	float total = a + b + g;
	//P = aV1 + bV2 + gV3; iff a+b+g == 1 && a,b,g >= 0
	if (total == 1 && a >= 0 && b >= 0 && g >= 0) {
		*intersection = transf * (transfDirn*t + transfPos);
		return t;
	}
	return -1;
}

Triangle::~Triangle()
{
}
Sphere::~Sphere() {}

Primitive::Primitive()
{
}
TriangleNorm::~TriangleNorm(){}
Primitive::~Primitive()
{
}