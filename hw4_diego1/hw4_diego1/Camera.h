#pragma once
#include "utilities.h"
#include <glm/glm.hpp>

using namespace std;
class Camera
{
public:
	glm::vec3 lookfrom;
	glm::vec3 lookat;
	glm::vec3 up;
	float fovy;

	Camera();
	void generateRay(int x, int y, int width, int height, Ray* ray);
	~Camera();
};
