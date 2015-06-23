#include "Mesh.h"

namespace path_tracer
{
	Mesh::~Mesh()
	{
		for (int i = 0; i < mesh.prim_data.size(); i++)
		{
			delete mesh.prim_data[i];
		}

		mesh.prim_data.clear();
		mesh.vertex_data.clear();
	}

	Triangle* Mesh::addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2, const glm::vec3 color)
	{
		
		size_t length = mesh.vertex_data.size();
		size_t prim_id = mesh.prim_data.size();

		mesh.vertex_data.push_back(v0);
		mesh.vertex_data.push_back(v1);
		mesh.vertex_data.push_back(v2);

		Triangle* tr = new Triangle(prim_id, color);

		mesh.prim_data.push_back(tr);
		return tr;
	}

	Triangle* Mesh::addTriangle(const glm::vec3 v0, const glm::vec3 v1, const glm::vec3 v2)
	{
		return addTriangle(v0, v1, v2, glm::vec3(0));
	}

	float* Mesh::getVertexArray()
	{
		return &mesh.vertex_data[0][0];
	}

	void** Mesh::getPrimData()
	{
		void** p = reinterpret_cast<void**> (mesh.prim_data.data());
		return p;
		
	}

	void Mesh::printTriangles()
	{
		for (int i = 0; i < mesh.vertex_data.size(); i++)
		{
			printf("%d %f,%f,%f\n", i, mesh.vertex_data[i].x, mesh.vertex_data[i].y, mesh.vertex_data[i].z);
		}
		printf("************");
		_fixReferences();
		for (int i = 0; i < mesh.prim_data.size(); i++)
		{
			printf("%d %f,%f,%f\n", i, mesh.prim_data[i]->v0->x, mesh.prim_data[i]->v0->y, mesh.prim_data[i]->v0->z);
		}
	}

	void Mesh::_fixReferences()
	{
		size_t index = 0;
		for (int i = 0; i < mesh.prim_data.size(); i++)
		{
			mesh.prim_data[i]->v0 = (mesh.vertex_data.data() + index++);
			mesh.prim_data[i]->v1 = (mesh.vertex_data.data() + index++);
			mesh.prim_data[i]->v2 = (mesh.vertex_data.data() + index++);
			mesh.prim_data[i]->calcNormal();
		}
	}
}