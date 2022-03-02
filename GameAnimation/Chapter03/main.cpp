#include "mat4.h"
#include <iostream>
int main(int argc, const char** argv)
{
	mat4 a(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	std::cout << a.v[2] << std::endl;
	return 0;
}