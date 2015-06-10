#include <Intersector.h>
#include <cstdio>
#include "Window.h"
#include "PathTracer.h"
#include "Mesh.h"
#include "Scene.h"
#include "ObjLoader.h"
using namespace intersector;
using namespace path_tracer;

int main(int argc, char *argv[])
{
	PathTracer* pt = PathTracer::createPathTracer(640, 480);
	Scene* scene = pt->getScene();
	loadObj(scene, "models\\cornell_box.obj");
	//Mesh* meshA = scene->createMesh();
	//Mesh* meshB = scene->createMesh();
	//meshA->addTriangle(glm::vec3(0.0, 1.0, 0.5), glm::vec3(0.75, 0.75, 0.75), glm::vec3(1.0, 2.0, 3.0), glm::vec3(255, 0, 255));
	//meshA->addTriangle(glm::vec3(2.0, 1.0, 0.5), glm::vec3(0.75, 0.75, 0.75), glm::vec3(1.0, 2.0, 3.0), glm::vec3(255, 255, 0));
	//meshA->printTriangles();
	//void** data = meshA->getPrimData();
	//data = data + 1;
	//void* dat = data[0];
	//Triangle* tr = Mesh::getTriangleFromVoidPtr(dat);
	//std::vector<Triangle*>* tr = static_cast<std::vector<Triangle*>*> (*data);
	//std::vector<Triangle*> tr(tr_buf, tr_buf);
	//Triangle** tr_buf = (Triangle**)ddata;
	//printf("%#010x %#010x %#010x\n",data, ddata, meshA->mesh.prim_data.data());
	//
	//void* ddata = data + 1;
	//Triangle* tr = meshA->getTriangleFromVoidPtr(ddata);
	//tr_buf[1]->v0->x = 5.0f;
	//tr->v0->x = 5.0f;
	//float* vert = meshA->getVertexArray();
	//vert[2] = 100.0;
	//meshA->printTriangles();
	Window* win = Window::createWindow("oeo");
	
	win->setIntersector(pt);
	
	//scene->addMesh(meshA);
	//scene->addMesh(meshB);
	pt->commitScene();
	win->startRender();
	delete win;
	delete pt;
	return 0;
}