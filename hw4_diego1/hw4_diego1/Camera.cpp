#include "camera.h"


Camera::Camera()
{
}
//Create a ray starting from the camera that passes through the 
//corresponding pixel(sample.x, sample.y) on the image plane.
void Camera::generateRay(int x, int y, int width, int height, Ray* ray) {

	//must realign ray to center of pixel
	float i = x + 0.5f;
	float j = y + 0.5f;
	float aspect = (float)width / (float)height;
	float fovyR = fovy * 3.14159265f / 180.f;
	float fovx = 2 * atan(tan(fovyR * 0.5f) * aspect);

	float alpha = tan(fovx * 0.5f) * (j - ((float)width * 0.5f)) / ((float)width * 0.5f);
	//convert fovy to radians
	float beta = tan(fovyR * 0.5f) * (((float)height * 0.5f) - i) / ((float)height * 0.5f);

	//camera coord frame
	vec3 a = lookfrom - lookat;//eye - center;
	vec3 b = up;
	vec3 w = normalize(a);
	vec3 u = normalize(cross(w, b));
	vec3 v = cross(w, u);

	vec3 ray_angled = normalize((alpha*u + beta*v - w));
	ray->dirn = ray_angled;
	ray->pos = lookfrom;
	//ray_angled = lookfrom + ray_angled;
	
}

Camera::~Camera()
{
}
