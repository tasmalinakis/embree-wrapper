#pragma once
#include <glm/glm.hpp>

namespace path_tracer
{
	struct Triangle
	{
		const size_t prim_id;
		glm::vec3* v0;
		glm::vec3* v1;
		glm::vec3* v2; // pointer to data, because intersector interface dictates continuous allocation of all vertices in a mesh
		glm::vec3 color;
		glm::vec3 normal;
		class Material* material;

		void setMaterial(Material* material) { this->material = material; }
		Material* getMaterial() { return material; }

		Triangle(size_t prim_id, glm::vec3* v0, glm::vec3* v1, glm::vec3* v2, glm::vec3 color) : prim_id(prim_id), v0(v0), v1(v1), v2(v2), color(color)
		{
			// calculate normal here
		}

		Triangle(size_t prim_id, glm::vec3 color) : prim_id(prim_id), color(color) {}

		Triangle(size_t prim_id) : prim_id(prim_id) {}
	};
}