#pragma once
#include <Intersector.h>
#include "Tools.h"
#include "Scene.h"
#include "Camera.h"
#include "Material.h"
#include "Ray.h"
#include <chrono>

#define PI 3.14159265358979323846

using namespace intersector;

namespace path_tracer
{
	class PathTracer
	{
	public:
		// factory method to create a path tracer
		static PathTracer* createPathTracer(const unsigned int scr_width, const unsigned int scr_height, const unsigned int samples_per_pixel = 1);

		// render a single frame
		void render(glm::dvec3 *&img_buffer);

		// direct render
		void renderDirect(glm::dvec3 *&img_buffer);

		void renderGI(glm::dvec3 *&img_buffer);

		// get the scene this Path Tracer is rendering
		inline Scene* getScene() { return scene; }

		// commit the scene to the intersector API reporting any errors to the stderr stream
		void commitScene();

		// reset the scene this Path Tracer is rendering
		isect_error_t resetScene();

		// destructor
		~PathTracer();

	private:
		// private constructor
		PathTracer(const int isect_id, const unsigned int scr_width, const unsigned int scr_height, const unsigned int samples_per_pixel) : 
			isect_id(isect_id), scr_width(scr_width), scr_height(scr_height), samples_per_pixel(samples_per_pixel), num_rays(scr_height * scr_width * samples_per_pixel) { init(); }

		// member variables
		const int isect_id;
		const unsigned int scr_width;
		const unsigned int scr_height;
		const unsigned int samples_per_pixel;
		const unsigned int num_rays;
		unsigned int active_rays;
		unsigned int shadow_rays_num;
		
		// internal cumulative image buffer
		glm::dvec3* img_buffer_cumulative;

		// internal per frame image buffer
		glm::dvec3* img_buffer_frame;

		// number of cumulative iterations for an unchanged scene
		size_t iterations;

		Scene* scene;
		Camera* camera;

		Ray* normal_rays;
		Ray* shadow_rays;

		double elapsed_time;

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

		// pack the active rays to contiguous array
		void packRays();

		// initialize the values before the first intersection
		void setupCameraRays();

		void gatherEmittance();

		// initialize the values of shadow rays
		// return the number of shadow rays that will be casted
		void setupShadowRays();

		void russianRoullete(int depth);

		void setupSecondaryRays();

		// 
		void shade();

		// tone map by dividing with max luma
		void toneMap();

		// called at the end of the render procedure to update the window buffer
		void updateWindowBuffer(glm::dvec3*& window_buffer);
	};
}