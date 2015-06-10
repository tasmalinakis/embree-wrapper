#pragma once
#include "intersector_commons.h"

#ifndef ISECT_EMBREE_API
#	if defined(COMPILING_DLL)
#		define ISECT_EMBREE_API __declspec(dllexport)
#	else
#		define ISECT_EMBREE_API __declspec(dllimport)
#	endif
#endif
namespace intersector
{

	ISECT_EMBREE_API int isectCreateIntersector(size_t ray_bunch_size_hint = 4096);
	// Upon creation time, internal buffers of rays can be allocated once and reallocated only if a larger
	// batch is requested. ray_bunch_size_hint is used for the first buffer allocation.
	// The id of the intersector (zero based is returned, or -1 if a problem occurs. All other functions use the
	// id of the target intersector.

	ISECT_EMBREE_API isect_error_t isectDestroyIntersector(int iid);

	ISECT_EMBREE_API isect_error_t isectAddTriangles(int iid, float *vertex_data, void **prim_ptr,
		float *world_transform, size_t num_triangles);
	// Pass a triangle group to the iid intersector. The triangle buffer (vertex_data) contains tuples of 9 floats,
	// each representing the coordinates of a triangle in the form x1,y1,z1,x2,y2,z2,x3,y3,z,3. An array
	// of num_triangles pointers (prim_ptr) is also passed. Each pointer contains the address of an external
	// triangle structure. This is useful when additional data must be retrieved after the intersection test,
	// in order to perform shading etc. Furthermore, it provides an indication in the isectTraceRays function
	// about whether a ray hit a triangle or not. If no such triangle structure exists, pass an array of
	// arbitrary non-zero values (can be the same). world_transform is an optional array of 16 floats containing
	// the 4X4 transformation matrix applied to the polygon group. Can be NULL, in which case it signifies an
	// identity matrix (no transformation).

	ISECT_EMBREE_API isect_error_t isectResetTriangles(int iid);
	// Clear all triangles in the iid intersector

	ISECT_EMBREE_API isect_error_t isectTraceRays(int iid, float *origin_data, float *point_at_data,
		float * intersection_data, void ** hit_primitive_ptr,
		size_t num_rays, isect_ray_type_t r_type, float max_range = 0.0f);
	// Intersect a batch of rays with the geometry in the iid intersector. The origin_data array contains the x,y,z
	// coordinates of the ray origins. point_at_data array contains either normalized directions of the rays cast OR
	// destination points, depending on whether ray_type_t is RAY_TYPE_NORMAL or RAY_TYPE_SHADOW, respectively.
	// intersection_data is an array of the corresponding hit points, each in the form x,y,z. If a ray does not hit
	// a triangle, the triplet values are undefined. hit_primitive_ptr contains the address or the passed id of the hit
	// primitive, as provided in the prim_ptr argument of isectAddTriangles(). If a ray hits nothing, the corresponding
	// entry is NULL.
	// max_range is an acceleration hint that tells the intersector to look for intersections up to max_range distance
	// fro mthe origin. 0 means infinite distance.

}
