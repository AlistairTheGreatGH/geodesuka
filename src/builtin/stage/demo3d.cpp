#include <geodesuka/builtin/stage/demo3d.h>

#include <geodesuka/builtin/object/triangle.h>
#include <geodesuka/builtin/object/quad.h>

namespace geodesuka::builtin::stage {

	using namespace core::object;


	demo3d::demo3d(core::gcl::context* aContext) : scene3d(aContext) {
		//this->RenderTarget		|= new camera3d(aEngine, aContext, this, "Camera3D", 3, 60.0, math::vec3<float>(0, 0, 0), math::vec2<int>(640, 480));

		//this->Object			|= new object::triangle(Engine, aContext, this, "Triangle");
	}

	demo3d::~demo3d() {

	}

}
