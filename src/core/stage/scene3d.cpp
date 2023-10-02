#include <geodesuka/core/stage/scene3d.h>

namespace geodesuka::core::stage {

	scene3d::scene3d(gcl::context* aContext) : stage_t(aContext->parent_engine(), aContext) {

	}

	scene3d::~scene3d() {

	}

	int scene3d::id() {
		return ID;
	}

	//stage_t::batch scene3d::render() {
	//	batch DrawBatch;
	//	return DrawBatch;
	//}

}