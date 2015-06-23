#pragma once

#include "Triangle.h"

namespace path_tracer
{

	class Light
	{
	public:
		Light();

		Light(Triangle* triangle) : triangle(triangle) {};

		~Light();

		sample();

	private:
		Triangle* triangle;
		double pdf;
	};
}