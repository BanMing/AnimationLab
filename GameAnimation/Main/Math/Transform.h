
#ifndef _H_TRANSFORM_
#define _H_TRANSFORM_

#include "vec3.h"
#include "quat.h"

struct Transform
{
	vec3 position;
	quat rotation;
	vec3 scale;

	Transform() :
		position(vec3(0, 0, 0)),
		rotation(quat(0, 0, 0, 1)),
		scale(vec3(1, 1, 1))
	{
	}

	Transform(const vec3& _position, const quat& _rotation, const vec3& _scale) :
		position(_position), rotation(_rotation), scale(_scale)
	{
	}
};

#endif //_H_TRANSFORM_

Transform combine(const Transform& a, const Transform& b);
Transform inverse(const Transform& t);
Transform mix(const Transform& a, const Transform& b, float t);

bool operator==(const Transform& a, const Transform& b);
bool operator!=(const Transform& a, const Transform& b);

mat4 transformToMat4(const Transform& t);
Transform mat4ToTransform(const mat4& m);

vec3 transformPoint(const Transform& a, const vec3& b);
vec3 transformVector(const Transform& a, const vec3& b);