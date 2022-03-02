#include "mat4.h"
#include <cmath>

bool operator==(const mat4& a, const mat4& b)
{
	for (size_t i = 0; i < 16; i++)
	{
		if (fabs(a.v[i] - b.v[i]) > MAT4_EPSILON)
		{
			return false;
		}
	}
	return true;
}

bool operator!=(const mat4& a, const mat4& b)
{
	return !(a == b);
}
