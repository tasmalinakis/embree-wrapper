#pragma once

#include "Triangle.h"
#include "Tools.h"
#include "Material.h"

namespace path_tracer
{

	class Light
	{
		friend class Scene;
	public:

		~Light() {};

		glm::vec3 getRadiance() { return material->getRadiance(); }

		double getTriangleArea() { return triangle_area; }

		// get a point of this light.
		glm::vec3 samplePoint();

	private:
		Light(Triangle*& triangle, Material*& material) : triangle(triangle), material(material) {};
		Triangle* triangle;
		double triangle_area; // pdf = 1 / area
		Material* material;
		void calcArea();
	};
}