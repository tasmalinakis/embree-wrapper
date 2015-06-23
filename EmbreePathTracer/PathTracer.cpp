#include "PathTracer.h"
#include <string>

using namespace intersector;

namespace path_tracer
{
	void PathTracer::init()
	{
		normal_ray_info = new RayInfo();
		shadow_ray_info = new RayInfo();

		size_t num_rays = scr_height * scr_width;
		size_t num_rays_3 = 3 * num_rays;

		normal_ray_info->origin_data = new float[num_rays_3]; // FIX * Samples
		normal_ray_info->point_at_data = new float[num_rays_3];
		normal_ray_info->intersection_data = new float[num_rays_3];
		normal_ray_info->hit_primitive_ptr = new void*[num_rays];

		shadow_ray_info->origin_data = new float[num_rays_3];
		shadow_ray_info->point_at_data = new float[num_rays_3];
		shadow_ray_info->intersection_data = new float[1]; // dummy
		shadow_ray_info->hit_primitive_ptr = new void*[num_rays];

		scene = new Scene();
		//camera = new Camera(scr_width, scr_height, glm::vec3(0, 2, -5), glm::vec3(.75, .75, .75));
		camera = new Camera(scr_width, scr_height, glm::vec3(278, 273, -800), glm::vec3(278, 273, 0));
	}

	PathTracer::~PathTracer()
	{
		delete normal_ray_info;
		delete shadow_ray_info;
		delete scene;
		delete camera;
		isectDestroyIntersector(isect_id);
	}

	PathTracer* PathTracer::createPathTracer(const unsigned int scr_width, const unsigned int scr_height)
	{
		// try to create an intersector using the API
		int id = isectCreateIntersector(scr_width * scr_height);
		if (id == -1) return nullptr;

		// if successful return a new path tracer with the given id
		return new PathTracer(id, scr_width, scr_height);
	}

	void PathTracer::renderTest(glm::dvec3*& img_buffer)
	{
		//intersector::isectTraceRays(isect_id, normal_ray_info.origin_data, normal_ray_info.point_at_data, normal_ray_info.intersection_data, normal_ray_info.hit_primitive_ptr, scr_height * scr_width, intersector::RAY_TYPE_NORMAL);
		glm::dvec3* cur_pixel = &img_buffer[0];
		int counter = 0;
		for (int y = 0; y < scr_height; y++)
		{
			for (int x = 0; x < scr_width; x++)
			{
				if (counter == 30) counter = 0;
				int r = 0; int g = 0; int b = 0;
				if (counter <= 10) r = 255;
				else if (counter <= 20) g = 255;
				else if (counter <= 30) b = 255;
				*cur_pixel = glm::vec3(r, g, b);
				cur_pixel++;
			}
			counter++;
		}
	}

	void PathTracer::render(glm::dvec3 *&img_buffer)
	{

		glm::vec3 cx = camera->getCx();
		glm::vec3 cy = camera->getCy();
		glm::vec3 cam_pos = camera->getPos();
		glm::vec3 cam_dir = camera->getDirection();

		float sx = 0;
		float sy = 0;
		float* cur_origin_data = normal_ray_info->origin_data;
		float* cur_point_at_data = normal_ray_info->point_at_data;

		for (int col = 0; col < scr_height; col++)
		{
			for (int row = 0; row < scr_width; row++)
			{
				
				double r1 = 2 * randomNum(); double dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
				double r2 = 2 * randomNum(), dy = r2<1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

				glm::vec3 d = cx* (float)(((sx + .5 + dx) / 2 + row) / scr_width - .5) + cy*(float)(((sy + .5 + dy) / 2 + col) / scr_height - .5) + cam_dir;
				d = glm::normalize(d);

				*cur_origin_data = cam_pos.x; cur_origin_data++;
				*cur_origin_data = cam_pos.y; cur_origin_data++;
				*cur_origin_data = cam_pos.z; cur_origin_data++;

				*cur_point_at_data = d.x; cur_point_at_data++;
				*cur_point_at_data = d.y; cur_point_at_data++;
				*cur_point_at_data = d.z; cur_point_at_data++;
			}
		}

		intersector::isectTraceRays(isect_id, normal_ray_info->origin_data, normal_ray_info->point_at_data, normal_ray_info->intersection_data, normal_ray_info->hit_primitive_ptr, scr_height * scr_width, intersector::RAY_TYPE_NORMAL);

		glm::dvec3* cur_pixel = &img_buffer[0];
		void** cur_hit_ptr = normal_ray_info->hit_primitive_ptr;
		for (int col = 0; col < scr_height; col++)
		{
			for (int row = 0; row < scr_width; row++)
			{
				void* cur_hit = *cur_hit_ptr;
				if (cur_hit != NULL)
				{
					Triangle* tr = Mesh::getTriangleFromVoidPtr(cur_hit);
					glm::vec3 color = tr->getMaterial()->getReflectivityDiffuse();
					//printf("%s\n", tr->getMaterial()->getName().c_str());
					cur_pixel->x += color.x * 255.0f;
					//assert(cur_pixel->x > 0);
					cur_pixel->y += color.y * 255.0f;
					cur_pixel->z += color.z * 255.0f;
				}
				cur_hit_ptr++;
				cur_pixel++;
			}
		}
	}

	void PathTracer::commitScene()
	{
		scene->commitScene();
		Mesh* cur_mesh;
		isect_error_t err;
		for (int i = 0; i < scene->meshes.size(); i++)
		{
			cur_mesh = scene->meshes[i];
			err = isectAddTriangles(isect_id, cur_mesh->getVertexArray(), cur_mesh->getPrimData(), cur_mesh->getWorldTransform(), cur_mesh->getTrianglesCount());
			if (err != ERROR_NONE) fprintf(stderr, "%s\n", err);
		}
	}
}