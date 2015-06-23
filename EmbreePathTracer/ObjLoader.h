#pragma once
#include <tinyobjloader\tiny_obj_loader.h>
#include "Scene.h"
#include <string>
#include <vector>
#include <unordered_set>
#include "Tools.h"

namespace path_tracer
{
	class ObjLoader
	{
	public:

		// constructor
		ObjLoader(Scene* scene, std::string obj_file, std::string lights_file) : scene(scene), obj_file(obj_file), lights_file(lights_file) {}

		// loads an OBJ file along with its mtl file, using tiny_obj_loader.
		void loadObj();

		// clean up
		void cleanUp() { scene_light_mats.clear(); }

	private:
		Scene* scene;
		std::string obj_file;
		std::string lights_file;

		std::unordered_set<std::string> scene_light_mats;

		void parseLightsFile();
	};
}