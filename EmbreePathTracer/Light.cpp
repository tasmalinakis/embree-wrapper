#include "Light.h"

namespace path_tracer
{
	void Light::calcArea()
	{
		// calcluate area of triangle
		// area = |AB x AC| / 2
		triangle_area = glm::length(glm::cross(*triangle->v1 - *triangle->v0, *triangle->v2 - *triangle->v0)) / 2.0;
	}

	glm::vec3 Light::samplePoint()
	{
		float rand1 = randomNum();
		float rand2 = randomNum();

		// calculate barycentrics
		rand1 = sqrtf(rand1);
		float a = 1 - rand1;
		float b = (1 - rand2) * rand1;
		float c = rand2 * rand1;

		assert(abs(a + b + c - 1.0) < 0.0001);
		// return random point inside triangle
		return a * (*triangle->v0) + b * (*triangle->v1) + c * (*triangle->v2);
	}
}