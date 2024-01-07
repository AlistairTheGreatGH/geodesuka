#include <geodesuka/core/object/render_target.h>

#include <geodesuka/core/stage_t.h>

#include <assert.h>

namespace geodesuka::core::object {

	render_target::render_target_renderer::render_target_renderer(gcl::context* aContext, window* aWindow, render_target* aRenderTarget) {
		// This a custom renderer for the render target class for when what has 
		// been drawn to it, needs to be referenced in canvas.h. (i.e. any form of compositing)


	}

	render_target::frame::frame(int aAttachmentCount) {
		for (size_t i = 0; i < aAttachmentCount; i++) {
			this->Image.emplace_back();
			this->Attachment.push_back(VK_NULL_HANDLE);
		}
	}

	render_target::~render_target() {}

	// Does a simple iteration to the next available frame.
	VkResult render_target::next_frame() {
		this->FrameReadIndex = this->FrameDrawIndex;
		this->FrameDrawIndex = ((this->FrameDrawIndex == (this->Frame.size() - 1)) ? 0 : (this->FrameDrawIndex + 1));
		return VK_SUCCESS;
		// VkSubmitInfo SubmitInfo{};
		// SubmitInfo.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// SubmitInfo.pNext					= NULL;
		// SubmitInfo.waitSemaphoreCount		= 0;
		// SubmitInfo.pWaitSemaphores			= NULL;
		// SubmitInfo.pWaitDstStageMask		= NULL;
		// SubmitInfo.commandBufferCount		= 1;
		// SubmitInfo.pCommandBuffers			= &Frame[FrameDrawIndex].ClearScreen;
		// SubmitInfo.signalSemaphoreCount		= 0;
		// SubmitInfo.pSignalSemaphores		= NULL;
		// return SubmitInfo;
	}

	// Is left empty, because it is not used for anything besides system_window.
	VkPresentInfoKHR render_target::present_frame() {
		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType 					= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.pNext 					= NULL;
		PresentInfo.waitSemaphoreCount 		= 0;
		PresentInfo.pWaitSemaphores 		= NULL;
		PresentInfo.swapchainCount 			= 0;
		PresentInfo.pSwapchains 			= NULL;
		PresentInfo.pImageIndices 			= NULL;
		PresentInfo.pResults 				= NULL;
		return PresentInfo;
	}

	render_target::render_target(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount) :
		object_t(aContext, aStage, aName)
	{
		CommandPool 			= new gcl::command_pool(aContext, gcl::device::operation::GRAPHICS_AND_COMPUTE);
		FrameRate 				= aFrameRate;
		FrameRateTimer 			= 1.0 / aFrameRate;
		FrameResolution 		= aFrameResolution;
		FrameReadIndex			= 0;
		FrameDrawIndex			= 0;
		Frame 					= std::vector<frame>(aFrameCount);
		for (frame& Frm : Frame) {
			Frm = frame(aAttachmentCount);
		}

		DefaultViewport.x					= 0.0f;
		DefaultViewport.y					= 0.0f;
		DefaultViewport.width				= this->FrameResolution.x;
		DefaultViewport.height				= this->FrameResolution.y;
		DefaultViewport.minDepth			= 0.0f;// +1.0f;
		DefaultViewport.maxDepth			= 1.0f;// -1.0f;
		RenderArea.offset.x					= 0;
		RenderArea.offset.y					= 0;
		RenderArea.extent.width				= this->FrameResolution.x;
		RenderArea.extent.height			= this->FrameResolution.y;

	}

}
