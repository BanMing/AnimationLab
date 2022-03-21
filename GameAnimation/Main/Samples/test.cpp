#include "test.h"
#include "Math/vec3.h"
#include<iostream>

void test_vec3()
{
	vec3 a(1, 1, 1);
	vec3 b(1, -1, 1);
	vec3 c = a - b;
	std::cout << "test vec3:" << c.x << c.y << c.z << std::endl;
}