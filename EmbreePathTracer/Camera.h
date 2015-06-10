#pragma once
#include <glm\glm.hpp>

namespace path_tracer
{
	class Camera
	{
	public:
		Camera(int width, int height, glm::vec3 pos, glm::vec3 point_to) : aspect_ratio((float)width / height), pos(pos), point_to(point_to) { init(); }

		// const getters
		inline const glm::vec3& getUp() const { return up; }
		inline const glm::vec3& getPointTo() const { return point_to; }
		inline const glm::vec3& getPos() const { return pos; }
		inline const glm::vec3& getDirection() const { return direction; }
		inline const glm::vec3& getCx() const { return cx; }
		inline const glm::vec3& getCy() const { return cy; }

		//TODO move camera

	private:
		glm::vec3 up;
		glm::vec3 point_to;
		glm::vec3 pos;
		glm::vec3 direction;
		glm::vec3 cx;
		glm::vec3 cy;
		float aspect_ratio;
		float fov;

		void init();
	};
}