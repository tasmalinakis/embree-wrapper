#include <Intersector.h>
#include <cstdio>
#include "Window.h"
#include "PathTracer.h"
#include "Scene.h"
#include "ObjLoader.h"
using namespace intersector;
using namespace path_tracer;

int main(int argc, char *argv[])
{
	PathTracer* pt = PathTracer::createPathTracer(640, 480);
	Scene* scene = pt->getScene();
	ObjLoader obj_loader(scene, "cornell_box.obj", "cornell_box.lgts");
	obj_loader.loadObj();
	obj_loader.cleanUp();
	Window* win = Window::createWindow("oeo");
	
	win->setIntersector(pt);

	pt->commitScene();
	win->startRender();
	delete win;
	delete pt;
	return 0;
}