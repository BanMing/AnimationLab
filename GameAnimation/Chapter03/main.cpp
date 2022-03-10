//#include "mat4.h"
#include "../Chapter02/vec3.h"
#include <iostream>
int main(int argc, const char** argv)
{
	//mat4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	//std::cout << a.v[2] << std::endl;

	vec3 a(1, 1, 1);
	vec3 b(1, 1, 1);
	vec3 c = a - b;
	std::cout << c.x << c.y << c.z << std::endl;
	return 0;
}