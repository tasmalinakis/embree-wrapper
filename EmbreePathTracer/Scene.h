#pragma once
#include <vector>
#include "Mesh.h"
#include "Light.h"

namespace path_tracer
{
	class Scene
	{
		friend class PathTracer;
	public:

		// create a mesh in the scene and return it
		Mesh* createMesh(float* world_transform = NULL);

		// create a light in the scene and return it
		Light* createLight(Triangle*& tr, Material*& material);

		// get a material given its name. If it exists return a pointer to the existing one. Otherwise create a new one.
		Material*& getMaterial(const std::string mat_name);

		bool materialExists(std::string mat_name) { return mesh_materials[mat_name] != NULL; }

		// reset the scene
		void resetScene();

		// get the saved meshes as an array
		inline Mesh* getMeshes(size_t& num_meshes) { num_meshes = meshes.size(); return meshes[0]; }

		// get scene lights as an array
		inline Light** getLights(size_t& num_lights) { num_lights = lights.size(); return lights.data(); }

		// get ambient light
		inline glm::dvec3 getAmbientLight() { return ambient_light; }

		// set ambient light
		inline void setAmbientLight(glm::dvec3 ambient_light) { this->ambient_light = ambient_light; }

		// destructor
		~Scene();

	private:
		// private constructor
		Scene() { ambient_light = glm::dvec3(0.0); }

		// vector of meshes added to this scene
		std::vector<Mesh*> meshes;

		// vector of lights
		std::vector<Light*> lights;

		// ambient light
		glm::dvec3 ambient_light;

		// map saving the encountered materials by their name
		std::map<std::string, Material*> mesh_materials;

		// commit the scene and prepare for path tracing
		void commitScene();
	};
}