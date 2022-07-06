#ifndef _H_TRIANGLE_
#define _H_TRIANGLE_

#include "vec3.h"

struct Triangle
{
	vec3 v0;
	vec3 v1;
	vec3 v2;
	vec3 normal;

	inline Triangle() {};
	inline Triangle(const vec3& _v0, const vec3& _v1, const vec3& _v2) :v0(_v0), v1(_v1), v2(_v2)
	{
		normal = normalized(cross(_v0 - _v1, _v2 - _v1));
	}
};
#endif
