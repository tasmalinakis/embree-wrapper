#include "embree_wrapper.h"
#include <tbb\parallel_for.h>
//#include "common.h"

namespace intersector
{
	// flag indicating if the core has been initialized
	bool core_initialized;

	// the scenes / intersectors requested
	std::vector<Scene> scenes_requested;

	isect_error_t _getAndTranslateError()
	{
		
		RTCError error = rtcGetError();
		switch (error)
		{
		case RTC_NO_ERROR: return intersector::ERROR_NONE;
		case RTC_UNKNOWN_ERROR: return intersector::ERROR_INTERNAL_ERROR;
		case RTC_INVALID_ARGUMENT: return intersector::ERROR_BAD_ARGUMENT;
		case RTC_INVALID_OPERATION: return intersector::ERROR_INTERNAL_ERROR;
		case RTC_OUT_OF_MEMORY: return intersector::ERROR_INTERNAL_ERROR;
		case RTC_UNSUPPORTED_CPU: return intersector::ERROR_INTERSECTOR_UNAVAILABLE;
		default: return intersector::ERROR_INTERNAL_ERROR;
		}
	}

	// initialize the embree core. This must be done only once
	void _initEmbree()
	{
		if (core_initialized) return;
		rtcInit();
		RTCError err = rtcGetError();
		if (err != RTC_NO_ERROR)
		{
			std::cout << err << std::endl;
			std::abort();
		}
		core_initialized = true;
	}

	// exit the embree core and release all the data
	void destroyEmbree()
	{
		for (Scene& scene : scenes_requested)
		{
			if (scene.is_valid)
			{
				rtcDeleteScene(scene.rtc_scene);
				scene.is_valid = false;
				delete[] scene.rays;
				scene.prim_data.clear();
			}
		}
		rtcExit();
		core_initialized = false;
	}

	// creates a scene and returns its iid
	int _createIntersector(size_t ray_bunch_size_hint)
	{
		if (!core_initialized)
		{
			_initEmbree();
		}

		// our custom scene structure
		Scene scene;
		
		// create the RTCScene
		RTCScene rtc_scene = rtcNewScene(RTC_SCENE_HIGH_QUALITY, RTC_INTERSECT1);

		// if an error occurs return -1
		if (_getAndTranslateError() != ERROR_NONE) return -1;

		// initialize rays array
		RTCRay* rays = new RTCRay[ray_bunch_size_hint];

		// save to the struct and the vector
		scene.iid = scenes_requested.size();
		scene.is_valid = true;
		scene.rays_num = ray_bunch_size_hint;
		scene.rays = rays;
		scene.rtc_scene = rtc_scene;

		scenes_requested.push_back(scene);

		return scene.iid;
	}

	// helper function to check iids
	isect_error_t _checkIid(const int& iid)
	{
		if (!core_initialized)
		{
			return ERROR_INTERSECTOR_UNAVAILABLE;
		}

		// check if the argument is in bounds
		if (iid < 0 || iid >= scenes_requested.size())
		{
			return ERROR_BAD_ARGUMENT;
		}

		// get the requested scene
		Scene* scene = &scenes_requested[iid];

		// check if the scene is valid or it has already been deleted
		if (!scene->is_valid)
		{
			return ERROR_INTERSECTOR_UNAVAILABLE;
		}

		return ERROR_NONE;
	}

	// destroys the given scene and returns an error enum. Tiny memory leak (no removal from vector) to preserve iid uniqueness
	isect_error_t _destroyIntersector(int iid)
	{
		// check for iid error
		isect_error_t iid_error = _checkIid(iid);
		if (iid_error != ERROR_NONE) return iid_error;

		// get the requested scene
		Scene scene = scenes_requested[iid];

		// call the embree api to destroy the scene
		rtcDeleteScene(scene.rtc_scene);

		// free memory from rays and set is_valid to false
		delete[] scene.rays;
		scene.is_valid = false;

		// return embree error
		return _getAndTranslateError();
	}

	// Clear all triangles in the iid intersector
	isect_error_t _resetTriangles(int iid)
	{
		isect_error_t iid_error = _checkIid(iid);
		if (iid_error != ERROR_NONE) return iid_error;

		Scene scene = scenes_requested[iid];

		// call the embree api to reset the geometry
		scene.is_valid = false;
		scene.prim_data.clear();
		rtcDeleteScene(scene.rtc_scene);
		scene.rtc_scene = rtcNewScene(RTC_SCENE_STATIC, RTC_INTERSECT1);
		scene.is_valid = true;
		
		// return embree error
		return _getAndTranslateError();
	}

	// used to parse the world transform in the add triangles function
	// and return an AffineSpace3f
	// we only care for the first 12 floats
	// a[0] a[1] a[2]  a[3]
	// a[4] a[5] a[6]  a[7]
	// a[8] a[9] a[10] a[11]
	// dc   dc   dc    dc

	inline glm::mat4 __parseTransformation(const float* world_transform)
	{
		glm::vec4 vx(world_transform[0], world_transform[4], world_transform[8], world_transform[12]);
		glm::vec4 vy(world_transform[1], world_transform[5], world_transform[9], world_transform[13]);
		glm::vec4 vz(world_transform[2], world_transform[6], world_transform[10], world_transform[14]);
		glm::vec4 vw(world_transform[3], world_transform[7], world_transform[11], world_transform[15]);
		glm::mat4 xfm_matrix(vx, vy, vz, vw);
		return xfm_matrix;
	}

	/*
	inline AffineSpace3f __parseTransformation(const float* world_transform)
	{
		Vec3f vx = makeVec3f(world_transform[0], world_transform[4], world_transform[8]);
		Vec3f vy = makeVec3f(world_transform[1], world_transform[5], world_transform[9]);
		Vec3f vz = makeVec3f(world_transform[2], world_transform[6], world_transform[10]);
		Vec3f vw = makeVec3f(world_transform[3], world_transform[7], world_transform[11]);
		glm::mat4 transformation_matrix(1.0);
		
		return makeAffineSpace3f(vx, vy, vz, vw);
	}
	*/

	// add the given triangles to the scene with the iid
	isect_error_t _addTriangles(const int iid, const float *vertex_data, void **prim_ptr,
		const float *world_transform, const size_t num_triangles)
	{
		 /*****************
		 *	Boring checks *
		 ******************/

		// check iid
		isect_error_t error = _checkIid(iid);
		if (error != ERROR_NONE) return error;

		// check the rest arguments
		if (vertex_data == NULL || num_triangles <= 0) return ERROR_BAD_ARGUMENT;

		/********************
		* Interesting stuff *
		*********************/

		// get the requested scene
		RTCScene scene = scenes_requested[iid].rtc_scene;

		// get the transformation array if it exists
		//AffineSpace3f xfm;
		glm::mat4 xfm_matrix(0.0);
		if (world_transform != NULL) xfm_matrix = __parseTransformation(world_transform);

		// create mesh. 3 vertices and 3 edges for each triangle
		unsigned int mesh_id = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, num_triangles, 3 * num_triangles);

		// create the vertex buffer
		Vertex* vertices = (Vertex*)rtcMapBuffer(scene, mesh_id, RTC_VERTEX_BUFFER);

		// create the triangle edge buffer
		TriangleEdge* triangle_edges = (TriangleEdge*)rtcMapBuffer(scene, mesh_id, RTC_INDEX_BUFFER);

		// temporary vertex to apply transformation before mapping it
		Vertex temp_vertex;

		// vertex index, used for edges;
		int edge_index = 0;

		for (size_t i = 0; i < num_triangles; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				// create each vertex
				//temp_vertex = makeVec3fa(*(vertex_data), *(vertex_data + 1), *(vertex_data + 2));
				temp_vertex = glm::vec4(*(vertex_data), *(vertex_data + 1), *(vertex_data + 2), 0.0);
				vertex_data += 3;

				// apply world transform if any
				if (world_transform != NULL) temp_vertex = temp_vertex * xfm_matrix;

				// map the vertices
				vertices[3 * i + j] = temp_vertex;

				std::cout << vertices[3 * i + j].x << " " << vertices[3 * i + j].y << " " << vertices[3 * i + j].z << std::endl;

			}

			// map the edges
			triangle_edges[i].v0 = edge_index++;
			triangle_edges[i].v1 = edge_index++;
			triangle_edges[i].v2 = edge_index++;

		}

		// unmap the buffers 
		rtcUnmapBuffer(scene, mesh_id, RTC_VERTEX_BUFFER);
		rtcUnmapBuffer(scene, mesh_id, RTC_INDEX_BUFFER);

		// save the prim information for future use
		// note: Thread unsafe. The index in the vector must be the same as the mesh_id. Maybe use map later
		scenes_requested[iid].prim_data.push_back(prim_ptr);

		return _getAndTranslateError();
	}

	// helper function to commit scene
	isect_error_t _commitScene(Scene& scene)
	{
		rtcCommit(scene.rtc_scene);
		isect_error_t error = _getAndTranslateError();
		if (error == ERROR_NONE) scene.is_committed = true;
		return error;
	}

	// trace the requested rays
	// ASSUME that the out arrays are initialized by the caller and have the correct lengths.
	isect_error_t _traceRays(const int iid, const float *origin_data, const float *point_at_data,
		float * intersection_data, void ** hit_primitive_ptr,
		const size_t num_rays, const isect_ray_type_t r_type, float max_range)
	{

		/*****************
		*  Boring checks *
		******************/

		// iid check
		isect_error_t error = _checkIid(iid);
		if (error != ERROR_NONE) return error;

		// rest arguments
		if (origin_data == NULL || num_rays < 0 || max_range < 0) return ERROR_BAD_ARGUMENT;

		/**************
		* Preparation *
		***************/

		// get the scene
		Scene* scene = &scenes_requested[iid];

		// check if the scene has committed its geometry. If not then commit it and return if an error occurs.
		if (scene->is_committed == false)
		{
			error = _commitScene(*scene);
			if (error != ERROR_NONE) return error;
		}

		// if the requested rays excede the current ones reallocate memory for the rays
		if (scene->rays_num < num_rays)
		{
			delete[] scene->rays;
			scene->rays = new RTCRay[num_rays];
			scene->rays_num = num_rays;
		}

		// if max_range is 0 set it to inf
		if (max_range == 0.0) max_range = inf;

		/****************
		* Parallel call *
		*****************/

		// use tbb as suggested by Embree documentation
		// set the parameters for each ray before calling the parallel subroutine
		int collisions = 0;
		tbb::parallel_for(tbb::blocked_range<size_t>(0, num_rays), [&](const tbb::blocked_range<size_t>& r)
		{
			for (size_t i = r.begin(); i != r.end(); ++i)
			{

				// set the parameters for each ray here
				isect_error_t trace_error;
				size_t offset3 = 3 * i; // compute once
				_traceSingleRay(*scene, scene->rays[i], origin_data + offset3, point_at_data + offset3, &intersection_data[offset3],
					hit_primitive_ptr[i], r_type, max_range, trace_error);
				//fprintf(stderr, "%s\n", trace_error);
			}
		});

		for (size_t i = 0; i < num_rays; i++)
		{
			if (hit_primitive_ptr[i] != NULL && r_type == intersector::RAY_TYPE_NORMAL) collisions++;
			else if (hit_primitive_ptr[i] == NULL && r_type == intersector::RAY_TYPE_SHADOW) collisions++;
		}

		if (r_type == RAY_TYPE_NORMAL)
			fprintf(stderr, "Collisions: %d\n", collisions);
		else if (r_type == RAY_TYPE_SHADOW)
			fprintf(stderr, "No Collisions: %d out of %d\n", collisions, num_rays);
		return ERROR_NONE;
	}

	// trace a single ray. Called in parallel by the _traceRays function
	void _traceSingleRay(const Scene& scene, RTCRay& ray, const float* origin_data, const float* point_at_data,
		float* intersection_data, void*& hit_primitive_ptr, const isect_ray_type_t& r_type, const float max_range, isect_error_t& error)
	{

		// set the ray origin
		ray.org[0] = origin_data[0];
		ray.org[1] = origin_data[1];
		ray.org[2] = origin_data[2];

		// indicates no mask
		ray.mask = -1;

		// set start time
		ray.time = 0;

		// set tnear to a very small number to avoid intersection with self
		ray.tnear = 0.001f;

		if (r_type == RAY_TYPE_NORMAL)
		{
			// set the ray range
			ray.tfar = max_range;

			// preset to invalid geometry
			ray.geomID = RTC_INVALID_GEOMETRY_ID;
			ray.primID = RTC_INVALID_GEOMETRY_ID;

			// set the direction
			ray.dir[0] = point_at_data[0];
			ray.dir[1] = point_at_data[1];
			ray.dir[2] = point_at_data[2];

			// intersect with scene
			rtcIntersect(scene.rtc_scene, ray);

			// check if something was hit and set the pointers
			// if it hasn't hit anything
			if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
			{
				hit_primitive_ptr = NULL;
			}
			else
			{
				// set collision coords
				intersection_data[0] = ray.org[0] + ray.dir[0] * ray.tfar;
				intersection_data[1] = ray.org[1] + ray.dir[1] * ray.tfar;
				intersection_data[2] = ray.org[2] + ray.dir[2] * ray.tfar;

				// set prim ptr data
				hit_primitive_ptr = scene.prim_data[ray.geomID][ray.primID];
			}
		}
		else // shadow ray
		{

			// create origin vector to calculate direction
			glm::dvec3 v_org = glm::dvec3(origin_data[0], origin_data[1], origin_data[2]);
			//Vec3fa v_org = makeVec3fa(origin_data[0], origin_data[1], origin_data[2]);

			// create destination vector to calculate direction
			glm::dvec3 v_dest = glm::dvec3(point_at_data[0], point_at_data[1], point_at_data[2]);
			//Vec3fa v_dest = makeVec3fa(point_at_data[0], point_at_data[1], point_at_data[2]);

			// calculate direction vector
			glm::dvec3 v_dir = glm::normalize(v_dest - v_org);
			//Vec3fa v_dir = normalize(v_dest - v_org);

			// set the length of the shadow ray
			//float v_length = glm::length(v_dest - v_org);
			//float g_length = glm::length(g_dest - g_org);
			ray.tfar = glm::length(v_dest - v_org) - 0.0001f;

			// set the direction
			ray.dir[0] = v_dir.x;
			ray.dir[1] = v_dir.y;
			ray.dir[2] = v_dir.z;

			// set the geomid to an arbitrary value
			ray.geomID = RTC_INVALID_GEOMETRY_ID;

			// occlude with scene
			rtcOccluded(scene.rtc_scene, ray);

			// if no occlusion occurs set hit_primitive_ptr to null
			if (ray.geomID == RTC_INVALID_GEOMETRY_ID)
			{
				// flags to point occlusion
				hit_primitive_ptr = NULL;
				*intersection_data = 1.0;
			}
			else
			{
				hit_primitive_ptr = (void*)1;
				*intersection_data = 0.0;
			}
		}
		
		// set the error
		error = _getAndTranslateError();
	}

}