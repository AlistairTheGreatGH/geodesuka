#include <geodesuka/builtin/stage/demo3d.h>

#include <geodesuka/engine.h>

using namespace geodesuka::core::object;

namespace geodesuka::builtin::stage {

	demo3d::demo3d(core::gcl::context* aContext) : scene3d(aContext) {
		// this->Camera		|= new camera3d(Context, this, "Camera3D", math::vec3<float>(0.0f, 0.0f, 0.0f), math::vec2<int>(1280, 720), 60.0, 3);
		// this->Object		|= new object::triangle(Context, this, "Triangle");
	}

	demo3d::~demo3d() {

	}

	render_target::render_info demo3d::render() {
		render_target::render_info RenderInfo{};

		return RenderInfo;
	}

}
