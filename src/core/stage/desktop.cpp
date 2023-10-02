#include <geodesuka/core/stage/desktop.h>

namespace geodesuka::core::stage {

	desktop::~desktop() {

	}

	int desktop::id() {
		return ID;
	}

	desktop::desktop(engine* aEngine, gcl::device* aParentDevice, object::system_display* aParentDisplay) : stage_t(aEngine, nullptr) {
		this->Device		= aParentDevice;
		this->Object		|= aParentDisplay;
		//this->RenderTarget	|= aParentDisplay;
	}

	desktop::desktop(engine* aEngine, object::system_display* aParentDisplay) : stage_t(aEngine, nullptr) {
		this->Device		= nullptr;
		this->Object		|= aParentDisplay;
		//this->RenderTarget	|= aParentDisplay;
	}

}
