#include "Transform.h"
#include<cmath>

Transform combine(const Transform& a, const Transform& b)
{
	Transform out;

	out.scale = a.scale * b.scale;
	out.rotation = b.rotation * a.rotation;

	out.position = a.rotation * (a.scale * b.position);
	out.position = a.position + out.position;

	return out;
}

// Inverting a transform can be done to remove the effects of onetransform from another transform
Transform inverse(const Transform& t)
{
	Transform inv;
	inv.rotation = inverse(t.rotation);

	inv.scale.x = fabs(t.scale.x) < VEC3_EPSILON ? 0.0f : 1.0f / inv.scale.x;
	inv.scale.y = fabs(t.scale.y) < VEC3_EPSILON ? 0.0f : 1.0f / inv.scale.y;
	inv.scale.z = fabs(t.scale.z) < VEC3_EPSILON ? 0.0f : 1.0f / inv.scale.z;

	vec3 invTranslation = t.position * -1.0f;

	inv.position = inv.rotation * (inv.scale * invTranslation);

	return inv;
}

//To make the model appear animated, 
//you need to interpolate or mix between the transformation of these frames.
//blend
Transform mix(const Transform& a, const Transform& b, float t)
{
	quat bRot = b.rotation;
	//make sure quaternion is neghborhood,travel the shorest arc
	if (dot(a.rotation, b.rotation) < 0.0f)
	{
		bRot = -bRot;
	}

	return Transform(lerp(a.position, b.position, t),
					 nlerp(a.rotation, bRot, t),
					 lerp(a.scale, b.scale, t));
}

bool operator==(const Transform& a, const Transform& b)
{
	return a.position == b.position &&
		a.rotation == b.rotation &&
		a.scale == b.scale;
}

bool operator!=(const Transform& a, const Transform& b)
{
	return !(a == b);
}

// mat4 can represent trasform in shader 
mat4 transformToMat4(const Transform& t)
{
	// First, extract the rotation basis of the transform
	vec3 x = t.rotation * vec3(1, 0, 0);
	vec3 y = t.rotation * vec3(0, 1, 0);
	vec3 z = t.rotation * vec3(0, 0, 1);

	// Next, scale the basis vectors
	x = x * t.scale.x;
	y = y * t.scale.y;
	z = z * t.scale.z;

	// Extract the position of the transform
	vec3 p = t.position;

	// Create matrix
	return mat4(
		x.x, x.y, x.z, 0,   // X basis (& Scale)
		y.x, y.y, y.z, 0,   // Y basis (& scale)
		z.x, z.y, z.z, 0,   // Z basis (& scale)
		p.x, p.y, p.z, 1);  // Position
}
// M = SRT
Transform mat4ToTransform(const mat4& m)
{
	Transform out;

	out.position = vec3(m.v[12], m.v[13], m.v[14]);
	out.rotation = mat4ToQuat(m);

	mat4 rotScaleMat(
		m.v[0], m.v[1], m.v[2], 0,
		m.v[4], m.v[5], m.v[6], 0,
		m.v[8], m.v[9], m.v[10], 0,
		0, 0, 0, 1
	);
	mat4 invRotMat = quatToMat4(inverse(out.rotation));
	mat4 scaleSkewMat = rotScaleMat * invRotMat;

	out.scale = vec3(
		scaleSkewMat.v[0],
		scaleSkewMat.v[5],
		scaleSkewMat.v[10]
	);

	return out;
}


// transform all the vertices using the Transform structure(or a matrix) 
// to properly display the model
vec3 transformPoint(const Transform& a, const vec3& b)
{
	vec3  out;
	out = a.rotation * (a.scale * b);
	out = a.position + out;

	return  out;
}

// no need add the position
vec3 transformPoint(const Transform& a, const vec3& b) 
{
	vec3 out;
	out = a.position * (a.scale * b);

	return out;
}