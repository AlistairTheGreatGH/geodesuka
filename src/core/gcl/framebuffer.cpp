#include <geodesuka/core/gcl/framebuffer.h>

namespace geodesuka::core::gcl {

	framebuffer::framebuffer() {

	}

	framebuffer::framebuffer(context* aContext, renderpass& aRenderPass, uint32_t aAttachmentCount, image* aAttachment, uint32_t aWidth, uint32_t aHeight, uint32_t aLayers) {
		VkResult Result = VkResult::VK_SUCCESS;

		this->Context = aContext;
		this->AttachmentCount = aAttachmentCount;
		this->Attachment = aAttachment;
		this->View = (VkImageView*)malloc(this->AttachmentCount * sizeof(VkImageView));

		//// Generate Image Views for framebuffer.
		//for (uint32_t i = 0; i < this->AttachmentCount; i++) {
		//	this->View[i] = this->Attachment[i].view();
		//}

		this->CreateInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		this->CreateInfo.pNext				= NULL;
		this->CreateInfo.flags				= 0;
		this->CreateInfo.renderPass			= aRenderPass.handle();
		this->CreateInfo.attachmentCount	= aAttachmentCount;
		this->CreateInfo.pAttachments		= this->View;
		this->CreateInfo.width				= aWidth;
		this->CreateInfo.height				= aHeight;
		this->CreateInfo.layers				= aLayers;

		Result = vkCreateFramebuffer(this->Context->handle(), &this->CreateInfo, NULL, &this->Handle);

	}

	framebuffer::~framebuffer() {
		if (this->Context != nullptr) {
			if (this->Handle != VK_NULL_HANDLE) {
				vkDestroyFramebuffer(this->Context->handle(), this->Handle, NULL);
				this->Handle = VK_NULL_HANDLE;
			}
			if (this->View != NULL) {
				for (uint32_t i = 0; i < this->AttachmentCount; i++) {
					if (this->Handle != VK_NULL_HANDLE) {
						vkDestroyImageView(this->Context->handle(), this->View[i], NULL);
						this->View[i] = VK_NULL_HANDLE;
					}
				}
			}
		}
		free(this->View);
		this->View = NULL;
		this->Attachment = nullptr;
		this->AttachmentCount = 0;
	}

}