#pragma once
#include <glm/glm.hpp>
#include "Camera.h"
#include "Transform.h"
#include <string>
#include "utilities.h"
#include "Film.h"
#include <stack>
#include "Primitive.h"
#include <vector>
#include <memory>
#include "Lights.h"

using namespace glm;
using namespace std;
class Scene
{
public:
	int height, width;
	int depth;
	string filename = "raytrace.png";
	Camera camera;
	//Primitive * primitives[20];
	vector<Sphere> spheres;
	vector<Triangle> tris;
	vector<Lights> lights;
	vec3 attenuation;
	
	Scene();
	void readfile(const char * file);
	vec3 Scene::ComputeLight(vec3 direction, vec3 lightcolor, vec3 normal, vec3 halfvec, vec3 mydiffuse, vec3 myspecular, float myshininess);
	bool readvals(stringstream &s, const int numvals, float* values);
	bool readstrings(stringstream &s, const int numvals, string* values);
	void rightmultiply(const mat4 & M, stack<mat4> &transfstack);
	void trace(Ray ray, int depth, Color *color);
	bool intersect(Ray ray);
	void render();
	~Scene();
};

