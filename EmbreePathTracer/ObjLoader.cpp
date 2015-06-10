#include "ObjLoader.h"

namespace path_tracer
{
	Scene* loadObj(Scene* scene, std::string input_file)
	{
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string err = tinyobj::LoadObj(shapes, materials, input_file.c_str());
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
			Material* material = mesh->getMaterial(mat.name);

			material->setReflectivityAmbient(float3ToVec3(mat.ambient));
			material->setReflectivityDiffuse(float3ToVec3(mat.diffuse));
			material->setReflectivitySpecular(float3ToVec3(mat.specular));
			material->setRefractionIndex(mat.ior);
			material->setSpecularExponent(mat.shininess);
			material->setTransmissionFilter(float3ToVec3(mat.transmittance));

		}

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
				tr->setMaterial(mesh->getMaterial(mat_name));
			}
		}

		return NULL;
	}
}