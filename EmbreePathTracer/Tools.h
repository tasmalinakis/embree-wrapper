#pragma once
#include <glm\glm.hpp>
namespace path_tracer
{
	inline float randomNum() { return static_cast<double>(rand()) / static_cast<double>(RAND_MAX + 1.0); }
	inline glm::vec3 float3ToVec3(float arr[3]) { glm::vec3 vec; vec.x = arr[0]; vec.y = arr[1]; vec.z = arr[2]; return vec; }
}