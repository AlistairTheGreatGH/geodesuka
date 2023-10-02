#include <geodesuka/core/stage/scene2d.h>

namespace geodesuka::core::stage {

	scene2d::scene2d(gcl::context* aContext) : stage_t(aContext->parent_engine(), aContext) {

	}

	scene2d::~scene2d() {

	}

	int scene2d::id() {
		return ID;
	}

	//stage_t::batch scene2d::render() {
	//	batch DrawBatch;
	//	return DrawBatch;
	//}

}