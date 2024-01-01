#include <geodesuka/core/object/virtual_window.h>

namespace geodesuka::core::object {

	int virtual_window::id() {
		return ID;
	}

	virtual_window::virtual_window(gcl::context* aContext, stage_t* aStage, const char* aName, const create_info& aCreateInfo, math::vec2<int> aPosition, math::vec2<int> aSize) :
		window(aContext, aStage, aName, math::vec3<uint>(aSize.x, aSize.y, 1u), aCreateInfo.FrameRate, aCreateInfo.FrameCount, 1u)
	{
		VkResult Result = VK_SUCCESS;
		gcl::image::create_info ICI = gcl::image::create_info(
			gcl::image::sample::COUNT_1,
			gcl::image::tiling::OPTIMAL,
			gcl::device::memory::DEVICE_LOCAL,
			gcl::image::usage::COLOR_ATTACHMENT | gcl::image::usage::SAMPLED | gcl::image::usage::TRANSFER_SRC | gcl::image::usage::TRANSFER_DST			
		);

		for (render_target::frame& Frm : Frame) {
			Frm = render_target::frame(1);
			Frm.Image[0] = gcl::image(Context, ICI, aCreateInfo.PixelFormat, aSize.x, aSize.y);
			Frm.Image[0].transition(gcl::image::LAYOUT_UNDEFINED, gcl::image::layout::SHADER_READ_ONLY_OPTIMAL);
		}

		AttachmentDescription[0] = Frame[0].Image[0].desc(gcl::image::layout::SHADER_READ_ONLY_OPTIMAL, gcl::image::layout::SHADER_READ_ONLY_OPTIMAL);

		// Create Renderer
		this->create_renderer();

	}

	virtual_window::~virtual_window() {

	}

}