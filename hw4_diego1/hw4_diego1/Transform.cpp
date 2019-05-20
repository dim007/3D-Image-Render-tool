// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"
#include <math.h>

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
    // YOUR CODE FOR HW2 HERE
    // Please implement this.  Likely the same as in HW 1.  
	float radians = degrees * 3.14159265f / 180.f;
	mat3 Identity = mat3();
	//axis needs a small tweak
	vec3 naxis = glm::normalize(axis);
	mat3 aaT = mat3(naxis.x * naxis.x, naxis.x * naxis.y, naxis.x * naxis.z,
		naxis.x * naxis.y, naxis.y * naxis.y, naxis.y * naxis.z,
		naxis.x * naxis.z, naxis.y * naxis.z, naxis.z * naxis.z); //goes in order of columns 
	mat3 Astar = mat3(0.f, naxis.z, -naxis.y,
		-naxis.z, 0.f, naxis.x,
		naxis.y, -naxis.x, 0.f);
	mat3 rotation = float(cos(radians))*Identity + float((1 - cos(radians)))*aaT + float(sin(radians))*Astar;

	// You will change this return call
	return rotation;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
    // YOUR CODE FOR HW2 HERE
	mat3 transform = rotate(degrees, normalize(up)); //rotate around the "up" axis
	eye = transform*eye;				 //update the view
	up = transform*up;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
    // YOUR CODE FOR HW2 HERE 
	mat3 transform = rotate(degrees, glm::normalize(glm::cross(eye, up)));
	up = transform*up;		 //constantly update "up" to match the new "up"
	eye = transform*eye;	 //update the view else no movement
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
    // YOUR CODE FOR HW2 HERE
	vec3 w = glm::normalize(eye - center); //eye - center
	vec3 u = glm::normalize(glm::cross(up, w));	// up==b , so bxw
	vec3 v = glm::cross(w, u);				//find last coordinate
	mat4 rotation = mat4(u.x, v.x, w.x, 0, u.y, v.y, w.y, 0, u.z, v.z, w.z, 0, 0, 0, 0, 1);
	mat4 translation = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -eye.x, -eye.y, -eye.z, 1); //ERROR FIX: forgot negatives
	mat4 RT = rotation * translation;
	return RT;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    mat4 ret;
    // YOUR CODE FOR HW2 HERE
    // New, to implement the perspective transform as well.  
	//Perspective = (d/aspect	0	0	0)
				//	(0			d	0	0)
				//	(0			0	A	B)
				//	(0			0	-1	0)
	//A = - far_plane + near_plane / (farplane - nearplane)
	//B = - 2*farplane*nearplane / (farplane - nearplane)
	//d = cot(field of view / 2)
	float radians = (fovy / 2) * 3.14159265f / 180.f;
	float d = 1/tan(radians); //only works in radians
	float A = -(zFar + zNear) / (zFar - zNear);
	float B = -(2 * zFar * zNear) / (zFar - zNear);
	ret = mat4((d/aspect), 0, 0, 0, 0, d, 0, 0, 0, 0, A, -1, 0, 0, B, 0);
    return ret;
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    mat4 ret;
    // YOUR CODE FOR HW2 HERE
    // Implement scaling 

	//Aplies scaling to identity matrix
	ret = mat4(sx, 0, 0, 0, 0, sy, 0, 0, 0, 0, sz, 0, 0, 0, 0, 1);
    return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
    mat4 ret;
    // YOUR CODE FOR HW2 HERE
    // Implement translation 

	//Applies translation of tx, ty, tz to a 4x4 matrix identity
	ret = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, tx, ty, tz, 1);
    return ret;
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
