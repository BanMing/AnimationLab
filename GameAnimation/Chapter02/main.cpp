#include "vec3.h"
#include <iostream>

int main(int argc, const char** argv)
{
	vec3 a(1, 1, 1);
	vec3 b(1, 1, 1);
	vec3 c = a - b;
	return 0;
}