#pragma once
#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>
#include <iostream>
#include <vector>
#include "intersector_commons.h"

namespace intersector
{

	// "infinite" distance
	#define inf float(0x7F800000)

	// simulates different intersectors. Embree allows only one core initialization, so we will use different scenes to resemble different intersectors
	struct Scene
	{
		int iid;
		bool is_valid;
		RTCScene rtc_scene;
		std::vector<void**> prim_data;
		RTCRay* rays;
		size_t rays_num;
		bool is_committed = false;
	};
	
	// used during the creation of the triangles to map the ids of the vertices used for each triangle.
	struct TriangleEdge
	{
		int v0;
		int v1;
		int v2;
	};


	// the Vertex structure has to be an aligned structure of 16 bytes as per the embree documentation
	// hence the use of Vec3fa instead of Vec3f. The 4th float value of the struct is arbitrary.
	typedef struct Vec3fa Vertex;

	// initializes the embree Ray Tracking Core
	void _initEmbree();

	// exits the embree Ray Tracking Core
	void _destroyEmbree();

	// gets the embree error of the current thread and translates it to our custom representation
	isect_error_t _getAndTranslateError();

	// helper function to check iids
	isect_error_t _checkIid(const int& iid);

	// helper function to commit a RTCScene
	isect_error_t _commitScene(Scene& scene);

	// creates a scene and returns its iid
	int _createIntersector(const size_t ray_bunch_size_hint);

	// destroys the given scene and returns an error enum
	isect_error_t _destroyIntersector(const int iid);

	// add the given triangles to the scene with the iid
	isect_error_t _addTriangles(const int iid, const float *vertex_data, void **prim_ptr,
		const float *world_transform, const size_t num_triangles);

	// Clear all triangles in the iid intersector
	isect_error_t _resetTriangles(const int iid);

	// trace the requested rays
	isect_error_t _traceRays(const int iid, const float *origin_data, const float *point_at_data,
		float * intersection_data, void ** hit_primitive_ptr,
		const size_t num_rays, const isect_ray_type_t r_type, float max_range);

	// trace a single ray. Called in parallel by the _traceRays function
	void _traceSingleRay(const Scene& scene, RTCRay& ray, const float* origin_data, const float* point_at_data,
		float* intersection_data, void*& hit_primitive_ptr, const isect_ray_type_t& r_type, const float max_range, isect_error_t& error);
}