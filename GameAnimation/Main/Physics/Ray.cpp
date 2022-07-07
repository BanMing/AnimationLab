#include "Ray.h"

bool RaycastTriangle(const Ray& ray, const Triangle& triangle, vec3& hitPoint)
{
	const float EPSILON = 0.0000001f;

	vec3 vertex0 = triangle.v0;
	vec3 vertex1 = triangle.v1;
	vec3 vertex2 = triangle.v2;

	vec3 rayVector = ray.direction;
	vec3 rayOrigin = ray.origin;

	vec3 edge1, edge2, h, s, q;
	float a, f, u, v;

	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;

	h = cross(rayVector, edge2);
	a = dot(edge1, h);

	if (a > -EPSILON && a < EPSILON)
	{
		// This ray is parallel to this triangle
		return false;
	}

	f = 1.0f / a;
	s = rayOrigin - vertex0;
	u = f * dot(s, h);
	if (u < 0.0f || u>1.0f)
	{
		return false;
	}

	q = cross(s, edge1);
	v = f * dot(rayVector, q);
	if (v < 0.0f || u + v > 1.0f)
	{
		return false;
	}

	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		hitPoint = rayOrigin + rayVector * t;
		return true;
	}

	return false;
}
