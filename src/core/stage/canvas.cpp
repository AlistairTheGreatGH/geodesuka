#include <geodesuka/core/stage/canvas.h>

#include <geodesuka/core/object/render_target.h>
#include <geodesuka/core/object/window.h>

#include <geodesuka/builtin/object/triangle.h>

namespace geodesuka::core::stage {

	canvas::canvas(gcl::context* aContext, object::window* aWindow) : stage_t(aContext->parent_engine(), aContext) {
		this->Window.push_back(aWindow);
		this->Object |= new builtin::object::triangle(Context, this, "triangle");
		//this->RenderTarget |= aWindow;

	}

	canvas::~canvas() {}

	canvas::canvas(gcl::context* aContext) : stage_t(aContext->parent_engine(), aContext) {

	}

	int canvas::id() {
		return 1;
	}

	object::render_target::render_info canvas::render() {
		object::render_target::render_info SRO;
		for (object::window* Wnd : Window) {
			if (Wnd->FrameRateTimer.check_and_reset()) {
				// Pull Next Frame Image
				VkResult Result = Wnd->next_frame();
				// Render Target Render entire stage.
				std::vector<VkSubmitInfo> StageRender = Wnd->render(this);
				// If system_window, get present info.
				VkPresentInfoKHR PresentFrame = Wnd->present_frame();
				// Append to stage render info.
				//SRO.SubmitInfo.push_back(NextFrame);
				SRO.SubmitInfo.insert(SRO.SubmitInfo.end(), StageRender.begin(), StageRender.end());
				SRO.PresentInfo.push_back(PresentFrame);
			}
		}
		return SRO;
	}

}
