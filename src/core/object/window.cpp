#include <geodesuka/engine.h>
#include <geodesuka/core/object/window.h>

// Standard C Library.
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>

#include <geodesuka/core/math.h>

//#include "../hid/mouse.h"
//#include "../hid/keyboard.h"
//#include "../hid/joystick.h"

#include <geodesuka/core/object_t.h>

namespace geodesuka::core::object {

	window::setting::setting() {
		Resizable		= GLFW_TRUE;
		Decorated		= GLFW_TRUE;
		UserFocused		= GLFW_TRUE;
		AutoMinimize	= GLFW_TRUE;
		Floating		= GLFW_FALSE;
		Maximized		= GLFW_FALSE;
		Minimized		= GLFW_FALSE;
		Visible			= GLFW_TRUE;
		ScaleToMonitor	= GLFW_FALSE;
		CenterCursor	= GLFW_TRUE;
		FocusOnShow		= GLFW_TRUE;
		Fullscreen		= GLFW_FALSE;

		Hovered			= GLFW_TRUE;
	}

	window::~window() {

	}

	void window::set_title(util::string aTitle) {

	}

	void window::set_size(math::vec2<float> aSize) {
		this->Size = aSize;
	}

	math::vec2<float> window::get_size() {
		return this->Size;
	}

	void window::set_resolution(math::vec2<uint> aResolution) {
		this->FrameResolution = math::vec3<uint>(aResolution.x, aResolution.y, 1u);
	}

	math::vec2<uint> window::get_resolution() {
		return math::vec2<uint>(this->FrameResolution.x, this->FrameResolution.y);
	}

	bool window::should_close() {
		return false;
	}

	std::vector<VkSubmitInfo> window::render(stage::canvas* aStage) {
		std::vector<gcl::command_list>		CommandList;
		int FDI = FrameDrawIndex;

		for (object_t* Obj : aStage->Object.Handle) {
			Obj->draw(this);
		}
		
		Frame[FDI].RenderingInfo.insert(Frame[FDI].RenderingInfo.end(), CommandList.begin(), CommandList.end());
		return convert(Frame[FDI].RenderingInfo);
	}

	window::window(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount) :
		render_target(aContext, aStage, aName, aFrameResolution, aFrameRate, aFrameCount, aAttachmentCount)
	{
		VkResult Result = VK_SUCCESS;
		this->Title			= aName;
		this->Size			= math::vec2<float>(0.0f, 0.0f);
		this->Setting		= {};
	}

	// This is called after a system_window or a virtual_window has acquired their images and filled out their frame structures.
	// system/virtual window must define attachment descriptions
	VkResult window::create_renderer() {
		VkResult Result = VK_SUCCESS;
		VkAttachmentReference ColorAttachmentReference{};
		VkSubpassDescription SubpassDescription{};
		VkSubpassDependency SubpassDependency{};
		VkRenderPassCreateInfo RPCI{};

		// 1. Create Render Pass
		ColorAttachmentReference.attachment				= 0;
		ColorAttachmentReference.layout					= (VkImageLayout)gcl::image::layout::COLOR_ATTACHMENT_OPTIMAL;

		SubpassDescription.flags						= 0;
		SubpassDescription.pipelineBindPoint			= VK_PIPELINE_BIND_POINT_GRAPHICS;
		SubpassDescription.inputAttachmentCount			= 0;
		SubpassDescription.pInputAttachments			= NULL;
		SubpassDescription.colorAttachmentCount			= 1;
		SubpassDescription.pColorAttachments			= &ColorAttachmentReference;
		SubpassDescription.pResolveAttachments			= NULL;
		SubpassDescription.pDepthStencilAttachment		= NULL;
		SubpassDescription.preserveAttachmentCount		= 0;
		SubpassDescription.pPreserveAttachments			= NULL;

		SubpassDependency.srcSubpass					= VK_SUBPASS_EXTERNAL;
		SubpassDependency.dstSubpass					= 0;
		SubpassDependency.srcStageMask					= gcl::pipeline::stage::TOP_OF_PIPE;
		SubpassDependency.dstStageMask					= gcl::pipeline::stage::BOTTOM_OF_PIPE;
		SubpassDependency.srcAccessMask					= gcl::device::access::MEMORY_WRITE;
		SubpassDependency.dstAccessMask					= gcl::device::access::MEMORY_READ;
		SubpassDependency.dependencyFlags				= 0;

		RPCI.sType										= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RPCI.pNext										= NULL;
		RPCI.flags										= 0;
		RPCI.attachmentCount							= AttachmentDescription.size();
		RPCI.pAttachments								= AttachmentDescription.data();
		RPCI.subpassCount								= 1;
		RPCI.pSubpasses									= &SubpassDescription;
		RPCI.dependencyCount							= 1;
		RPCI.pDependencies								= &SubpassDependency;

		Result = vkCreateRenderPass(Context->handle(), &RPCI, NULL, &RenderPass);

		// 2. Create Framebuffers
		for (render_target::frame& Frm : Frame) {
			VkFramebufferCreateInfo FCI{};
			FCI.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FCI.pNext				= NULL;
			FCI.flags				= 0;
			FCI.renderPass			= RenderPass;
			FCI.attachmentCount		= Frm.Attachment.size();
			FCI.pAttachments		= Frm.Attachment.data(); 
			FCI.width				= FrameResolution.x;
			FCI.height				= FrameResolution.y;
			FCI.layers				= 1;
			Result = vkCreateFramebuffer(Context->handle(), &FCI, NULL, &Frm.Buffer);
		}

		return Result;
	}

}