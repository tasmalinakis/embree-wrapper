#include "ObjLoader.h"

namespace path_tracer
{
	void ObjLoader::loadObj()
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err = tinyobj::LoadObj(shapes, materials, obj_file.c_str());
		if (!err.empty()) {
			fprintf(stderr, "%s\n", err);
			exit(1);
		}

		printf("# of shapes: %d\n", shapes.size());
		printf("# of materials: %d\n", materials.size());

		// create a new mesh
		Mesh* mesh = scene->createMesh(NULL);

		// save the materials of the mesh
		for (size_t i = 0; i < materials.size(); i++)
		{
			tinyobj::material_t mat = materials[i];
			Material* material = scene->getMaterial(mat.name);

			material->setReflectivityAmbient(float3ToVec3(mat.ambient));
			material->setReflectivityDiffuse(float3ToVec3(mat.diffuse));
			material->setReflectivitySpecular(float3ToVec3(mat.specular));
			material->setRefractionIndex(mat.ior);
			material->setSpecularExponent(mat.shininess);
			material->setTransmissionFilter(float3ToVec3(mat.transmittance));

		}

		// parse the lights file to prepare the set for
		// querying the lights.
		parseLightsFile();

		// iterate through shapes
		for (size_t i = 0; i < shapes.size(); i++)
		{
			tinyobj::shape_t cur_shape = shapes[i];
			// iterate through triangles of each shape
			for (size_t j = 0; j < cur_shape.mesh.indices.size();)
			{
				// new triangle positions
				glm::vec3 tr_pos[3];
				
				for (unsigned short tr = 0; tr < 3; tr++)
				{
					float* base = &cur_shape.mesh.positions[cur_shape.mesh.indices[j++] * 3];
					tr_pos[tr].x = base[0];
					tr_pos[tr].y = base[1];
					tr_pos[tr].z = base[2];
				}

				Triangle* tr = mesh->addTriangle(tr_pos[0], tr_pos[1], tr_pos[2]);

				int mat_id = cur_shape.mesh.material_ids[(j - 1) / 3];
				std::string mat_name = materials[mat_id].name;
				tr->setMaterial(scene->getMaterial(mat_name));

				// check if the material is light material
				if (scene_light_mats.count(mat_name) != 0)
				{
					scene->createLight(tr, scene->getMaterial(mat_name));
				}
			}
		}
	}

	void ObjLoader::parseLightsFile()
	{
		FILE* f_lights = fopen(lights_file.c_str(), "r");
		if (f_lights == NULL)
		{
			fprintf(stderr, "ERROR: Lights file not found.\n");
			exit(1);
		}
		char* mat_name = new char[100];
		float r_r, r_g, r_b;
		while (fscanf(f_lights, "%s %f %f %f\n", mat_name, &r_r, &r_g, &r_b) != EOF)
		{
			std::string s_mat_name(mat_name);

			// check if the material is ambient
			if (s_mat_name.compare("ambient") == 0)
			{
				scene->setAmbientLight(glm::vec3(r_r, r_g, r_b));
				continue;
			}

			// check if there is a material with that name
			if (!scene->materialExists(s_mat_name))
			{
				fprintf(stderr, "WARNING: light mat name %s not found.\n", mat_name);
				continue;
			}

			// add to lights index
			scene_light_mats.insert(s_mat_name);
			scene->getMaterial(s_mat_name)->setRadiance(glm::vec3(r_r, r_g, r_b));

			printf("%s %f %f %f\n", mat_name, r_r, r_g, r_b);
		}
		delete mat_name;
		fclose(f_lights);
	}
}