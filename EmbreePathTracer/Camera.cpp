#include "Camera.h"

namespace path_tracer
{
	void Camera::init()
	{
		// calculate the direction
		direction = glm::normalize(point_to - pos);

		up = glm::vec3(0, 1, 0);
		cx = glm::vec3(aspect_ratio * 0.75355f, 0, 0); //0.5135f
		cy = glm::normalize(glm::cross(cx, direction)) * 0.75355f;
	}
}