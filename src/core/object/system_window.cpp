#include <geodesuka/engine.h>
#include <geodesuka/core/object/system_window.h>

#include <string.h>
#include <assert.h>

#include <vector>
#include <algorithm>

#include <GLFW/glfw3.h>

#include <iostream>
//#include <intrin.h>
//__stosb(NULL, 0x00, 1024);

namespace geodesuka::core::object {

	using namespace util;
	using namespace gcl;

	std::vector<const char*> system_window::RequiredInstanceExtension;
	std::vector<const char*> system_window::RequiredContextExtension = { /*VK_KHR_SURFACE_EXTENSION_NAME,*/ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	std::thread::id system_window::MainThreadID;
	std::atomic<bool> system_window::SignalCreate = false;
	std::atomic<bool> system_window::WindowCreated = false;
	system_window::glfwargs system_window::WindowTempData = { window::setting(), 0, 0, NULL, NULL, NULL };
	GLFWwindow* system_window::ReturnWindow = NULL;
	std::atomic<GLFWwindow*> system_window::DestroyWindow = NULL;

	// ------------------------------ public methods ------------------------------- //

	// The way a system_window is created is by first creating a GLFW window, then creating a Vulkan surface from the GLFW window. With the vulkan
	// surface created, a swapchain can be created from the vulkan surface. The swapchain is used to create the framebuffers and render passes.
	
	system_window::system_window(context* aContext, system_display* aDisplay, const char* aName, const create_info& aCreateInfo, math::vec2<int> aPosition, math::vec2<int> aSize) :
		window(aContext, aDisplay->Stage, aName, math::vec3<uint>(aSize.x, aSize.y, 1u), aCreateInfo.Swapchain.FrameRate, aCreateInfo.Swapchain.FrameCount, 1)
	{
		
	}

	system_window::system_window(context* aContext, system_display* aDisplay, const char* aName, const create_info& aCreateInfo, math::vec3<float> aPosition, math::vec2<float> aSize) :
		window(aContext, aDisplay->Stage, aName, math::vec3<uint>(1u, 1u, 1u), aCreateInfo.Swapchain.FrameRate, aCreateInfo.Swapchain.FrameCount, 1) 
	{

	}


	//system_window::system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, math::vec3<float> aPosition, math::vec2<float> aSize) : 
	//	window(aContext, aSystemDisplay->Stage, aName, math::vec3<uint>(1u, 1u, 1u), aProperty.Swapchain.FrameRate, aProperty.Swapchain.FrameCount, 1)
	//{
	//	VkResult Result = VK_SUCCESS;

	//	this->zero_out();

	//	this->Display			= aSystemDisplay;
	//	this->Position			= aPosition;
	//	this->Size				= aSize;
	//	this->PositionVSC		= this->Display->convert_to_vsc_position(aPosition);
	//	this->SizeVSC			= this->Display->convert_to_vsc_size(aSize);

	//	Result = this->create_system_window(aContext, aSystemDisplay, aName, aProperty, this->PositionVSC, this->SizeVSC);

	//	if (Result == VK_SUCCESS) {
	//		this->State = READY;
	//	}
	//}

	//system_window::system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, math::vec2<int> aPosition, math::vec2<int> aResolution) :
	//	window(aContext, aSystemDisplay->Stage, aName, math::vec3<uint>(aResolution.x, aResolution.y, 1u), aProperty.Swapchain.FrameRate, aProperty.Swapchain.FrameCount, 1)
	//{
	//	VkResult Result = VK_SUCCESS;

	//	this->zero_out();

	//	this->Display			= aSystemDisplay;
	//	this->Position			= Display->convert_to_physical_position(aPosition);
	//	this->Size				= Display->convert_to_physical_size(aResolution);
	//	this->PositionVSC		= aPosition;
	//	this->SizeVSC			= aResolution;

	//	Result = this->create_system_window(aContext, aSystemDisplay, aName, aProperty, aPosition, aResolution);

	//	if (Result == VK_SUCCESS) {
	//		this->State = READY;
	//	}
	//}

	system_window::~system_window() {
		this->Mutex.lock();
		this->clear();
		this->Mutex.unlock();
	}

	void system_window::set_position(math::vec3<float> aPosition) {
		Mutex.lock();
		this->Position = aPosition;
		this->PositionVSC = this->Display->convert_to_screen(math::vec2<float>(aPosition.x, aPosition.y));
		math::vec2<int> GlobalPosition = this->Display->convert_to_global_position(this->PositionVSC, this->SizeVSC);
		glfwSetWindowPos(this->Window, GlobalPosition.x, GlobalPosition.y);
		Mutex.unlock();
	}

	int system_window::id() {
		return ID;
	}

	void system_window::set_size(math::vec2<float> aSize) {
		Mutex.lock();
		this->Size = aSize;
		this->SizeVSC = this->Display->convert_to_screen(aSize);
		math::vec2<int> GlobalPosition = this->Display->convert_to_global_position(this->PositionVSC, this->SizeVSC);
		glfwSetWindowPos(Window, GlobalPosition.x, GlobalPosition.y);
		Mutex.unlock();
	}

	void system_window::set_resolution(math::vec2<uint> aResolution) {
		math::vec2<int> NewSize(aResolution.x, aResolution.y);
		this->set_size(NewSize);
	}

	void system_window::set_position(math::vec2<int> aPosition) {
		Mutex.lock();
		this->Position = this->Display->convert_to_physical_position(aPosition);
		this->PositionVSC = aPosition;
		math::vec2<int> GlobalPosition = this->Display->convert_to_global_position(aPosition, this->SizeVSC);
		glfwSetWindowPos(Window, GlobalPosition.x, GlobalPosition.y);
		Mutex.unlock();
	}

	void system_window::set_size(math::vec2<int> aSize) {
		Mutex.lock();
		this->Size = this->Display->convert_to_physical_size(aSize);
		this->SizeVSC = aSize;
		math::vec2<int> GlobalPosition = this->Display->convert_to_global_position(this->PositionVSC, this->SizeVSC);
		math::vec2<int> FramebufferResolution;
		glfwSetWindowSize(Window, SizeVSC.x, SizeVSC.y);
		glfwSetWindowPos(Window, GlobalPosition.x, GlobalPosition.y);
		glfwGetFramebufferSize(this->Window, &FramebufferResolution.x, &FramebufferResolution.y);
		this->FrameResolution = math::vec3<uint>(FramebufferResolution.x, FramebufferResolution.y, 1u);
		// Need to refresh window.
		Mutex.unlock();
	}

	void system_window::set_option(setting::id, bool aValue) {

	}

	// ------------------------------ protected methods ---------------------------- //
	
	void system_window::update(double aDeltaTime) {
		//VkSubmitInfo TransferBatch{};
		//TransferBatch.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
		//TransferBatch.pNext = NULL;
		//TransferBatch.waitSemaphoreCount = 0;
		//TransferBatch.pWaitSemaphores = NULL;
		//TransferBatch.pWaitDstStageMask = NULL;
		//TransferBatch.commandBufferCount = 0;
		//TransferBatch.pCommandBuffers = NULL;
		//TransferBatch.signalSemaphoreCount = 0;
		//TransferBatch.pSignalSemaphores = NULL;
		//this->Mutex.lock();
		//this->Time += aDeltaTime;
		//this->Mutex.unlock();
		//return TransferBatch;
	}

	//std::vector<VkSubmitInfo> system_window::render(stage_t* aStage) {
	//	VkResult Result = VK_SUCCESS;
	//	std::vector<VkSubmitInfo> Batch;
	//	stage::canvas* Canvas = (stage::canvas*)aStage;

	//	//this->Mutex.lock();

	//	Result = this->next_frame();

	//	// If swapchain is out of date, discard frame and wait for swapchain recreation.
	//	if ((Result == VK_ERROR_OUT_OF_DATE_KHR) || (Result == VK_SUBOPTIMAL_KHR)) return Batch;

	//	// Get all draw commands from each object
	//	Frame[FrameDrawIndex].DrawCommand.resize(Canvas->Object.Handle.size() + 2);
	//	Frame[FrameDrawIndex].DrawCommand[0] = PreRenderOperations[FrameDrawIndex];
	//	for (size_t i = 0; i < Canvas->Object.Handle.size(); i++) {
	//		Frame[FrameDrawIndex].DrawCommand[i + 1] = Canvas->Object[i]->draw(this);
	//	}
	//	Frame[FrameDrawIndex].DrawCommand[Canvas->Object.Handle.size() + 1] = PostRenderOperations[FrameDrawIndex];
	//	
	//	Batch += Frame[FrameDrawIndex].DrawCommand;

	//	Batch += this->present_frame();

	//	//this->Mutex.unlock();

	//	return Batch;
	//}

	VkResult system_window::next_frame() {
		return vkAcquireNextImageKHR(Context->handle(), Swapchain->handle(), UINT64_MAX, RenderWait, VK_NULL_HANDLE, &FrameDrawIndex);
	}

	VkPresentInfoKHR system_window::present_frame() {
		return this->PresentInfo;
	}

	// ------------------------------ private methods ------------------------------ //

	VkResult system_window::create_system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const create_info& aProperty, math::vec2<int> aPosition, math::vec2<int> aResolution) {
		VkResult Result = VK_SUCCESS;

		this->FrameResolution	= math::vec3<uint>(aResolution.x, aResolution.y, 1u);
		this->Title				= aName;

		// Create GLFW Window and Vulkan Surface.
		int ReturnCode = this->create_glfw_window(this->Engine, aProperty.Window, this->SizeVSC.x, this->SizeVSC.y, this->Title.ptr(), this->Display);
		if (ReturnCode == GLFW_NO_ERROR) {
			*this->Engine << log::message(log::INFO, log::SUCCESS, "System Window Creation", log::GLFW, "system_window", this->Name.Handle, "GLFW Window Creation Successful!");
		} else {
			*this->Engine << log::message(ReturnCode, "System Window Creation", log::GLFW, "system_window", this->Name.Handle, "GLFW Window Creation Failure!");
		}

		// Create Vulkan Surface
		if (ReturnCode == GLFW_NO_ERROR) {
			Result = this->create_vulkan_surface(aContext->parent_engine(), aContext, this->Window, this->Surface);
			if (Result == VK_SUCCESS) {
				*this->Engine << log::message(log::INFO, log::SUCCESS, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Vulkan Surface Creation Successful!");
			} else {
				*this->Engine << log::message(Result, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Vulkan Surface Creation Failed!");
			}
		}

		// Create Semaphores
		if (Result == VK_SUCCESS) {
			Result = this->create_semaphores();
		}
		
		// Create Vulkan Swapchain
		if (Result == VK_SUCCESS) {
			this->Swapchain = new gcl::swapchain(aContext, this->Surface, aProperty.Swapchain);
			if (Result == VK_SUCCESS) {
				*this->Engine << log::message(log::INFO, log::SUCCESS, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Vulkan Swapchain Creation Successful!");
			}
			else {
				*this->Engine << log::message(Result, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Vulkan Surface Creation Failed!");
			}
		}

		// Get Swapchain Images
		if (Result == VK_SUCCESS) {
			Result = this->create_images();
			if (Result == VK_SUCCESS) {
				*this->Engine << log::message(log::INFO, log::SUCCESS, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Swapchain Images Fetched!");
			}
			else {
				*this->Engine << log::message(Result, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Couldn't Fetch Swapchain Images!");
			}
		} 

		if (Result == VK_SUCCESS) {
			Result = this->create_cscbs();
		}

		if (Result == VK_SUCCESS) {
			Result = this->create_render_pass();
		}

		if (Result == VK_SUCCESS) {
			Result = this->create_framebuffers();
		}

		if (Result == VK_SUCCESS) {
			Result = this->create_pipelines();
		}

		return Result;
	}

	int system_window::create_glfw_window(engine* aEngine, window::setting aSetting, int aWidth, int aHeight, const char* aTitle, system_display* aDisplay) {
		// Create GLFW Window
		if ((aSetting.Fullscreen) && (aDisplay != nullptr)) {
			this->Window = create_window_handle(aSetting, aWidth, aHeight, aTitle, aDisplay->Handle, NULL);
		}
		else {
			this->Window = create_window_handle(aSetting, aWidth, aHeight, aTitle, NULL, NULL);
		}

		if (this->Window != NULL) {
			math::vec2<int> FramebufferResolution;
			// User pointer to forward input stream.
			glfwSetWindowUserPointer(this->Window, (void*)this);

			// system_window callbacks
			glfwSetWindowPosCallback(this->Window, system_window::position_callback);
			glfwSetWindowSizeCallback(this->Window, system_window::size_callback);
			glfwSetWindowCloseCallback(this->Window, system_window::close_callback);
			glfwSetWindowRefreshCallback(this->Window, system_window::refresh_callback);
			glfwSetWindowFocusCallback(this->Window, system_window::focus_callback);
			glfwSetWindowIconifyCallback(this->Window, system_window::iconify_callback);
			glfwSetWindowMaximizeCallback(this->Window, system_window::maximize_callback);
			glfwSetWindowContentScaleCallback(this->Window, system_window::content_scale_callback);

			// framebuffer callbacks
			glfwSetFramebufferSizeCallback(this->Window, system_window::framebuffer_size_callback);

			// Mouse callbacks
			glfwSetMouseButtonCallback(this->Window, system_window::mouse_button_callback);
			glfwSetCursorPosCallback(this->Window, system_window::cursor_position_callback);
			glfwSetCursorEnterCallback(this->Window, system_window::cursor_enter_callback);
			glfwSetScrollCallback(this->Window, system_window::scroll_callback);

			// Keyboard callbacks
			glfwSetKeyCallback(this->Window, system_window::key_callback);
			glfwSetCharCallback(this->Window, system_window::character_callback);

			// File drop
			glfwSetDropCallback(this->Window, system_window::file_drop_callback);

			glfwGetFramebufferSize(this->Window, &FramebufferResolution.x, &FramebufferResolution.y);
			this->FrameResolution = math::vec3<uint>(FramebufferResolution.x, FramebufferResolution.y, 1u);

			return GLFW_NO_ERROR;
		} else {
			const char* ReturnErrorString;
			int ReturnErrorCode = glfwGetError(&ReturnErrorString);
			*aEngine << log::message(ReturnErrorCode, "System Window Creation", log::GLFW, "system_window", this->Name.Handle, ReturnErrorString);
			return ReturnErrorCode;
		}
	}

	VkResult system_window::create_vulkan_surface(engine* aEngine, gcl::context* aContext, GLFWwindow* aWindow, VkSurfaceKHR aSurface) {
		device* Device = Context->parent_device();
		VkResult Result = glfwCreateWindowSurface(aEngine->handle(), aWindow, NULL, &aSurface);
		if (Result == VK_SUCCESS) {
			VkBool32 isSupported = VK_FALSE;
			Result = vkGetPhysicalDeviceSurfaceSupportKHR(Device->handle(), Context->qfi(device::operation::PRESENT), this->Surface, &isSupported);
			if (Result == VK_SUCCESS) {
				if (isSupported == VK_TRUE) {
					*Engine << log::message(log::INFO, log::SUCCESS, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Vulkan Surface Presentation Support: Available!");
				}
				else {
					*Engine << log::message(log::INFO, log::SUCCESS, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Vulkan Surface Presentation Support: NOT Available!");
				}
			}
			else {
				*Engine << log::message(Result, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Failed to query surface presentation capabilities!");
			}
		}
		else {
			*Engine << log::message(Result, "System Window Creation", log::VULKAN, "system_window", this->Name.Handle, "Failed to create Vulkan Surface!");
			vkDestroySurfaceKHR(Engine->handle(), Surface, NULL);
		}
		return Result;
	}

	VkResult system_window::create_semaphores() {
		this->PipelineStageFlags	= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		this->PresentationResult	= VK_SUCCESS;
		this->RenderWait			= Context->create_semaphore();
		this->RenderComplete		= Context->create_semaphore();
		return VK_SUCCESS;
	}

	//VkResult system_window::create_swapchain(create_info aProperty) {
	//	VkResult Result = VK_SUCCESS;
	//	gcl::device* aDevice = Context->parent_device();
	//	VkSurfaceCapabilitiesKHR SurfaceCapabilities		= aDevice->get_surface_capabilities(Surface);
	//	std::vector<VkSurfaceFormatKHR> SurfaceFormat	= aDevice->get_surface_format(Surface);
	//	std::vector<VkPresentModeKHR> PresentMode		= aDevice->get_surface_present_mode(Surface);

	//	bool SurfaceFormatSupported = false;
	//	for (size_t i = 0; i < SurfaceFormat.size(); i++) {
	//		if ((SurfaceFormat[i].format == aProperty.PixelFormat) && (SurfaceFormat[i].colorSpace == aProperty.Swapchain.ColorSpace)) {
	//			SurfaceFormatSupported = true;
	//			break;
	//		}
	//	}

	//	bool PresentModeSupported = false;
	//	for (size_t i = 0; i < PresentMode.size(); i++) {
	//		if (PresentMode[i] == aProperty.Swapchain.PresentMode) {
	//			PresentModeSupported = true;
	//			break;
	//		}
	//	}

	//	bool TransformSupported = true;
	//	// TODO: Test for transform support.

	//	bool CompositeAlphaSupported = false;
	//	CompositeAlphaSupported = ((aProperty.Swapchain.CompositeAlpha & SurfaceCapabilities.supportedCompositeAlpha) == aProperty.Swapchain.CompositeAlpha);

	//	bool ImageUsageSupported = false;
	//	ImageUsageSupported = ((aProperty.Swapchain.Usage & SurfaceCapabilities.supportedUsageFlags) == aProperty.Swapchain.Usage);

	//	// Check is options are supported by device and surface.
	//	if ((!SurfaceFormatSupported) || (!PresentModeSupported) || (!TransformSupported) || (!CompositeAlphaSupported) || (!ImageUsageSupported)) return VK_ERROR_FEATURE_NOT_PRESENT;

	//	// (VkFormat aFormat, VkColorSpaceKHR aColorSpace, VkImageUsageFlags aFlags, VkCompositeAlphaFlagBitsKHR aCompositeAlpha, VkPresentModeKHR aPresentMode, VkBool32 aClipped);
	//	
	//	this->CreateInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	//	this->CreateInfo.pNext					= NULL;
	//	this->CreateInfo.flags					= 0;
	//	this->CreateInfo.surface				= Surface;
	//	this->CreateInfo.minImageCount			= aProperty.Swapchain.FrameCount; //std::clamp(aProperty.Swapchain.FrameCount, SurfaceCapabilities.minImageCount, SurfaceCapabilities.maxImageCount);
	//	this->CreateInfo.imageFormat			= aProperty.PixelFormat;
	//	this->CreateInfo.imageColorSpace		= (VkColorSpaceKHR)aProperty.Swapchain.ColorSpace;
	//	this->CreateInfo.imageExtent 			= SurfaceCapabilities.currentExtent;
	//	this->CreateInfo.imageArrayLayers		= std::clamp(1u, 1u, SurfaceCapabilities.maxImageArrayLayers);
	//	this->CreateInfo.imageUsage				= aProperty.Swapchain.Usage;
	//	this->CreateInfo.imageSharingMode		= VK_SHARING_MODE_EXCLUSIVE;
	//	this->CreateInfo.queueFamilyIndexCount	= 0;
	//	this->CreateInfo.pQueueFamilyIndices	= NULL;
	//	this->CreateInfo.preTransform			= SurfaceCapabilities.currentTransform; 
	//	this->CreateInfo.compositeAlpha			= (VkCompositeAlphaFlagBitsKHR)aProperty.Swapchain.CompositeAlpha;
	//	this->CreateInfo.presentMode			= (VkPresentModeKHR)aProperty.Swapchain.PresentMode;
	//	this->CreateInfo.clipped				= aProperty.Swapchain.Clipped;
	//	this->CreateInfo.oldSwapchain			= this->Swapchain;

	//	return vkCreateSwapchainKHR(Context->handle(), &this->CreateInfo, NULL, &this->Swapchain);
	//}

	VkResult system_window::create_images() {
		VkResult Result = VK_SUCCESS;
		uint32_t ImageCount = 0;

		Result = vkGetSwapchainImagesKHR(Context->handle(), Swapchain->handle(), &ImageCount, NULL);
		if (Result != VK_SUCCESS) return Result;

		std::vector<VkImage> ImageHandle(ImageCount);
		Result = vkGetSwapchainImagesKHR(Context->handle(), Swapchain->handle(), &ImageCount, ImageHandle.data());
		if (Result != VK_SUCCESS) return Result;

		//for (int i = 0; i < ImageCount; i++) {
		//	this->Frame[i].Attachment[0] = this->Frame[i].Image[0].create_system_window_frame(Context, this->CreateInfo.imageFormat, this->CreateInfo.imageUsage, ImageHandle[i], this->FrameResolution);
		//}

		// Transition to Default being PRESENT_SRC
		{
			VkCommandBufferBeginInfo TransitionBeginInfo{};
			VkCommandBuffer TransitionCommandBuffer = CommandPool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			VkFence TransitionFence = Context->create_fence();

			TransitionBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			TransitionBeginInfo.pNext = NULL;
			TransitionBeginInfo.flags = 0;
			TransitionBeginInfo.pInheritanceInfo = NULL;

			//Result = vkBeginCommandBuffer(TransitionCommandBuffer, &TransitionBeginInfo);

			//for (int i = 0; i < this->Frame.size(); i++) {
			//	this->Frame[i].Image[0].cmd_transition(
			//		TransitionCommandBuffer,
			//		VK_ACCESS_NONE,							VK_ACCESS_MEMORY_READ_BIT,
			//		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			//												VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			//	);
			//}

			//Result = vkEndCommandBuffer(TransitionCommandBuffer);

			Result = Context->execute(device::operation::GRAPHICS, TransitionCommandBuffer, TransitionFence);

			Result = vkWaitForFences(Context->handle(), 1, &TransitionFence, VK_TRUE, UINT64_MAX);

			CommandPool.release(TransitionCommandBuffer);
			Context->destroy_fence(TransitionFence);
		}

		// Only a single attachment exists for this render target.
		this->AttachmentDescription[0].flags				= 0;
		this->AttachmentDescription[0].format				= this->CreateInfo.imageFormat;
		this->AttachmentDescription[0].samples				= VK_SAMPLE_COUNT_1_BIT;
		this->AttachmentDescription[0].loadOp				= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		this->AttachmentDescription[0].storeOp				= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		this->AttachmentDescription[0].stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		this->AttachmentDescription[0].stencilStoreOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		this->AttachmentDescription[0].initialLayout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		this->AttachmentDescription[0].finalLayout			= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		this->PresentInfo.sType								= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		this->PresentInfo.pNext								= NULL;
		this->PresentInfo.waitSemaphoreCount				= 1;
		this->PresentInfo.pWaitSemaphores					= &this->RenderComplete;
		this->PresentInfo.swapchainCount					= 1;
		this->PresentInfo.pSwapchains						= &this->Swapchain->Handle;
		this->PresentInfo.pImageIndices						= &this->FrameDrawIndex;
		this->PresentInfo.pResults							= &this->PresentationResult;

		return Result;
	}

	VkResult system_window::create_cscbs() {
		VkResult Result = VK_SUCCESS;

		PreRenderOperations.resize(Frame.size());
		PostRenderOperations.resize(Frame.size());
		for (size_t i = 0; i < Frame.size(); i++) {
			VkCommandBufferBeginInfo	CSCBBI{};
			VkImageSubresourceRange		Range;
			VkClearColorValue			ClearColor = { (127.0f / 256.0f), 0.0f, (256.0f / 256.0f), 0.0f };

			PreRenderOperations[i]		|= CommandPool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			PostRenderOperations[i]		|= CommandPool.allocate(VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			Range.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
			Range.baseMipLevel			= 0;
			Range.levelCount			= 1;
			Range.baseArrayLayer		= 0;
			Range.layerCount			= 1;

			CSCBBI.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			CSCBBI.pNext				= NULL;
			CSCBBI.flags				= 0;
			CSCBBI.pInheritanceInfo		= NULL;

			//// Called before render operations, clears screen
			//Result = vkBeginCommandBuffer(PreRenderOperations[i][0], &CSCBBI);
			//Frame[i].Image[0].cmd_transition(
			//	PreRenderOperations[i][0],
			//	VK_ACCESS_NONE,							VK_ACCESS_MEMORY_WRITE_BIT,
			//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,		VK_PIPELINE_STAGE_TRANSFER_BIT,
			//											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			//);
			//vkCmdClearColorImage(PreRenderOperations[i][0], Frame[i].Image[0].Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColor, 1, &Range);
			//Frame[i].Image[0].cmd_transition(
			//	PreRenderOperations[i][0],
			//	VK_ACCESS_MEMORY_WRITE_BIT,				VK_ACCESS_MEMORY_WRITE_BIT,
			//	VK_PIPELINE_STAGE_TRANSFER_BIT,			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			//											VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
			//);
			//Result = vkEndCommandBuffer(PreRenderOperations[i][0]);

			//Result = vkBeginCommandBuffer(PostRenderOperations[i][0], &CSCBBI);
			//Frame[i].Image[0].cmd_transition(
			//	PostRenderOperations[i][0],
			//	VK_ACCESS_MEMORY_WRITE_BIT,				VK_ACCESS_MEMORY_READ_BIT,
			//	VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			//											VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			//);
			//Result = vkEndCommandBuffer(PostRenderOperations[i][0]);

			//PreRenderOperations[i].wait_on(RenderWait, PipelineStageFlags);
			//PostRenderOperations[i].signal_to(RenderComplete);
		}

		return Result;
	}

	VkResult system_window::create_render_pass() {
		VkResult									Result = VK_SUCCESS;

		// Custom Render Pass Creation
		VkRenderPassCreateInfo					RenderPassCreateInfo;
		VkAttachmentReference						AttachmentReference;
		VkSubpassDescription						SubpassDescription;
		VkSubpassDependency						SubpassDependency;

		// The total number of attachments for the render pass.
		RenderPassCreateInfo.sType						= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassCreateInfo.pNext						= NULL;
		RenderPassCreateInfo.flags						= 0;
		RenderPassCreateInfo.attachmentCount			= AttachmentDescription.size();
		RenderPassCreateInfo.pAttachments				= AttachmentDescription.data();
		RenderPassCreateInfo.subpassCount				= 1;
		RenderPassCreateInfo.pSubpasses					= &SubpassDescription;
		RenderPassCreateInfo.dependencyCount			= 1;
		RenderPassCreateInfo.pDependencies				= &SubpassDependency;

		// Take the first element of the render_target attachments, and transition it to COLOR_ATTACHMENT_OPTIMAL
		AttachmentReference.attachment					= 0;
		AttachmentReference.layout						= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// First Subpass
		SubpassDescription.flags						= 0;
		SubpassDescription.pipelineBindPoint			= Rasterizer.BindPoint;
		SubpassDescription.inputAttachmentCount			= 0;
		SubpassDescription.pInputAttachments			= NULL;
		SubpassDescription.colorAttachmentCount			= 1;
		SubpassDescription.pColorAttachments			= &AttachmentReference;
		SubpassDescription.pResolveAttachments			= NULL;
		SubpassDescription.pDepthStencilAttachment		= NULL;
		SubpassDescription.preserveAttachmentCount		= 0;
		SubpassDescription.pPreserveAttachments			= NULL;
		// Dependency
		SubpassDependency.srcSubpass					= VK_SUBPASS_EXTERNAL;
		SubpassDependency.dstSubpass					= 0;
		SubpassDependency.srcStageMask					= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		SubpassDependency.dstStageMask					= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		SubpassDependency.srcAccessMask					= VK_ACCESS_MEMORY_WRITE_BIT;
		SubpassDependency.dstAccessMask					= VK_ACCESS_MEMORY_READ_BIT;
		SubpassDependency.dependencyFlags				= 0;

		return vkCreateRenderPass(Context->handle(), &RenderPassCreateInfo, NULL, &RenderPass);
	}

	VkResult system_window::create_pipelines() {
		VkResult Result = VK_SUCCESS;

		// Color Blending Info.
		VkPipelineColorBlendAttachmentState AttachmentBlending;

		// Load Shader Source, and compile to SPIRV. 
		gcl::shader* VertexSource = (gcl::shader*)Engine->open("assets/shader/window.vert");
		gcl::shader* PixelSource = (gcl::shader*)Engine->open("assets/shader/window.frag");

		gcl::shader* ShaderList[2] = { VertexSource, PixelSource };
		Rasterizer = gcl::pipeline::rasterizer(2, ShaderList);

		// Subpass 0:

		// Describe how Vertex Buffers must be be accessed while bound.
		Rasterizer.Input.vertexBindingDescriptionCount = 1;
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 0, offsetof(physics::mesh::vertex, Position));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 1, offsetof(physics::mesh::vertex, TextureCoordinate));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 2, offsetof(physics::mesh::vertex, Color));

		// How to intepret vertex data in rasterization.
		Rasterizer.InputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		Rasterizer.InputAssembly.primitiveRestartEnable		= false;

		// Viewport crap.
		Rasterizer.Viewport.scissorCount					= 1;
		Rasterizer.Viewport.pScissors						= &DefaultScissor;
		Rasterizer.Viewport.viewportCount					= 1;
		Rasterizer.Viewport.pViewports						= &DefaultViewport;

		// Rasterizer Info
		Rasterizer.Rasterizer.rasterizerDiscardEnable		= VK_FALSE;
		Rasterizer.Rasterizer.polygonMode					= VK_POLYGON_MODE_FILL;
		Rasterizer.Rasterizer.cullMode						= VK_CULL_MODE_BACK_BIT;
		Rasterizer.Rasterizer.frontFace						= VK_FRONT_FACE_COUNTER_CLOCKWISE;

		// Copy Paste
		Rasterizer.Multisample.rasterizationSamples			= VK_SAMPLE_COUNT_1_BIT;

		// Per Attachment Blending.
		AttachmentBlending.blendEnable						= VK_FALSE;
		AttachmentBlending.srcColorBlendFactor				= VK_BLEND_FACTOR_SRC_ALPHA;
		AttachmentBlending.dstColorBlendFactor				= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		AttachmentBlending.colorBlendOp						= VK_BLEND_OP_ADD;
		AttachmentBlending.srcAlphaBlendFactor				= VK_BLEND_FACTOR_ONE;
		AttachmentBlending.dstAlphaBlendFactor				= VK_BLEND_FACTOR_ZERO;
		AttachmentBlending.alphaBlendOp						= VK_BLEND_OP_ADD;
		AttachmentBlending.colorWriteMask					= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		Rasterizer.ColorBlend.logicOpEnable					= VK_FALSE;
		Rasterizer.ColorBlend.logicOp						= VK_LOGIC_OP_COPY;
		Rasterizer.ColorBlend.attachmentCount				= this->AttachmentDescription.size();
		Rasterizer.ColorBlend.pAttachments					= &AttachmentBlending;

		Pipeline.resize(1);
		Pipeline[0] = gcl::pipeline(Context, Rasterizer, RenderPass, 0);

		return Result;
	}

	VkResult system_window::recreate_swapchain(int aFrameSizeX, int aFrameSizeY) {
		VkResult Result = VK_SUCCESS;

		// Wait for all resoure operations to finish.
		Result = vkDeviceWaitIdle(Context->handle());

		FrameResolution = math::vec3<uint>(aFrameSizeX, aFrameSizeY, 1u);

		// Destroys old swapchain.
		for (int i = 0; i < Frame.size(); i++) {
			CommandPool.release(PreRenderOperations[i]);
			PreRenderOperations[i].resize(0);
			CommandPool.release(PostRenderOperations[i]);
			PostRenderOperations[i].resize(0);
			Frame[i].Image[0].Handle = VK_NULL_HANDLE;
			if (Frame[i].Attachment[0] != VK_NULL_HANDLE) {
				vkDestroyImageView(Context->handle(), Frame[i].Attachment[0], NULL);
				Frame[i].Attachment[0] = VK_NULL_HANDLE;
			}
		}

		VkSurfaceCapabilitiesKHR SurfaceCapabilities = Context->parent_device()->get_surface_capabilities(Surface);

		CreateInfo.imageExtent 		= SurfaceCapabilities.currentExtent;
		CreateInfo.oldSwapchain		= Swapchain->Handle;
		Result = vkCreateSwapchainKHR(Context->handle(), &CreateInfo, NULL, &Swapchain->Handle);

		Result = this->create_images();

		Result = this->create_cscbs();

		Result = this->create_framebuffers();

		return Result;
	}

	void system_window::clear() {
		if (Context != nullptr) {
			for (int i = 0; i < Frame.size(); i++) {
				CommandPool.release(PreRenderOperations[i]);
				PreRenderOperations[i].resize(0);
				CommandPool.release(PostRenderOperations[i]);
				PostRenderOperations[i].resize(0);
				Frame[i].Image[0].Handle = VK_NULL_HANDLE;
				if (Frame[i].Attachment[0] != VK_NULL_HANDLE) {
					vkDestroyImageView(Context->handle(), Frame[i].Attachment[0], NULL);
					Frame[i].Attachment[0] = VK_NULL_HANDLE;
				}
			}
			if (Swapchain != VK_NULL_HANDLE) {
				vkDestroySwapchainKHR(Context->handle(), Swapchain->Handle, NULL);
			}
			Context->destroy_semaphore(RenderComplete);
			Context->destroy_semaphore(RenderWait);
		}
		if (Engine != nullptr) {
			if (Surface != VK_NULL_HANDLE) {
				vkDestroySurfaceKHR(Engine->handle(), Surface, NULL);
			}
		}
		glfwDestroyWindow(Window);
		this->zero_out();
	}

	void system_window::zero_out() {
		this->PositionVSC				= math::vec2<int>(0, 0);
		this->SizeVSC					= math::vec2<int>(640, 480);
		this->Display					= nullptr;
		this->Window					= NULL;
		this->Surface					= VK_NULL_HANDLE;
		this->CreateInfo				= {};
		this->Swapchain					= VK_NULL_HANDLE;
		this->PipelineStageFlags		= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		this->PresentationResult		= VK_SUCCESS;
		this->RenderWait				= VK_NULL_HANDLE;
		this->RenderComplete			= VK_NULL_HANDLE;
		this->PresentInfo				= {};

		this->CreateInfo.sType			= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		this->CreateInfo.pNext			= NULL;
		this->PresentInfo.sType			= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		this->PresentInfo.pNext			= NULL;
	}

	// static 

	bool system_window::initialize() {
		if (glfwInit() == GLFW_TRUE) {
			MainThreadID = std::this_thread::get_id();
			uint32_t Count = 0;
			const char** List = glfwGetRequiredInstanceExtensions(&Count);
			RequiredInstanceExtension.resize(Count);
			for (uint32_t i = 0; i < Count; i++) {
				RequiredInstanceExtension[i] = List[i];
			}
			return true;
		}
		else {
			return false;
		}
	}

	void system_window::terminate() {
		glfwTerminate();
	}

	void system_window::poll_events() {
		glfwPollEvents();
	}

	GLFWwindow* system_window::create_window_handle(window::setting aProperty, int aWidth, int aHeight, const char* aTitle, glfw_monitor* aMonitor, GLFWwindow* aWindow) {
		GLFWwindow* Temp = NULL;
		if (MainThreadID == std::this_thread::get_id()) {
			glfwWindowHint(GLFW_RESIZABLE,			aProperty.Resizable);
			glfwWindowHint(GLFW_DECORATED,			aProperty.Decorated);
			glfwWindowHint(GLFW_FOCUSED,			aProperty.UserFocused);
			glfwWindowHint(GLFW_AUTO_ICONIFY,		aProperty.AutoMinimize);
			glfwWindowHint(GLFW_FLOATING,			aProperty.Floating);
			glfwWindowHint(GLFW_MAXIMIZED,			aProperty.Maximized);
			glfwWindowHint(GLFW_VISIBLE,			aProperty.Visible);
			glfwWindowHint(GLFW_SCALE_TO_MONITOR,	aProperty.ScaleToMonitor);
			glfwWindowHint(GLFW_CENTER_CURSOR,		aProperty.CenterCursor);
			glfwWindowHint(GLFW_FOCUS_ON_SHOW,		aProperty.FocusOnShow);
			glfwWindowHint(GLFW_CLIENT_API,			GLFW_NO_API);
			glfwWindowHint(GLFW_REFRESH_RATE,		GLFW_DONT_CARE);

			Temp = glfwCreateWindow(aWidth, aHeight, aTitle, aMonitor, aWindow);
		}
		else {
			WindowTempData.Property = aProperty;
			WindowTempData.Width = aWidth;
			WindowTempData.Height = aHeight;
			WindowTempData.Title = aTitle;
			WindowTempData.Monitor = aMonitor;
			WindowTempData.Window = aWindow;

			WindowCreated.store(false);
			SignalCreate.store(true);
			// Wait for window to be created.
			while (!WindowCreated.load()) {}
			Temp = ReturnWindow;
		}
		return Temp;
	}

	void system_window::destroy_window_handle(GLFWwindow* aWindow) {
		if (MainThreadID == std::this_thread::get_id()) {
			glfwDestroyWindow(aWindow);
		}
		else {
			while (DestroyWindow.load() != NULL) {}
			DestroyWindow.store(aWindow);
		}
	}

	void system_window::process_window_handle_call() {
		if (SignalCreate.load()) {
			glfwWindowHint(GLFW_RESIZABLE,			WindowTempData.Property.Resizable);
			glfwWindowHint(GLFW_DECORATED,			WindowTempData.Property.Decorated);
			glfwWindowHint(GLFW_FOCUSED,			WindowTempData.Property.UserFocused);
			glfwWindowHint(GLFW_AUTO_ICONIFY,		WindowTempData.Property.AutoMinimize);
			glfwWindowHint(GLFW_FLOATING,			WindowTempData.Property.Floating);
			glfwWindowHint(GLFW_MAXIMIZED,			WindowTempData.Property.Maximized);
			glfwWindowHint(GLFW_VISIBLE,			WindowTempData.Property.Visible);
			glfwWindowHint(GLFW_SCALE_TO_MONITOR,	WindowTempData.Property.ScaleToMonitor);
			glfwWindowHint(GLFW_CENTER_CURSOR,		WindowTempData.Property.CenterCursor);
			glfwWindowHint(GLFW_FOCUS_ON_SHOW,		WindowTempData.Property.FocusOnShow);
			glfwWindowHint(GLFW_CLIENT_API,			GLFW_NO_API);
			glfwWindowHint(GLFW_REFRESH_RATE,		GLFW_DONT_CARE);

			ReturnWindow = glfwCreateWindow(WindowTempData.Width, WindowTempData.Height, WindowTempData.Title, WindowTempData.Monitor, WindowTempData.Window);
			WindowCreated.store(true);
			SignalCreate.store(false);
		}
		
		// Check if window needs to be destroyed.
		GLFWwindow* temp = DestroyWindow.load();
		if (temp != NULL) {
			glfwDestroyWindow(temp);
			DestroyWindow.store(NULL);
		}		
	}

	// ------------------------------ callback methods ----------------------------- //

	void system_window::position_callback(GLFWwindow* aWindowHandle, int aPosX, int aPosY) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
		//Window->Position		= Window->Display->convert_to_display_position(aPosX, aPosY);
		Window->PositionVSC		= math::vec2<int>(aPosX, aPosY);
	}

	void system_window::size_callback(GLFWwindow* aWindowHandle, int aSizeX, int aSizeY) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
		Window->Size			= Window->Display->convert_to_physical_size(math::vec2<int>(aSizeX, aSizeY));
		Window->SizeVSC			= math::vec2<int>(aSizeX, aSizeY);
	}

	void system_window::close_callback(GLFWwindow* aWindowHandle) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
		Window->Setting.ShouldClose = GLFW_TRUE;
	}

	// 
	void system_window::refresh_callback(GLFWwindow* aWindowHandle) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	void system_window::focus_callback(GLFWwindow* aWindowHandle, int aFocused) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	void system_window::iconify_callback(GLFWwindow* aWindowHandle, int aIconified) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	void system_window::maximize_callback(GLFWwindow* aWindowHandle, int aMaximized) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);


	}

	void system_window::content_scale_callback(GLFWwindow* aWindowHandle, float aXScale, float aYScale) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
	}

	// framebuffer callbacks

	void system_window::framebuffer_size_callback(GLFWwindow* aWindowHandle, int aFrameSizeX, int aFrameSizeY) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
		//Window->FrameResolution = math::vec3<uint>(aFrameSizeX, aFrameSizeY, 1u);
		//Window->Engine->ThreadController.suspend(Window->Engine->RenderThreadID);
		//Window->recreate_swapchain(aFrameSizeX, aFrameSizeY);
		//Window->Engine->ThreadController.resume(Window->Engine->RenderThreadID);
	}

	// Mouse callbacks

	void system_window::mouse_button_callback(GLFWwindow* aWindowHandle, int aButton, int aAction, int aMods) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
	}

	void system_window::cursor_position_callback(GLFWwindow* aWindowHandle, double aPosX, double aPosY) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	void system_window::cursor_enter_callback(GLFWwindow* aWindowHandle, int aEntered) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	void system_window::scroll_callback(GLFWwindow* aWindowHandle, double aOffsetX, double aOffsetY) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	void system_window::key_callback(GLFWwindow* aWindowHandle, int aKey, int aScancode, int aAction, int aMods) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);
	}

	void system_window::character_callback(GLFWwindow* aWindowHandle, uint aCodepoint) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

	// File Drops

	void system_window::file_drop_callback(GLFWwindow* aWindowHandle, int aPathCount, const char** aPath) {
		system_window* Window = (system_window*)glfwGetWindowUserPointer(aWindowHandle);

	}

}
