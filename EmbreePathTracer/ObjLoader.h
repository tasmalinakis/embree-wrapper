#pragma once
#include <tinyobjloader\tiny_obj_loader.h>
#include "Scene.h"
#include <string>
#include <vector>
#include "Tools.h"

namespace path_tracer
{
	// loads an OBJ file along with its mtl file, using tiny_obj_loader.
	Scene* loadObj(Scene* scene, std::string input_file);
}