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
		iterations = 0;

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
				double r2 = 2 * randomNum(), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

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

		//intersect with scene
		intersector::isectTraceRays(isect_id, normal_ray_info->origin_data, normal_ray_info->point_at_data, normal_ray_info->intersection_data, normal_ray_info->hit_primitive_ptr, active_rays, intersector::RAY_TYPE_NORMAL);

		gatherEmittance();

		packRays();

		//set up shadow rays
		setupShadowRays();

		// check for occlusions
		intersector::isectTraceRays(isect_id, shadow_ray_info->origin_data, shadow_ray_info->point_at_data, shadow_ray_info->intersection_data, shadow_ray_info->hit_primitive_ptr, shadow_rays_num, intersector::RAY_TYPE_SHADOW);

		shade();

		updateWindowBuffer(img_buffer);

	}

	void PathTracer::renderGI(glm::dvec3 *&img_buffer)
	{
		// initial setup of camera rays
		setupCameraRays();

		int depth = 0;
		while (active_rays > 0)
		{
			//intersect with scene
			intersector::isectTraceRays(isect_id, normal_ray_info->origin_data, normal_ray_info->point_at_data, normal_ray_info->intersection_data, normal_ray_info->hit_primitive_ptr, active_rays, intersector::RAY_TYPE_NORMAL);

			// make at least 5 iterations
			
			russianRoullete(depth);

			gatherEmittance();

			packRays();

			//set up shadow rays
			setupShadowRays();
			// check for occlusions
			if (shadow_rays_num > 0)
			{
				intersector::isectTraceRays(isect_id, shadow_ray_info->origin_data, shadow_ray_info->point_at_data, shadow_ray_info->intersection_data, shadow_ray_info->hit_primitive_ptr, shadow_rays_num, intersector::RAY_TYPE_SHADOW);
				shade();
			}

			setupSecondaryRays();

			depth++;
		}
		updateWindowBuffer(img_buffer);
	}

	void PathTracer::russianRoullete(int depth)
	{

		if (depth < 3)
		{
			for (unsigned int i = 0; i < active_rays; i++)
			{
				Ray* ray = &normal_rays[i];

				// if rand depth < 5 set scale to 1.
				ray->pdf_inversed = 1.0;
			}
		}
		else
		{
			for (unsigned int i = 0; i < active_rays; i++)
			{
				Ray* ray = &normal_rays[i];
				if (*ray->hit_primitive_ptr == NULL) continue;

				double thresh = 1.0 / (double)depth;

				float rand = randomNum();

				// if rand > thresh set hit_primitive_ptr to null to be picked up by packer
				if (rand > thresh)
				{
					*ray->hit_primitive_ptr = NULL;
				}
				else
				{
					ray->pdf_inversed = (double)depth;
				}
			}
		}
	}

	void PathTracer::setupSecondaryRays()
	{
		for (unsigned int i = 0; i < active_rays; i++)
		{
			Ray* ray = &normal_rays[i];

			// decide what event will occur
			// diffusion for now

			// set origin to current intersection
			glm::vec3 new_origin = float3ToVec3(ray->intersection_data);
			ray->origin_data[0] = ray->intersection_data[0];
			ray->origin_data[1] = ray->intersection_data[1];
			ray->origin_data[2] = ray->intersection_data[2];

			// multiply pdf by selected event pdf
			// FIX
			ray->pdf_inversed *= 1.0;

			// cosine sampling
			// set ray strength to PI * brdf = PI * brdf.diffusion / PI = brdf.diffusion
			Triangle* tr = Mesh::getTriangleFromVoidPtr(*ray->hit_primitive_ptr);
			ray->ray_strength *= tr->getMaterial()->getReflectivityDiffuse();

			/* Adopted from Global Compendium (35) */

			float rand1 = randomNum();
			float rand2 = randomNum();
			/*
			float x = glm::cos(2 * PI * rand1) * glm::sqrt(1 - rand2);
			float y = glm::sin(2 * PI * rand1) * glm::sqrt(1 - rand2);
			float z = glm::sqrt(rand2);
			*/

			float phi = float(2 * PI) * rand1;
			float cosTheta = sqrt(rand2), sinTheta = sqrt(1.0f - rand2);
			glm::vec3 cos_sample(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

			//glm::vec3 cos_sample(x, y, z);

			/* Altered code from Embree Start */

			// make basis for coordinate change to normal
			glm::vec3 dx0 = glm::cross(glm::vec3(1.0, 0.0, 0.0), tr->normal);
			glm::vec3 dx1 = cross(glm::vec3(0.0, 1.0, 0.0), tr->normal);
			glm::vec3 dx = glm::normalize(glm::dot(dx0, dx0) > dot(dx1, dx1) ? dx0 : dx1);
			glm::vec3 dy = glm::normalize(glm::cross(tr->normal, dx));

			glm::mat3 change_mat(dx, dy, tr->normal);
			//change_mat = glm::transpose(change_mat);
			
			// make the coordinate change
			cos_sample = glm::normalize(change_mat * cos_sample);

			/* - Altered code from Embree End */

			// set the direction of the ray
			//cos_sample = glm::normalize(cos_sample - new_origin);
			ray->point_at_data[0] = cos_sample.x;
			ray->point_at_data[1] = cos_sample.y;
			ray->point_at_data[2] = cos_sample.z;

			// set previous direction
			ray->previous_direction_in = -glm::dvec3(ray->point_at_data[0], ray->point_at_data[1], ray->point_at_data[2]);

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

			normal_rays[i].pdf_inversed = 1.0;
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
				double r2 = 2 * randomNum(), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);

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
			if (tr->getMaterial()->getReflectivitySpecular() == glm::dvec3(1.0, 1.0, 1.0)) continue;

			Ray* shadow_ray = &shadow_rays[shadow_rays_num];

			// set origin at the hit point of the normal ray
			shadow_ray->origin_data = cur_origin_data;
			cur_origin_data += 3;
			
			/*
			glm::dvec3 normal_offset = 0.0001f * tr->normal;
			shadow_ray->origin_data[0] = normal_ray.intersection_data[0] + normal_offset.x;
			shadow_ray->origin_data[1] = normal_ray.intersection_data[1] + normal_offset.y;
			shadow_ray->origin_data[2] = normal_ray.intersection_data[2] + normal_offset.z;
			*/

			shadow_ray->origin_data[0] = normal_ray.intersection_data[0];
			shadow_ray->origin_data[1] = normal_ray.intersection_data[1];
			shadow_ray->origin_data[2] = normal_ray.intersection_data[2];

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
			shadow_ray->source_triangle = Mesh::getTriangleFromVoidPtr(*normal_ray.hit_primitive_ptr);

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

			// set the target material
			shadow_ray->target_triangle = light->getTriangle();

			// set the pdf to 1 / (lightsNum * SelectedLightArea)
			shadow_ray->pdf_inversed = normal_ray.pdf_inversed * (double)(lights_num)* light->getTriangleArea();

			//assert(shadow_ray->pdf_inversed < 100000000);

			shadow_rays_num++;
		}
	}

	void PathTracer::gatherEmittance()
	{
		for (unsigned int i = 0; i < active_rays; i++)
		{
			if (*normal_rays[i].hit_primitive_ptr == NULL) continue;
			Triangle* tr = Mesh::getTriangleFromVoidPtr(*normal_rays[i].hit_primitive_ptr);
			if (tr->getMaterial()->getRadiance() == glm::dvec3(0.0)) continue;
			//double cos = glm::dot(tr->normal, normal_rays[i].previous_direction_in);
			glm::dvec3 lambertian_brdf = tr->getMaterial()->getReflectivityDiffuse() / PI;
			*normal_rays[i].pixel_contributing += tr->getMaterial()->getRadiance() * lambertian_brdf * normal_rays[i].ray_strength * normal_rays[i].pdf_inversed;

			// set hit_primitive_ptr to null to be picked up by packer
			*normal_rays[i].hit_primitive_ptr = NULL;
		}
	}

	void PathTracer::updateWindowBuffer(glm::dvec3*& window_buffer)
	{

		toneMap();

		iterations++;


		for (int i = 0; i < scr_width * scr_height; i++)
		{
			img_buffer_cumulative[i] += img_buffer_frame[i];

			window_buffer[i] = img_buffer_cumulative[i];
			window_buffer[i] /= (double)iterations;

			img_buffer_frame[i].x = 0.0;
			img_buffer_frame[i].y = 0.0;
			img_buffer_frame[i].z = 0.0;

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
				//*cur_ray.pixel_contributing += scene->ambient_light * cur_ray.ray_strength;
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
				inactive_ray->pdf_inversed = cur_ray.pdf_inversed;
				inactive_ray->previous_direction_in = cur_ray.previous_direction_in;
				inactive_ray->ray_strength = cur_ray.ray_strength;
				inactive_ray->source_triangle = cur_ray.source_triangle;

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
				Triangle* tr_source = shadow_ray.source_triangle;
				Triangle* tr_dest = shadow_ray.target_triangle;

				glm::vec3 point_source = float3ToVec3(shadow_ray.origin_data);
				glm::vec3 point_dest = float3ToVec3(shadow_ray.point_at_data);
				
				// calculate G factor
				double cos1 = glm::dot(tr_source->normal, glm::normalize(point_dest - point_source));
				double cos2 = glm::dot(tr_dest->normal, glm::normalize(point_source - point_dest));
				double geometry_factor = cos1 * cos2 / pow(glm::length(point_dest - point_source), 2);
				
				if (geometry_factor < 0) geometry_factor = 0; // happens for erratic samples

				// calculate brdf
				glm::dvec3 lambertian_brdf_source = tr_source->getMaterial()->getReflectivityDiffuse() / PI;
				glm::dvec3 lambertian_brdf_target = tr_dest->getMaterial()->getReflectivityDiffuse() / PI;

				*shadow_ray.pixel_contributing += geometry_factor * shadow_ray.target_triangle->getMaterial()->getRadiance() * lambertian_brdf_source * lambertian_brdf_target * shadow_ray.ray_strength * shadow_ray.pdf_inversed;
				//assert(shadow_ray.pixel_contributing->x == 0.0);
			}

			// add ambient light
			//*shadow_ray.pixel_contributing += scene->getAmbientLight() * max(shadow_ray.source_triangle->getMaterial()->getReflectivityAmbient(), 1.0);
		}
	}

	void PathTracer::toneMap()
	{
		
		for (unsigned int i = 0; i < scr_height * scr_width; i++)
		{
			//assert(rays_hit_per_pixel[i] == 0);
			img_buffer_frame[i] = glm::clamp(img_buffer_frame[i] * 255.0, 0.0, 255.0);
		}
		
		/*
		float max_luma = 0.0;
		for (unsigned int i = 0; i < scr_height * scr_width; i++)
		{
			img_buffer_frame[i] *= 255.0;
			glm::dvec3 cur_pixel = img_buffer_frame[i];
			max_luma = fmax(max_luma, fmax(cur_pixel.x, fmax(cur_pixel.y, cur_pixel.z)));
		}

		for (unsigned int i = 0; i < scr_height * scr_width; i++)
		{
			glm::dvec3 cur_pixel = img_buffer_frame[i];
			float max_luma_cur = 0.0;
			max_luma_cur = fmax(max_luma_cur, fmax(cur_pixel.x, fmax(cur_pixel.y, cur_pixel.z)));

			img_buffer_frame[i] = cur_pixel / (1.0 + max_luma_cur / 255.0);
			assert(img_buffer_frame[i].x <= 255.0);
			/*
			img_buffer_frame[i] /= max_luma;
			img_buffer_frame[i] *= 255.0;
			assert(img_buffer_frame[i].x - 255.0 < 0.0001);
			
		}
	*/
		
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