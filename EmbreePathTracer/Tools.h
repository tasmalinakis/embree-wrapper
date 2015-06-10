#pragma once
#include <glm\glm.hpp>
namespace path_tracer
{
	inline float randomNum() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); }
	inline glm::vec3 float3ToVec3(float arr[3]) { glm::vec3 vec; vec.x = arr[0]; vec.y = arr[1]; vec.z = arr[2]; return vec; }
}