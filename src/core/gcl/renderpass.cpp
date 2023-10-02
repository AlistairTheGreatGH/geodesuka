#include <geodesuka/core/gcl/renderpass.h>

namespace geodesuka::core::gcl {

	renderpass::renderpass() {
	}

	renderpass::renderpass(
		context* aContext,
		int aAttachmentCount, image* aAttachment,
		uint32_t aSubpassCount, VkSubpassDescription* aSubpass,
		uint32_t aDependencyCount, VkSubpassDependency* aDependency
	) {

		this->AttachmentDescriptionCount = aAttachmentCount;
		this->AttachmentDescription = (VkAttachmentDescription*)malloc(this->AttachmentDescriptionCount * sizeof(VkAttachmentDescription));
		for (int i = 0; i < aAttachmentCount; i++) {
			//this->AttachmentDescription[i] = aAttachment[i].description();
		}

		this->CreateInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		this->CreateInfo.pNext					= NULL;
		this->CreateInfo.flags					= 0;
		this->CreateInfo.attachmentCount		= this->AttachmentDescriptionCount;
		this->CreateInfo.pAttachments			= this->AttachmentDescription;
		this->CreateInfo.subpassCount			= aSubpassCount;
		this->CreateInfo.pSubpasses				= aSubpass;
		this->CreateInfo.dependencyCount		= aDependencyCount;
		this->CreateInfo.pDependencies			= aDependency;

		VkResult Result = VkResult::VK_SUCCESS;
		if ((this->CreateInfo.pAttachments != NULL) && (this->CreateInfo.pSubpasses != NULL) && (this->CreateInfo.pDependencies != NULL)) {



			Result = vkCreateRenderPass(this->Context->handle(), &this->CreateInfo, NULL, &this->Handle);
		}
		else {

		}

	}

	renderpass::renderpass(
		context* aContext,
		uint32_t aAttachmentCount, VkAttachmentDescription* aAttachment,
		uint32_t aSubpassCount, VkSubpassDescription* aSubpass,
		uint32_t aDependencyCount, VkSubpassDependency* aDependency
	) {

	}

	renderpass::~renderpass() {

	}

	VkRenderPass renderpass::handle() {
		return this->Handle;
	}

}