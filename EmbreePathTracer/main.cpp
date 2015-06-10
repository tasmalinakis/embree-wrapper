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