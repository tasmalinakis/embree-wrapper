#include "PathTracer.h"
#include <string>

using namespace intersector;

namespace path_tracer
{
	void PathTracer::init()
	{
		normal_ray_info = new RayInfo();
		shadow_ray_info = new RayInfo();

		size_t num_rays_3 = 3 * num_rays;

		normal_ray_info->origin_data = new float[num_rays_3];
		normal_ray_info->point_at_data = new float[num_rays_3];
		normal_ray_info->intersection_data = new float[num_rays_3];
		normal_ray_info->hit_primitive_ptr = new void*[num_rays];

		shadow_ray_info->origin_data = new float[num_rays_3];
		shadow_ray_info->point_at_data = new float[num_rays_3];
		shadow_ray_info->intersection_data = new float[num_rays_3]; 
		shadow_ray_info->hit_primitive_ptr = new void*[num_rays];

		normal_rays = new Ray[num_rays];
		shadow_rays = new Ray[num_rays];

		scene = new Scene();
		//camera = new Camera(scr_width, scr_height, glm::vec3(0, 2, -5), glm::vec3(.75, .75, .75));
		camera = new Camera(scr_width, scr_height, glm::vec3(278, 273, -800), glm::vec3(278, 273, 0));

		img_buffer_cumulative = new glm::dvec3[scr_width * scr_height];
		img_buffer_frame = new glm::dvec3[scr_width * scr_height];
	}

	PathTracer::~PathTracer()
	{
		delete normal_ray_info;
		delete shadow_ray_info;
		delete[] normal_rays;
		delete[] shadow_rays;
		delete scene;
		delete camera;
		isectDestroyIntersector(isect_id);
	}

	PathTracer* PathTracer::createPathTracer(const unsigned int scr_width, const unsigned int scr_height, const unsigned int samples_per_pixel)
	{
		// try to create an intersector using the API
		int id = isectCreateIntersector(scr_width * scr_height);
		if (id == -1) return nullptr;

		// if successful return a new path tracer with the given id
		return new PathTracer(id, scr_width, scr_height, samples_per_pixel);
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

		updateWindowBuffer(img_buffer);
	}

	void PathTracer::renderDirect(glm::dvec3 *&img_buffer) 
	{	
		// initial setup of camera rays
		setupCameraRays();

		bool first_cast = true;

		//intersect with scene
		intersector::isectTraceRays(isect_id, normal_ray_info->origin_data, normal_ray_info->point_at_data, normal_ray_info->intersection_data, normal_ray_info->hit_primitive_ptr, active_rays, intersector::RAY_TYPE_NORMAL);

		packRays();

		// if it is the first time that rays are casted, assign shading materials
		if (first_cast)
		{
			assignShadingMats();
			first_cast = false;
		}

		//set up shadow rays
		setupShadowRays();

		/*
		FILE* testFile = fopen("testOcclusion.txt", "w");
		for (int i = 0; i < shadow_rays_num; i++)
		{
			int i_3 = 3 * i;
			fprintf(testFile, "org: %f %f %f dest: %f %f %f\n", shadow_ray_info->origin_data[i_3], shadow_ray_info->origin_data[i_3 + 1], shadow_ray_info->origin_data[i_3 + 2],
				shadow_ray_info->point_at_data[i_3], shadow_ray_info->point_at_data[i_3 + 1], shadow_ray_info->point_at_data[i_3 + 2]);
		}
		fclose(testFile);
		*/
		// check for occlusions
		intersector::isectTraceRays(isect_id, shadow_ray_info->origin_data, shadow_ray_info->point_at_data, shadow_ray_info->intersection_data, shadow_ray_info->hit_primitive_ptr, shadow_rays_num, intersector::RAY_TYPE_SHADOW);

		shade();

		updateWindowBuffer(img_buffer);
		
	}

	void PathTracer::assignShadingMats()
	{
		for (unsigned int i = 0; i < active_rays; i++)
		{
			normal_rays[i].mat_shaded = Mesh::getTriangleFromVoidPtr(*normal_rays[i].hit_primitive_ptr)->getMaterial();
		}
	}

	void PathTracer::setupCameraRays()
	{
		glm::vec3 cx = camera->getCx();
		glm::vec3 cy = camera->getCy();
		glm::vec3 cam_pos = camera->getPos();
		glm::vec3 cam_dir = camera->getDirection();

		float sx = 0;
		float sy = 0;
		float* cur_origin_data = normal_ray_info->origin_data;
		float* cur_point_at_data = normal_ray_info->point_at_data;
		float* cur_intersection_data = normal_ray_info->intersection_data;
		Ray* cur_ray = normal_rays;

		// prepare for first cast
		for (int i = 0; i < num_rays; i++)
		{
			// set relationships between Ray struct and RayInfo struct
			// origin data
			normal_rays[i].origin_data = cur_origin_data;
			cur_origin_data += 3;

			// point at data
			normal_rays[i].point_at_data = cur_point_at_data;
			cur_point_at_data += 3;

			// intersection data
			normal_rays[i].intersection_data = cur_intersection_data;
			cur_intersection_data += 3;

			// hit prim data
			normal_rays[i].hit_primitive_ptr = &normal_ray_info->hit_primitive_ptr[i];

			// set ray to pixel relationship
			normal_rays[i].pixel_contributing = &img_buffer_frame[i];

			// set ray strength
			normal_rays[i].ray_strength = glm::dvec3(1.0);
		}

		// set active rays count
		active_rays = num_rays;

		cur_origin_data = normal_ray_info->origin_data;
		cur_point_at_data = normal_ray_info->point_at_data;

		// setup camera rays
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

				cur_ray->previous_direction_in = -d; 
				
				cur_ray++;
			}
		}
	}

	void PathTracer::setupShadowRays()
	{
		shadow_rays_num = 0;
		float* cur_origin_data = shadow_ray_info->origin_data;
		float* cur_point_at_data = shadow_ray_info->point_at_data;
		float* cur_intersection_data = shadow_ray_info->intersection_data;
		for (unsigned int i = 0; i < active_rays; i++)
		{
			Ray normal_ray = normal_rays[i];

			// check if the material is perfectly specular and don't spawn shadow ray in that case
			Triangle* tr = Mesh::getTriangleFromVoidPtr(*normal_ray.hit_primitive_ptr);
			if (tr->getMaterial()->getReflectivitySpecular() == glm::vec3(1.0, 1.0, 1.0)) continue;

			Ray* shadow_ray = &shadow_rays[shadow_rays_num];

			// set origin at the hit point of the normal ray
			shadow_ray->origin_data = cur_origin_data;
			cur_origin_data += 3;
			glm::dvec3 normal_offset = 0.0001f * tr->normal;
			shadow_ray->origin_data[0] = normal_ray.intersection_data[0] + normal_offset.x;
			shadow_ray->origin_data[1] = normal_ray.intersection_data[1] + normal_offset.y;
			shadow_ray->origin_data[2] = normal_ray.intersection_data[2] + normal_offset.z;

			// set intersection data
			shadow_ray->intersection_data = cur_intersection_data;
			cur_intersection_data += 3;

			// set hit prim data
			shadow_ray->hit_primitive_ptr = &shadow_ray_info->hit_primitive_ptr[shadow_rays_num];

			// set the contributing pixel
			shadow_ray->pixel_contributing = normal_ray.pixel_contributing;

			// set the ray strength
			shadow_ray->ray_strength = normal_ray.ray_strength;

			// set the shading material
			shadow_ray->mat_shaded = normal_ray.mat_shaded;

			// set the destination
			shadow_ray->point_at_data = cur_point_at_data;
			cur_point_at_data += 3;

			// get the lights of the scene
			size_t lights_num;
			Light** lights = scene->getLights(lights_num);

			// select a random light
			float ra = randomNum();
			int rand_light = ra * lights_num;
			Light* light = lights[rand_light];
			
			// get a random point in the light
			glm::dvec3 point = light->samplePoint();

			// set the point at destination
			shadow_ray->point_at_data[0] = point.x;
			shadow_ray->point_at_data[1] = point.y;
			shadow_ray->point_at_data[2] = point.z;

			// set the pdf to 1 / (lightsNum * SelectedLightArea)
			shadow_ray->pdf = 1.0 / (double)(lights_num * light->getTriangleArea());

			shadow_rays_num++;
		}
	}

	void PathTracer::updateWindowBuffer(glm::dvec3*& window_buffer)
	{
		/*
		glm::dvec3* cur_pixel = img_buffer_cumulative;
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
		*/
		for (unsigned int i = 0; i < scr_width * scr_height; i++)
		{

			img_buffer_cumulative[i] += glm::clamp(img_buffer_frame[i] * 255.0, 0.0, 255.0);
			img_buffer_frame[i].x = 0.0;
			img_buffer_frame[i].y = 0.0;
			img_buffer_frame[i].z = 0.0;
		}

		iterations++;
		glm::dvec3* cur_pixel = img_buffer_cumulative;
		glm::dvec3* cur_pixel_window = window_buffer;

		for (int i = 0; i < scr_width * scr_height; i++)
		{
			if (cur_pixel->x > 255)
			{
				int x = 2;
			}
			*cur_pixel_window = *cur_pixel;
			*cur_pixel_window /= iterations;
			cur_pixel_window++;
			cur_pixel++;
		}
	}

	void PathTracer::packRays()
	{
		unsigned int inactive_rays = 0;
		unsigned int packed_index = 0;
		for (unsigned int i = 0; i < active_rays; i++)
		{
			Ray cur_ray = normal_rays[i];

			// if the ray has not hit anything return ambient color
			if (*cur_ray.hit_primitive_ptr == NULL)
			{
				*cur_ray.pixel_contributing += scene->ambient_light * cur_ray.ray_strength;
				inactive_rays++;
			}
			else // if something was hit, swap the current with the last inactive ray in the tables. There is no need to conserve the data of the inactive ray.
			{
				// get the inactive ray in the last packed index
				Ray* inactive_ray = &normal_rays[packed_index];

				// update the origin data
				inactive_ray->origin_data[0] = cur_ray.origin_data[0];
				inactive_ray->origin_data[1] = cur_ray.origin_data[1];
				inactive_ray->origin_data[2] = cur_ray.origin_data[2];

				// update the point at data
				inactive_ray->point_at_data[0] = cur_ray.point_at_data[0];
				inactive_ray->point_at_data[1] = cur_ray.point_at_data[1];
				inactive_ray->point_at_data[2] = cur_ray.point_at_data[2];

				// update the intersection data
				inactive_ray->intersection_data[0] = cur_ray.intersection_data[0];
				inactive_ray->intersection_data[1] = cur_ray.intersection_data[1];
				inactive_ray->intersection_data[2] = cur_ray.intersection_data[2];

				// update the hit primitive ptr data
				*inactive_ray->hit_primitive_ptr = *cur_ray.hit_primitive_ptr;

				// update the normal rays table
				inactive_ray->pixel_contributing = cur_ray.pixel_contributing;
				inactive_ray->pdf = cur_ray.pdf;
				inactive_ray->previous_direction_in = cur_ray.previous_direction_in;
				inactive_ray->ray_strength = cur_ray.ray_strength;
				inactive_ray->mat_shaded = cur_ray.mat_shaded;

				// increment the packed index
				packed_index++;
			}
		}
		active_rays -= inactive_rays;
		fprintf(stderr, "Active Rays: %d\n", active_rays);
	}

	void PathTracer::shade()
	{
		for (unsigned int i = 0; i < shadow_rays_num; i++)
		{
			Ray shadow_ray = shadow_rays[i];
			if (*shadow_ray.hit_primitive_ptr == NULL)
			{
				*shadow_ray.pixel_contributing += shadow_ray.mat_shaded->getReflectivityDiffuse() * glm::vec3(10.0) / (float)shadow_ray.pdf;
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