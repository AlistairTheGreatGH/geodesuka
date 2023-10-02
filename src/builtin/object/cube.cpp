#include <geodesuka/builtin/object/cube.h>

#include <geodesuka/engine.h>

using namespace geodesuka::core;
using namespace object;
using namespace hid;
using namespace gcl;
using namespace util;

namespace geodesuka::builtin::object {

	cube::cube(engine* aEngine, core::gcl::context* aContext, core::stage::scene3d* aScene3D, const char* aName) : object_t(aContext, aScene3D, aName) {
		// Compile time vertex data example.
		float VertexData[] = {
			-1.0, -1.0, -1.0,  0.0,  0.0,  0.0,	 // 0
			 1.0, -1.0, -1.0,  1.0,  0.0,  0.0,	 // 1
			 1.0,  1.0, -1.0,  0.0,  1.0,  0.0,	 // 2
			-1.0,  1.0, -1.0,  1.0,  1.0,  0.0,	 // 3
			-1.0, -1.0,  1.0,  0.0,  0.0,  1.0,	 // 4
			 1.0, -1.0,  1.0,  1.0,  0.0,  1.0,	 // 5
			 1.0,  1.0,  1.0,  0.0,  1.0,  1.0,	 // 6
			-1.0,  1.0,  1.0,  1.0,  1.0,  1.0	 // 7
		};

		// Compile time index data.
		short IndexData[] = {
			0, 3, 1, // -z
			2, 1, 3, // -z
			2, 6, 1, // +x
			5, 1, 6, // +x
			3, 7, 2, // +y
			6, 2, 7, // +y
			1, 5, 0, // -y
			4, 0, 5, // -y
			0, 4, 3, // -x
			7, 3, 4, // -x
			5, 6, 4, // +z
			7, 4, 6  // +z
		};

		//this->Model = graphics::model(this->Context, "assets/model/Dragon 2.5_dae.dae");


	}

	cube::~cube() {

	}

}
