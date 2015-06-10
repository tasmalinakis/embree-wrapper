#pragma once
#include <Intersector.h>
#include "Tools.h"
#include "Scene.h"
#include "Camera.h"
#include "Material.h"

using namespace intersector;

namespace path_tracer
{
	class PathTracer
	{
	public:
		// factory method to create a path tracer
		static PathTracer* createPathTracer(const unsigned int scr_width, const unsigned int scr_height);

		// render a single frame
		void render(glm::dvec3 *&img_buffer);

		// get the scene this Path Tracer is rendering
		inline Scene* getScene() { return scene; }

		// commit the scene to the intersector API reporting any errors to the stderr stream
		void commitScene();

		// reset the scene this Path Tracer is rendering
		isect_error_t resetScene();

		// destructor
		~PathTracer();

		// debug
		void renderTest(glm::dvec3 *&img_buffer);

	private:
		// private constructor
		PathTracer(const int isect_id, const unsigned int scr_width, const unsigned int scr_height) : isect_id(isect_id), scr_width(scr_width), scr_height(scr_height) { init(); }

		// member variables
		const int isect_id;
		const unsigned int scr_width;
		const unsigned int scr_height;
		Scene* scene;
		Camera* camera;

		// holds the information for the rays being cast
		struct RayInfo
		{
			float* origin_data;
			float* point_at_data;
			float* intersection_data;
			void** hit_primitive_ptr;

			~RayInfo()
			{
				delete[] origin_data;
				delete[] point_at_data;
				delete[] intersection_data;
				delete[] hit_primitive_ptr;
			}
		} *normal_ray_info, *shadow_ray_info;

		// methods

		// initialize arrays
		void init();
	};
}