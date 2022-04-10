#include "quat.h"
#include <cmath>

quat angleAxis(float angle, const vec3& axis)
{
	vec3 norm = normalized(axis);
	float s = sinf(angle * 0.5f);

	return quat(norm.x * s, norm.y * s, norm.z * s, cosf(angle * 0.5f));
}

quat fromTo(const vec3& from, const vec3& to)
{
	vec3 f = normalized(from);
	vec3 t = normalized(to);

	if (f == t)
	{
		return quat();
	}
	else if (f == t * -1.0f)
	{
		// 180°
		// find the most perpendicular vector between the two vectors to create a pure quaternion
		vec3 ortho = vec3(1, 0, 0);
		if (fabsf(f.y) < fabsf(f.x))
		{
			ortho = vec3(0, 1, 0);
		}
		if (fabsf(f.z) < fabsf(f.y) && fabsf(f.z) < fabsf(f.x))
		{
			ortho = vec3(0, 0, 1);
		}

		vec3 axis = normalized(cross(f, ortho));
		return quat(axis.x, axis.y, axis.z, 0);
	}

	vec3 half = normalized(f + t);
	vec3 axis = cross(f, half);

	return quat(axis.x, axis.y, axis.z, dot(f, half));
}

vec3 getAxis(const quat& quat)
{
	return normalized(vec3(quat.x, quat.y, quat.z));
}

float getAngle(const quat& quat)
{
	return 2.0f * acosf(quat.w);
}


quat operator+(const quat& a, const quat& b)
{
	return quat(
		a.x + b.x,
		a.y + b.y,
		a.z + b.z,
		a.w + b.w
	);
}

quat operator-(const quat& a, const quat& b)
{
	return quat(
		a.x - b.x,
		a.y - b.y,
		a.z - b.z,
		a.w - b.w
	);
}

quat operator*(const quat& a, float b)
{
	return quat(
		a.x * b,
		a.y * b,
		a.z * b,
		a.w * b
	);
}

quat operator*(const quat& Q1, const quat& Q2)
{
	return quat(
		Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
	);
}

// vector rotates by quaternion ,get a vector
vec3 operator*(const quat& q, const vec3& v)
{

	return  q.vector * 2.0f * dot(q.vector, v) +
		v * (q.scalar * q.scalar - dot(q.vector, q.vector)) +
		cross(q.vector, v) * 2.0f * q.scalar;
}

quat operator-(const quat& q)
{
	return quat(
		-q.x,
		-q.y,
		-q.z,
		-q.w
	);
}

quat operator^(const quat& q, float f)
{
	float angle = 2.0f * acosf(q.scalar);
	vec3 axis = normalized(q.vector);

	float halfCos = cosf(f * angle * 0.5f);
	float halfSin = sinf(f * angle * 0.5f);

	return quat(
		axis.x * halfSin,
		axis.y * halfSin,
		axis.z * halfSin,
		halfCos
	);
}

bool operator==(const quat& left, const quat& right)
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON &&
			fabsf(left.y - right.y) <= QUAT_EPSILON &&
			fabsf(left.z - right.z) <= QUAT_EPSILON &&
			fabsf(left.w - left.w) <= QUAT_EPSILON);
}

bool operator!=(const quat& a, const quat& b)
{
	return !(a == b);
}

// to test the inverted quaternion
bool sameOrientation(const quat& left, const quat& right)
{
	return (fabsf(left.x - right.x) <= QUAT_EPSILON &&
			fabsf(left.y - right.y) <= QUAT_EPSILON &&
			fabsf(left.z - right.z) <= QUAT_EPSILON &&
			fabsf(left.w - left.w) <= QUAT_EPSILON)
		|| (fabsf(left.x + right.x) <= QUAT_EPSILON &&
			fabsf(left.y + right.y) <= QUAT_EPSILON &&
			fabsf(left.z + right.z) <= QUAT_EPSILON &&
			fabsf(left.w + left.w) <= QUAT_EPSILON);
}

// test how similar two quaternions are,same as vector dot
float dot(const quat& a, const quat& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// The length of a quaternion is thesquare root of the square length
float lenSq(const quat& q)
{
	return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
}

float len(const quat& q)
{
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return 0.0f;
	}
	return sqrtf(lenSq);
}

void normalize(quat& q)
{
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return;
	}
	float i_len = 1.0f / sqrtf(lenSq);

	q.x *= i_len;
	q.y *= i_len;
	q.z *= i_len;
	q.w *= i_len;
}

quat normalized(const quat& q)
{
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return quat();
	}
	float i_len = 1.0f / sqrtf(lenSq);

	return quat(
		q.x * i_len,
		q.y * i_len,
		q.z * i_len,
		q.w * i_len
	);
}

// The inverse of a normalized quaternion is its conjugate
quat conjugate(const quat& q)
{
	return quat(
		-q.x,
		-q.y,
		-q.z,
		q.w
	);
}

//The proper inverse of a quaternion is the conjugate divided by the squared length of the quaternion
quat inverse(const quat& q)
{
	float lenSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	if (lenSq < QUAT_EPSILON)
	{
		return quat();
	}
	float recip = 1.0f / lenSq;

	// conjugate / norm
	return quat(
		-q.x * recip,
		-q.y * recip,
		-q.z * recip,
		q.w * recip
	);
}

/*All input quaternion of interpolation methods should be neghborhood, dot > 0*/

// from,to can be not normalized.It travels on an arc.
// from,to are normalized,mix == lerp
quat mix(const quat& from, const quat& to, float t)
{
	return from * (1.0f - t) + to * t;
}

quat nlerp(const quat& from, const quat& to, float t)
{
	return normalized(from + (to - from) * t);
}

quat slerp(const quat& start, const quat& end, float t)
{
	if (fabsf(dot(start, end)) > 1.0f - QUAT_EPSILON)
	{
		return nlerp(start, end, t);
	}

	return normalized(((inverse(start) * end) ^ t) * start);
}

// Given a directionand a reference for which way is up, 
// it's possible to createa quaternion that looks in that direction with the correct orientation.
quat lookRotation(const vec3& direcion, const vec3& up)
{
	// Find orthonormal basis vectors
	vec3 f = normalized(direcion);
	vec3 u = normalized(up);
	vec3 r = cross(u, f);
	u = cross(f, r);

	// From world forward to object forward
	quat f2d = fromTo(vec3(0, 0, 1), f);

	// what direction is the new object up?
	vec3 objectUp = f2d * vec3(0, 1, 0);
	// From object up to desired up
	quat u2u = fromTo(objectUp, u);

	// Rotate to forward direction first, then twist to correct up
	quat result = f2d * u2u;
	// Don’t forget to normalize the result
	return normalized(result);
}

mat4 quatToMat4(const quat& q)
{
	vec3 r = q * vec3(1, 0, 0);
	vec3 u = q * vec3(0, 1, 0);
	vec3 f = q * vec3(0, 0, 1);

	return mat4(
		r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0, 0, 0, 1
	);
}

quat mat4ToQuat(const mat4& m)
{
	vec3 up = normalized(vec3(m.up.x, m.up.y, m.up.z));
	vec3 forward = normalized(vec3(m.forward.x, m.forward.y, m.forward.z));
	vec3 right = cross(up, forward);
	up = cross(forward, right);

	return lookRotation(forward, up);
}