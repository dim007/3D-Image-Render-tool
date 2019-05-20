#include "scene.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <stack>
#include "include\FreeImage.h"

Scene::Scene()
{
}

void Scene::render() {
	FreeImage_Initialise();
	Ray ray;
	Color color;
	Film film = Film(width, height);
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++){
			camera.generateRay(i, j, width, height, &ray);
			trace(ray, depth, &color);
			film.commit(i, j, color);
	}
	film.writeImage(filename,width,height);
	FreeImage_DeInitialise();
}
bool Scene::intersect(Ray ray) {
	vec4 intersection, normHit;
	float min = 999999999999.f;
	for (vector<Sphere>::iterator it = spheres.begin(); it != spheres.end(); it++) {
		float t = it->intersect(ray, &intersection, &normHit);
		if (t < min && t > 0) {
			return true;
		}
	}
	for (vector<Triangle>::iterator it = tris.begin(); it != tris.end(); it++) {
		float t = it->intersect(ray, &intersection, &normHit);
		if (t > 0 && t < min) {
			return true;
		}
	}
	return false;
}
void Scene::trace(Ray ray, int depth, Color *color) {

	*color = Color(vec3(0.f, 0.f, 0.f)); //initialize to black
	
	Primitive * minPrim;
	vec4 normHit, minNorm;
	float min = 99999999999.f;
	float t;
	vec4 intersection, minIntersect;
	bool isHit = 0;
	for (vector<Sphere>::iterator it = spheres.begin(); it != spheres.end(); it++) {
		t = it->intersect(ray, &intersection, &normHit);
		if (t < min && t > 0) {
			minPrim = &(*it);
			min = t;
			isHit = 1;
			minNorm = normHit;
			minIntersect = intersection;
		}
	}
	for (vector<Triangle>::iterator it = tris.begin(); it != tris.end(); it++) {
		t = it->intersect(ray, &intersection, &normHit);
		if (t > 0 && t < min) {
			minPrim = &(*it);
			min = t;
			isHit = 1;
			minNorm = normHit;
			minIntersect = intersection;
		}
	}
	//for testing with only ambient + emission
	/*if (isHit) {
		*color = Color(minPrim->ambient + minPrim->emission);
		//return;
	}*/
	if (!isHit)
		return;
	// There is an intersection, loop through all light source
	vec3 eyedirn = normalize(camera.lookat - camera.lookfrom);
	vec3 norm = normalize(vec3(minNorm.x, minNorm.y, minNorm.z));
	vec3 lightDirn, half0;
	vec3 col, lighting = vec3(0.f);
	Ray lightRay;
	for (vector<Lights>::iterator it = lights.begin(); it != lights.end(); it++) {
		if (it->type == LightType::Directional) {
			lightDirn = normalize(it->ldirn);
			lightRay = Ray(vec3(minIntersect.x, minIntersect.y, minIntersect.z),lightDirn);
			half0 = normalize(lightDirn - eyedirn);
			col = ComputeLight(lightDirn, it->lcolor, norm, half0, minPrim->diffuse, minPrim->specular, minPrim->shininess);
		}
		else if (it->type == LightType::Point) {
			float distance = sqrt((it->lpos.x - minIntersect.x)*(it->lpos.x - minIntersect.x) + (it->lpos.y - minIntersect.y)*(it->lpos.y - minIntersect.y) + (it->lpos.z - minIntersect.z)*(it->lpos.z - minIntersect.z));
			float intensity = 1.f;
			if (attenuation.y != 0.f || attenuation.z != 0.f) {
				intensity = attenuation.y * distance + attenuation.z * distance * distance;
			}
			lightDirn = normalize(it->lpos - vec3(minIntersect));
			lightRay = Ray(vec3(minIntersect) + lightDirn /100.f, lightDirn);
			half0 = normalize(lightDirn - eyedirn);
			col = ComputeLight(lightDirn, it->lcolor, norm, half0, minPrim->diffuse, minPrim->specular, minPrim->shininess);
			col = col * intensity;
		}
		if (intersect(lightRay))
			col = vec3(0.f, 0.f, 0.f);
		lighting = col + lighting;
	}
	*color = Color(minPrim->ambient + minPrim->emission + lighting);
	/*
	//mirror reflection
	if (depth > 0) {
		reflectRay = createReflectRay(in.local, ray);

		// Make a recursive call to trace the reflected ray
		col = trace(reflectRay, depth + 1, &tempColor);
		*color += col * tempColor;
	}*/
	return;
}
//FROM Hw1
vec3 Scene::ComputeLight(vec3 direction, vec3 lightcolor, vec3 normal, vec3 halfvec, vec3 mydiffuse, vec3 myspecular, float myshininess) {

	float nDotL = dot(normal, direction);
	vec3 lambert = mydiffuse * lightcolor * std::max(nDotL, 0.f);

	float nDotH = dot(normal, halfvec);
	vec3 phong = myspecular * lightcolor * pow(std::max(nDotH, 0.f), myshininess);

	vec3 retval = lambert + phong;
	return retval;
}
//readfile written with hw2 readfile in mind
void Scene::readfile(const char* file) {
	string str, cmd;
	ifstream in;
	in.open(file);

	//FOR OBJs
	int maxverts = 0, maxvertnorms = 0;
	vector<vec3> vertices, verticesnorms1, verticesnorms2; //1 for pos, 2 for norm

	//LIGHTS
	attenuation = vec3(1.f, 0.f, 0.f);//default
	
	//PROPERTIES
	vec3 ambient = vec3(), diffuse = vec3(), specular = vec3(), emission = vec3();
	float shininess = 0;
	//for default ambience
	ambient = vec3(0.2f, 0.2f, 0.2f);
	

	//written with hw2 readfile as a base
	stack <glm::mat4> transfstack;
	transfstack.push(glm::mat4(1.0)); //push identity
	getline(in, str);
	while (in) {
		//removes comments #, and newlines/empty space
		if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
			stringstream s(str);
			s >> cmd;
			int i;
			float values[10]; //stores command arguments
			string strings[1];
			bool validinput;
			//*****GENERAL
			if (cmd == "size") {
				validinput = readvals(s, 2, values);
				if (validinput) {
					width = values[0];
					height = values[1];
				}
			}
			else if (cmd == "maxdepth") {
				validinput = readvals(s, 1, values);
				if (validinput)
					depth = values[0];
			}
			else if (cmd == "output") {
				validinput = readstrings(s, 1, strings);
				//default value in scene.h "raytrace.png"
				if (validinput) {
					filename = strings[0];
					cout << filename << endl;
				}
			}

			//***** CAMERA
			else if (cmd == "camera") {
				validinput = readvals(s, 10, values);
				if (validinput) {
					camera.lookfrom = vec3(values[0], values[1], values[2]);
					camera.lookat = vec3(values[3], values[4], values[5]);
					camera.up = vec3(values[6], values[7], values[8]);
					camera.fovy = values[9];
				}
			}
			///******* GEOMETRY
			else if (cmd == "maxverts") {
				validinput = readvals(s, 1, values);
				if (validinput)
					maxverts = values[0];
			}
			else if (cmd == "maxvertnorms") {
				validinput = readvals(s, 1, values);
				if (validinput)
					maxvertnorms = values[0];
			}
			else if (cmd == "vertex") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					vertices.push_back(vec3(values[0], values[1], values[2]));
				}
			}
			else if (cmd == "vertexnormal") {
				validinput = readvals(s, 6, values);
				if (validinput) {
					verticesnorms1.push_back(vec3(values[0], values[1], values[2]));
					verticesnorms2.push_back(vec3(values[3], values[4], values[5]));
				}
			}
			else if (cmd == "sphere") {
				validinput = readvals(s, 4, values);
				if (validinput) {
					vec3 pos = vec3(values[0], values[1], values[2]);
					float rad = values[3];
					Sphere obj = Sphere(pos, rad);
					obj.ambient = ambient;
					obj.diffuse = diffuse;
					obj.emission = emission;
					obj.shininess = shininess;
					obj.specular = specular;
					obj.transf = transfstack.top();
					//primitives[numobj] = &obj;
					spheres.push_back(obj);
					//numobj++;
				}
			}
			else if (cmd == "tri") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					vec3 v1 = vertices[(int)values[0]];
					vec3 v2 = vertices[(int)values[1]];
					vec3 v3 = vertices[(int)values[2]];
					Triangle obj = Triangle(v1, v2, v3);
					obj.ambient = ambient;
					obj.diffuse = diffuse;
					obj.emission = emission;
					obj.shininess = shininess;
					obj.specular = specular;
					obj.transf = transfstack.top();
					tris.push_back(obj);
					//primitives[numobj] = &obj;
					//numobj++;
				}
			}
			else if (cmd == "trinormal") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					vec3 v1 = verticesnorms1[(int)values[0]];
					vec3 v1_n = verticesnorms2[(int)values[0]];
					vec3 v2 = verticesnorms1[(int)values[1]];
					vec3 v2_n = verticesnorms2[(int)values[1]];
					vec3 v3 = verticesnorms1[(int)values[2]];
					vec3 v3_n = verticesnorms2[(int)values[2]];

					Primitive obj = TriangleNorm(v1, v2, v3, v1_n, v2_n, v3_n);
					obj.ambient = ambient;
					obj.diffuse = diffuse;
					obj.emission = emission;
					obj.shininess = shininess;
					obj.specular = specular;
					obj.transf = transfstack.top();
					//	primitives[numobj] = &obj;
					//numobj++;
				}
			}
			///******* TRANSFORMS
			else if (cmd == "translate") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					mat4 translation = Transform::translate(values[0], values[1], values[2]);
					rightmultiply(translation, transfstack); //mult top
				}
			}
			else if (cmd == "rotate") {
				validinput = readvals(s, 4, values);
				if (validinput) {
					vec3 axis = vec3(values[0], values[1], values[2]);
					mat3 rotation = Transform::rotate(values[3], axis);
					rightmultiply(mat4(rotation), transfstack); //mult top
				}
			}
			else if (cmd == "scale") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					mat4 scaler = Transform::scale(values[0], values[1], values[2]);
					rightmultiply(scaler, transfstack);
				}
			}
			else if (cmd == "pushTransform") {
				transfstack.push(transfstack.top());
			}
			else if (cmd == "popTransform") {
				if (transfstack.size() <= 1) {
					cerr << "Stack has no elements.  Cannot Pop\n";
				}
				else {
					transfstack.pop();
				}
			}
			//****** LIGHTS
			else if (cmd == "directional") {
				validinput = readvals(s, 6, values);
				if (validinput) {
					vec3 dirn = vec3(values[0], values[1], values[2]);
					vec3 color = vec3(values[3], values[4], values[5]);
					LightD light = LightD(dirn, color);
					lights.push_back(light);
				}
			}
			else if (cmd == "point") {
				validinput = readvals(s, 6, values);
				if (validinput) {
					vec3 pos = vec3(values[0], values[1], values[2]);
					vec3 color = vec3(values[3], values[4], values[5]);
					LightP light = LightP(pos, color);
					lights.push_back(light);
				}
			}
			else if (cmd == "attenuation") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					attenuation = vec3(values[0], values[1], values[2]);
				}
			}
			else if (cmd == "ambient") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					ambient = vec3(values[0], values[1], values[2]);
				}
			}
			//****** MATERIALS
			else if (cmd == "diffuse") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					diffuse = vec3(values[0], values[1], values[2]);
				}
			}
			else if (cmd == "specular") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					specular = vec3(values[0], values[1], values[2]);
				}
			}
			else if (cmd == "shininess") {
				validinput = readvals(s, 1, values);
				if (validinput) {
					shininess = values[0];
				}
			}
			else if (cmd == "emission") {
				validinput = readvals(s, 3, values);
				if (validinput) {
					emission = vec3(values[0], values[1], values[2]);
				}
			}
			else {
				cerr << "Unknown Command: " << cmd << " Skipping \n";
			}
		}
		getline(in, str);
	}
}
//FUNCTION TAKEN FROM HW2 
// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool Scene::readvals(stringstream &s, const int numvals, float* values)
{
	for (int i = 0; i < numvals; i++) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}
bool Scene::readstrings(stringstream &s, const int numvals, string* values)
{
	for (int i = 0; i < numvals; i++) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}
//ALSO TAKEN FROM HW2
void Scene::rightmultiply(const mat4 & M, stack<mat4> &transfstack)
{
	mat4 &T = transfstack.top();
	T = T * M;
}

Scene::~Scene()
{
}
