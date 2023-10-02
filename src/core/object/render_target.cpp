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
		this->Buffer = VK_NULL_HANDLE;
	}

	render_target::~render_target() {

	}

	bool render_target::refresh_signal() {
		return false;
	}

	bool render_target::render_signal() {
		return FrameRateTimer.check_and_reset();
	}

	// This is an example method of rendering a stage. Can be overriden.
	std::vector<VkSubmitInfo> render_target::render(stage_t* aStage) {
		std::vector<VkSubmitInfo> RenderBatch;
		this->Mutex.lock();

		// Prepares next frame for rendering operation by render target.
		this->next_frame();

		// Use next_frame semaphore to pause render operations until
		//RenderBatch += this->draw(aStage->Object);

		// Use Submission Semaphore to hold present.
		//RenderBatch += this->present_frame();

		this->Mutex.unlock();
		return RenderBatch;
	}

	// Does a simple iteration to the next available frame.
	VkResult render_target::next_frame() {
		this->FrameReadIndex = this->FrameDrawIndex;
		this->FrameDrawIndex = ((this->FrameDrawIndex == (this->Frame.size() - 1)) ? 0 : (this->FrameDrawIndex + 1));
		return VK_SUCCESS;
	}

	std::vector<gcl::command_list> render_target::draw(const std::vector<object_t*>& aObject) {
		std::vector<gcl::command_list> DrawCommand(aObject.size());
		for (size_t i = 0; i < aObject.size(); i++) {
			DrawCommand[i] = aObject[i]->draw(this);
		}
		return DrawCommand;
	}

	// Is left empty, because it is not used for anything besides system_window.
	VkPresentInfoKHR render_target::present_frame() {
		VkPresentInfoKHR PresentInfo{};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		PresentInfo.pNext = NULL;
		PresentInfo.waitSemaphoreCount = 0;
		PresentInfo.pWaitSemaphores = NULL;
		PresentInfo.swapchainCount = 0;
		PresentInfo.pSwapchains = NULL;
		PresentInfo.pImageIndices = NULL;
		PresentInfo.pResults = NULL;
		return PresentInfo;
	}

	uint32_t render_target::descriptor_set_count() const {
		uint32_t DescriptorSetCount = 0;
		for (size_t i = 0; i < this->Pipeline.size(); i++) {
			DescriptorSetCount += this->Pipeline[i].DescriptorSetLayoutCount;
		}
		return DescriptorSetCount;
	}

	std::vector<VkDescriptorPoolSize> render_target::descriptor_pool_sizes() const {
		std::vector<VkDescriptorPoolSize> DescriptorPoolSize;
		for (size_t i = 0; i < this->Pipeline.size(); i++) {
			std::vector<VkDescriptorPoolSize> PoolSize = this->Pipeline[i].descriptor_pool_size();
			for (size_t j = 0; j < PoolSize.size(); j++) {
				// Check if PoolSize[j] exits in
				bool AlreadyExistsInSet = false;
				for (size_t k = 0; k < DescriptorPoolSize.size(); k++) {
					AlreadyExistsInSet |= (PoolSize[j].type == DescriptorPoolSize[k].type);
				}

				if (AlreadyExistsInSet) {
					// Already Exists, add count
					for (size_t k = 0; k < DescriptorPoolSize.size(); k++) {
						if (PoolSize[j].type == DescriptorPoolSize[k].type) {
							DescriptorPoolSize[k].descriptorCount += PoolSize[j].descriptorCount;
						}
					}
				}
				else {
					// Does not exist in set, add new type.
					VkDescriptorPoolSize NewPoolSize;
					NewPoolSize.descriptorCount = PoolSize[j].descriptorCount;
					NewPoolSize.type = PoolSize[j].type;
					DescriptorPoolSize.push_back(NewPoolSize);
				}
			}
		}
		return DescriptorPoolSize;
	}

	render_target::render_target(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount) :
		object_t(aContext, aStage, aName),
		CommandPool(aContext, 0, gcl::device::operation::GRAPHICS_AND_COMPUTE)
	{
		this->FrameRate						= aFrameRate;
		this->FrameResolution				= aFrameResolution;
		this->AttachmentDescription.resize(aAttachmentCount);
		this->FrameReadIndex				= 0;
		this->FrameDrawIndex				= 0;
		for (size_t i = 0; i < aFrameCount; i++) {
			this->Frame.emplace_back(aAttachmentCount);
		}
		this->FrameRateTimer.set(1.0 / aFrameRate);
		this->RenderPass 						= VK_NULL_HANDLE;

		// Viewport
		VkViewport	DefaultViewport;
		VkRect2D DefaultScissor;

		this->DefaultViewport.x						= 0.0f;
		this->DefaultViewport.y						= 0.0f;
		this->DefaultViewport.width					= this->FrameResolution.x;
		this->DefaultViewport.height				= this->FrameResolution.y;
		this->DefaultViewport.minDepth				= 0.0f;// +1.0f;
		this->DefaultViewport.maxDepth				= 1.0f;// -1.0f;
		this->DefaultScissor.offset.x				= 0;
		this->DefaultScissor.offset.y				= 0;
		this->DefaultScissor.extent.width			= this->FrameResolution.x;
		this->DefaultScissor.extent.height			= this->FrameResolution.y;
		this->RenderArea.offset.x					= 0;
		this->RenderArea.offset.y					= 0;
		this->RenderArea.extent.width				= this->FrameResolution.x;
		this->RenderArea.extent.height				= this->FrameResolution.y;
	}

	VkResult render_target::create_framebuffers() {
		VkResult Result = VK_SUCCESS;
		for (int i = 0; i < this->Frame.size(); i++) {
			VkFramebufferCreateInfo FramebufferCreateInfo{};
			FramebufferCreateInfo.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FramebufferCreateInfo.pNext				= NULL;
			FramebufferCreateInfo.flags				= 0;
			FramebufferCreateInfo.renderPass		= this->RenderPass;
			FramebufferCreateInfo.attachmentCount	= this->Frame[i].Attachment.size();
			FramebufferCreateInfo.pAttachments		= this->Frame[i].Attachment.data();
			FramebufferCreateInfo.width				= this->FrameResolution.x;
			FramebufferCreateInfo.height			= this->FrameResolution.y;
			FramebufferCreateInfo.layers			= 1u; //this->FrameResolution.z;
			Result = vkCreateFramebuffer(Context->handle(), &FramebufferCreateInfo, NULL, &Frame[i].Buffer);
		}
		return Result;
	}

}
