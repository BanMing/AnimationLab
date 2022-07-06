#ifndef _H_RAY_
#define _H_RAY_

#include "../Math/vec3.h"
#include "../Math/Triangle.h"

struct Ray
{
	vec3 origin;
	vec3 direction;

	inline Ray() :direction(vec3(0, -1, 0)) {}
	inline Ray(const vec3& o) : origin(o), direction(vec3(0, -1, 0)) {}
	inline Ray(const vec3& o, const vec3& d) : origin(o), direction(d) {}
}; 

bool RaycastTriangle(const Ray& ray, const Triangle& triangle, vec3& hitPoint);

#endif
