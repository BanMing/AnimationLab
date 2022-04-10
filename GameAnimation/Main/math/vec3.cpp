#include "vec3.h"

vec3 operator+(const vec3& l, const vec3& r)
{
	return vec3(l.x + r.x, l.y + r.y, l.z + r.z);
}

vec3 operator-(const vec3& l, const vec3& r)
{
	return vec3(l.x - r.x, l.y - r.y, l.z - r.z);
}

vec3 operator*(const vec3& v, float f)
{
	return vec3(v.x * f, v.y * f, v.z * f);
}

vec3 operator*(const vec3& l, const vec3& r)
{
	return vec3(l.x * r.x, l.y * r.y, l.z * r.z);
}

float dot(const vec3& l, const vec3& r)
{
	return l.x * r.x + l.y * r.y + l.z * r.z;
}

float lenSq(const vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// float distance = len(vec1 - vec2).
float len(const vec3& v)
{
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON)
	{
		return 0.0f;
	}
	return sqrtf(lenSq);
}

void normalize(vec3& v)
{
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON)
	{
		return;
	}
	float invLen = (1.0f / sqrtf(lenSq));

	v.x *= invLen;
	v.y *= invLen;
	v.z *= invLen;
}

vec3 normalized(const vec3& v)
{
	float lenSq = v.x * v.x + v.y * v.y + v.z * v.z;
	if (lenSq < VEC3_EPSILON)
	{
		return v;
	}
	float invLen = 1.0f / sqrtf(lenSq);

	return vec3(
		v.x * invLen,
		v.y * invLen,
		v.z * invLen
	);
}

// retrun angles in radians
// To convert radians to degrees, multiply by 57.2958f
// To convert degrees to radians , multiply by 0.0174533f.
float angle(const vec3& l, const vec3& r)
{
	float sqMagL = l.x * l.x + l.y * l.y + l.z * l.z;
	float sqMagR = r.x * r.x + r.y * r.y + r.z * r.z;

	if (sqMagL < VEC3_EPSILON || sqMagR < VEC3_EPSILON)
	{
		return  0.0f;
	}

	float dot = l.x * r.x + l.y * r.y + l.z * r.z;
	float len = sqrtf(sqMagL) * sqrtf(sqMagR);

	return acosf(dot / len);
}

vec3 project(const vec3& a, const vec3& b)
{
	float lenB = len(b);
	if (lenB < VEC3_EPSILON)
	{
		return vec3();
	}

	float scale = dot(a, b) / lenB;
	return normalized(b) * scale;
}

vec3 reject(const vec3& a, const vec3& b)
{
	vec3 prjection = project(a, b);
	return a - prjection;
}

vec3 reflect(const vec3& a, const vec3& b)
{
	vec3 prjection = project(a, b);
	return b + prjection - a;
}

vec3 cross(const vec3& l, const vec3& r)
{
	return vec3(
		l.y * r.z - l.z * r.y,
		l.z * r.x - l.x * r.z,
		l.x * r.y - l.y * r.x
	);
}

// liner interpolating
vec3 lerp(const vec3& s, const vec3& e, float t)
{
	return vec3(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);
}

// Interpolating on the shorest arc : spherical linear interpolation
vec3 slerp(const vec3& s, const vec3& e, float t)
{
	if (t < 0.01f)
	{
		return lerp(s, e, t);
	}

	vec3 from = normalized(s);
	vec3 to = normalized(e);
	float theta = angle(from, to);
	float sin_theta = sinf(theta);

	float a = sinf((1.0f - t) * theta) / sin_theta;
	float b = sinf(t * theta) / sin_theta;

	return from * a + to * b;
}

vec3 nlerp(const vec3& s, const vec3& e, float t)
{
	vec3 linear(
		s.x + (e.x - s.x) * t,
		s.y + (e.y - s.y) * t,
		s.z + (e.z - s.z) * t
	);

	return normalized(linear);
}

bool operator== (const vec3& l, const vec3& r)
{
	vec3 diff(l - r);

	return lenSq(diff) < VEC3_EPSILON;
}

bool operator!=(const vec3& l, const vec3& r)
{
	return !(l == r);
}