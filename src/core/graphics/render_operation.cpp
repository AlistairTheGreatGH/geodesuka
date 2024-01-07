#include <geodesuka/core/graphics/render_operation.h>

namespace geodesuka::core::graphics {

	render_operation::~render_operation() {

	}

	std::vector<VkCommandBuffer> render_operation::operator[](int aIndex) {
		return this->DrawCommand[aIndex];
	}

	render_operation::render_operation() {
		this->Context				= nullptr;
	}


}
