// MemSetTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string>

int main(int argc, char* argv[])
{
	float pos[3] = { 0.0f, 1.0f, 2.0f };
	float rot[4] = { 3.0f, 4.0f, 5.0f, 6.0f };

	char buffer[512];
	memset(buffer, '\0', 512);

	//serialize
	memcpy(&buffer[0], &pos[0], 3 * sizeof(float));
	memcpy(&buffer[3 * sizeof(float)], &rot[0], 4 * sizeof(float));

	//deserializer
	float px, py, pz, rx, ry, rz, rw;
	memcpy(&px, &buffer[0], sizeof(float));
	memcpy(&py, &buffer[sizeof(float)], sizeof(float));
	memcpy(&pz, &buffer[2 * sizeof(float)], sizeof(float));

	memcpy(&rx, &buffer[3 * sizeof(float)], sizeof(float));
	memcpy(&ry, &buffer[4 * sizeof(float)], sizeof(float));
	memcpy(&rz, &buffer[5 * sizeof(float)], sizeof(float));
	memcpy(&rw, &buffer[6 * sizeof(float)], sizeof(float));

	printf("%f %f %f %f %f %f %f\n", px, py, pz, rx, ry, rz, rw);

	getchar();
	return 0;
}

