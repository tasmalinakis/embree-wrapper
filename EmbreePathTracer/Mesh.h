#pragma once
#include "Triangle.h"
#include <vector>
#include <map>
#include "Material.h"

namespace path_tracer
{

	class Mesh
	{
		friend class Scene;
	public:

		// destructor
		~Mesh();

		// add a triangle to the mesh
		Triangle* addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 color);

		Triangle* addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2);

		// get the vertices of every triangle in the order they were inserted as a float array
		float* getVertexArray();

		// get the Triangle* structs of every triangle in the order they were inserted as a void* array
		void** getPrimData();

		// get the number of the inserted Triangles
		inline int getTrianglesCount() { return mesh.prim_data.size(); }

		inline float* getWorldTransform() { return world_transform; }

		// pointers... pointers everywhere!
		static inline Triangle* getTriangleFromVoidPtr(void* ptr) { return (Triangle*)ptr; }

		// debug
		void printTriangles();


	private:

		// constructor taking world_transform as argument
		Mesh(float* world_transform = NULL) : world_transform(world_transform) {}

		// member variables
		float* world_transform;

		// mesh internal structure
		struct _Mesh
		{
			// the aggregated data of every added vertex. Keep it that way because of easy transformation to float*
			std::vector<glm::vec3> vertex_data;

			// pointer to Triangle structure for every triangle in this mesh
			std::vector<Triangle*> prim_data;
		} mesh;

		// fix the references in Triangle structs before returning prim_data array
		void _fixReferences();
		};
}