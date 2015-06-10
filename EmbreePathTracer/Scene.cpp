#include "Scene.h"

namespace path_tracer
{
	Scene::~Scene()
	{
		resetScene();
	}

	void Scene::resetScene()
	{
		for (int i = 0; i < meshes.size(); i++)
		{
			delete meshes[i];
		}
		meshes.clear();
	}

	Mesh* Scene::createMesh(float* world_transform)
	{
		Mesh* mesh = new Mesh(world_transform);
		meshes.push_back(mesh);
		return mesh;
	}
}