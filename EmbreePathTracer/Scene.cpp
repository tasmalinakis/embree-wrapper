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
		for (int i = 0; i < lights.size(); i++)
		{
			delete lights[i];
		}
		lights.clear();
	}

	Mesh* Scene::createMesh(float* world_transform)
	{
		Mesh* mesh = new Mesh(world_transform);
		meshes.push_back(mesh);
		return mesh;
	}

	Light* Scene::createLight(Triangle* tr, Material* material)
	{
		Light* light = new Light(tr, material);
		lights.push_back(light);
		return light;
	}

	Material* Scene::getMaterial(const std::string mat_name)
	{
		Material* mat = mesh_materials[mat_name];
		if (mat == NULL)
		{
			mat = new Material(mesh_materials.size() - 1, mat_name);
			mesh_materials[mat_name] = mat;
		}
		return mat;
	}

	void Scene::commitScene()
	{
		for (Mesh* mesh : meshes)
		{
			mesh->_fixReferences();
		}
		for (Light* light : lights)
		{
			light->calcArea();
		}
	}
}