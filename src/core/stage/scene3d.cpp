#include <geodesuka/core/stage/scene3d.h>


namespace geodesuka::core::stage {

	using namespace object;

	scene3d::scene3d(gcl::context* aContext) : stage_t(aContext->parent_engine(), aContext) {

	}

	scene3d::~scene3d() {

	}

	int scene3d::id() {
		return ID;
	}

	render_target::render_info scene3d::render() {
		render_target::render_info RenderInfo{};
		for (auto& Cam : this->Camera) {
			if (Cam->FrameRateTimer.check_and_reset()) {
				Cam->next_frame();

				Cam->present_frame();
			}
		}
		return RenderInfo;
	}

}