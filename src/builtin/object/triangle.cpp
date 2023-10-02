#include <geodesuka/builtin/object/triangle.h>

#include <geodesuka/engine.h>

#include <assert.h>

using namespace geodesuka::core;
using namespace object;
using namespace hid;
using namespace gcl;
using namespace util;

namespace geodesuka::builtin::object {

	triangle::triangle(core::gcl::context* aContext, core::stage_t* aStage, const char* aName) : object_t(aContext, aStage, aName) {

		//this->VertexShader = shader(aContext, "assets/shader/camera3d.vert");
		//this->PixelShader = shader(aContext, "assets/shader/camera3d.frag");


		// Load file into host memory.
		//graphics::model* HostModel = (graphics::model*)Engine->File.open("dep/assimp/test/models/BLEND/CubeHierarchy_248.blend");
		graphics::model* HostModel = (graphics::model*)Engine->File.open("dep/assimp/test/models/FBX/animation_with_skeleton.fbx");
		//graphics::model* HostModel = (graphics::model*)Engine->File.open("dep/assimp/test/models/FBX/huesitos.fbx");

		//graphics::model* HostModel = new graphics::model("huesitos.fbx");

		// Load into device context memory.
		//this->Model = graphics::model(Context, HostModel);

		State = READY;
	}

	triangle::~triangle() {
		State = DESTRUCTION;
		// Delete Drawpack
		//delete PixelShader;
		//delete VertexShader;
		//delete VertexBuffer;
	}

}
