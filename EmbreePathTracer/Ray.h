#pragma once
#include <glm\glm.hpp>

namespace path_tracer
{
	typedef struct Ray
	{
		float* origin_data;
		float* point_at_data;
		float* intersection_data;
		void** hit_primitive_ptr;

		// the pixel this ray is contributing color to
		glm::dvec3* pixel_contributing;

		// the previous incoming direction, used to calculate angles
		glm::vec3 previous_direction_in;

		// the current strength of the ray, used because of iterative implementation
		glm::dvec3 ray_strength;

		// the probability distribution function of this ray
		double pdf;

		// the material this ray is shading
		Material* mat_shaded;

	} Ray;
}



