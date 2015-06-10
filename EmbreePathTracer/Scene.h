#pragma once
#include <vector>
#include "Mesh.h"

namespace path_tracer
{
	class Scene
	{
		friend class PathTracer;
	public:

		// create a mesh in the scene and return it
		Mesh* createMesh(float* world_transform = NULL);

		// reset the scene
		void resetScene();

		// get the saved meshes as an array
		inline Mesh* getMeshes(size_t& num_meshes) { num_meshes = meshes.size(); return meshes[0]; }

		// destructor
		~Scene();

	private:
		// private constructor
		Scene() {}

		// vector of meshes added to this scene
		std::vector<Mesh*> meshes;
	};
}