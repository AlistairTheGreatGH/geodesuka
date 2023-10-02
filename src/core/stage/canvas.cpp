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

	canvas::~canvas() {
		// Will be used to clear out all windows.
	}

	canvas::canvas(gcl::context* aContext) : stage_t(aContext->parent_engine(), aContext) {

	}

	int canvas::id() {
		return ID;
	}

	std::vector<VkSubmitInfo> canvas::render() {
		std::vector<VkSubmitInfo> StageRenderOperation;
		for (size_t i = 0; i < this->Window.size(); i++) {
			//StageRenderOperation += this->Window[i]->render(this);
		}
		return StageRenderOperation;
	}

}
