#pragma once
namespace intersector
{
	enum isect_ray_type_t {
		RAY_TYPE_NORMAL,
		RAY_TYPE_SHADOW
	};

	enum isect_error_t {
		ERROR_NONE = 0,
		ERROR_BAD_ARGUMENT,
		ERROR_INTERNAL_ERROR,
		ERROR_BAD_OPERATION,
		ERROR_INTERSECTOR_UNAVAILABLE
	};
}