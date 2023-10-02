#pragma once
#ifndef GEODESUKA_CORE_OBJECT_SYSTEM_WINDOW_H
#define GEODESUKA_CORE_OBJECT_SYSTEM_WINDOW_H

#include <vector>
#include <thread>

#include "../math.h"

#include "../gcl/config.h"
#include "../gcl/device.h"
#include "../gcl/context.h"
#include "../gcl/image.h"
#include "../gcl/framebuffer.h"
#include "../gcl/swapchain.h"

#include "../object_t.h"
#include "window.h"
#include "system_display.h"

struct GLFWwindow;

/// <summary>
/// system_window.h is an object that will represent Operating System (OS) managed windows, managed
/// by a window manager like Wayland, X11, win32, and so on. It will also be a render target that
/// can be used by other objects to create render commands for. Position and Size will accept
/// two units for modifying the state of a SystemWindow instance. It will take physical coordinates
/// in units of meters which the position and size will be in reference to the parent display it
/// is a child of.
/// </summary>
namespace geodesuka::core::object {

	class system_window : public window {
	public:

		friend class geodesuka::engine;

		// system_window::create_info has two important aspects to it, the first being the window::setting, which 
		// describes certain properties of the window frame itself. The second is the swapchain::property, which
		// are the qualities which describe the contents and swapchain images of the window. 
		struct create_info {
			window::setting Window;
			gcl::swapchain::property Swapchain;
			float FrameRate;
		};

		// Required Extensions for the class
		static std::vector<const char*> RequiredInstanceExtension;
		static std::vector<const char*> RequiredContextExtension;
		static constexpr int ID = 2;

		// When creating a system window, it requires a device context which will designate the device that will be used to render to the window.
		// Any object that wishes to be rendered to the window must be created with the same device context as the system window. The second 
		// argument is the Display the system window will be associated with. It will also be the display which the window can be transitioned
		// fullscreen mode as well. The third argument is the name of the window, which will be used by the window manager to identify the window.
		// It will also be the string identifier of the system window object. The fourth argument contains all the relevant information for the
		// properties of the window, and the swapchain.
		
		// The first constructor using integers for the position and size will, use screen coordinates defined by the GLFW API. The position is 
		// absolute screen coordinates for the constellation of displays the host system uses. The size is of course in screen coordinates as well.
		system_window(gcl::context* aContext, system_display* aDisplay, const char* aName, const create_info& aCreateInfo, math::vec2<int> aPosition, math::vec2<int> aSize);
		// The second constructor uses floating point values for the position and size, which is in units of centimeters. The position of the system
		// window is the physical position on the display starting from the center. The size is the physical size of the window on the provided display
		// in the argument of the constructor.
		system_window(gcl::context* aContext, system_display* aDisplay, const char* aName, const create_info& aCreateInfo, math::vec3<float> aPosition, math::vec2<float> aSize);

		// system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, math::vec3<float> aPosition, math::vec2<float> aSize);
		// system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const property& aProperty, math::vec2<int> aPosition, math::vec2<int> aResolution);
		~system_window();

		// -------------------- object_t methods -------------------- //

		virtual void set_position(math::vec3<float> aPosition) override;

		// -------------------- render_target methods --------------- //

		virtual int id() override;

		// -------------------- window methods ---------------------- //

		virtual void set_size(math::vec2<float> aSize) override;
		virtual void set_resolution(math::vec2<uint> aResolution) override;

		// -------------------- native methods ---------------------- //

		void set_position(math::vec2<int> aPosition);
		void set_size(math::vec2<int> aSize);
		void set_option(setting::id, bool aValue);

	protected:

		// -------------------- object_t methods -------------------- //

		virtual void update(double aDeltaTime) override;

		//virtual VkSubmitInfo transfer() override;

		//virtual VkSubmitInfo compute() override;
		
		// -------------------- render_target methods --------------- //

		//virtual std::vector<VkSubmitInfo> render(stage_t* aStage) override;

		virtual VkResult next_frame() override;
		//virtual std::vector<gcl::command_list> draw(const std::vector<object_t*>& aObject) override;
		virtual VkPresentInfoKHR present_frame() override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //


	private:

		math::vec2<int>							PositionVSC;
		math::vec2<int>							SizeVSC;
		system_display*							Display;
		GLFWwindow*								Window;
		VkSurfaceKHR							Surface;
		VkPipelineStageFlags					PipelineStageFlags;
		VkResult								PresentationResult;
		VkSemaphore								RenderWait;
		VkSemaphore								RenderComplete;
		VkSwapchainCreateInfoKHR				CreateInfo;
		gcl::swapchain*							Swapchain;
		VkPresentInfoKHR						PresentInfo;
		std::vector<gcl::command_list>			PreRenderOperations;
		std::vector<gcl::command_list>			PostRenderOperations;

		// ------------------------------ Utility (Internal, Do Not Use) ------------------------------ //

		// Render Target Side.
		VkResult create_system_window(gcl::context* aContext, system_display* aSystemDisplay, const char* aName, const create_info& aCreateInfo, math::vec2<int> aPosition, math::vec2<int> aResolution);
		
		// ---------- system_window resources ---------- //

		int create_glfw_window(engine* aEngine, window::setting aSetting, int aWidth, int aHeight, const char* aTitle, system_display* aDisplay);
		VkResult create_vulkan_surface(engine* aEngine, gcl::context* aContext, GLFWwindow* aWindow, VkSurfaceKHR aSurface);
		VkResult create_semaphores();
		VkResult create_images();
		VkResult create_cscbs();

		// ---------- system_window renderer ---------- //

		VkResult create_render_pass();
		VkResult create_pipelines();

		VkResult recreate_swapchain(int aFrameSizeX, int aFrameSizeY);

		void clear();
		void zero_out();

		// ------------------------- Backend ------------------------- //

		struct glfwargs {
			window::setting Property;
			int Width;
			int Height;
			const char* Title;
			glfw_monitor* Monitor;
			GLFWwindow* Window;
		};

		static bool initialize();
		static void terminate();
		static void poll_events();

		// Signals to update thread to create window handle
		// Needed backend for system window creation
		static std::thread::id MainThreadID;
		static std::atomic<bool> SignalCreate;
		static std::atomic<bool> WindowCreated;
		static glfwargs WindowTempData;
		static GLFWwindow* ReturnWindow;
		static std::atomic<GLFWwindow*> DestroyWindow;

		// This is necessary for main thread to create window handles.
		static GLFWwindow* create_window_handle(core::object::window::setting aProperty, int aWidth, int aHeight, const char* aTitle, glfw_monitor* aMonitor, GLFWwindow* aWindow); 

		static void destroy_window_handle(GLFWwindow* aWindow);
		// This function is by the engine update thread to create and destroy handles.
		static void process_window_handle_call();

		// ------------------------------ Callbacks (Internal, Do Not Use) ------------------------------ //

		// Window Callbacks
		static void position_callback(GLFWwindow* ContextHandle, int PosX, int PosY);
		static void size_callback(GLFWwindow* ContextHandle, int SizeX, int SizeY);
		static void close_callback(GLFWwindow* ContextHandle);
		static void refresh_callback(GLFWwindow* ContextHandle);
		static void focus_callback(GLFWwindow* ContextHandle, int Focused);
		static void iconify_callback(GLFWwindow* ContextHandle, int Iconified);
		static void maximize_callback(GLFWwindow* ContextHandle, int Maximized);
		static void content_scale_callback(GLFWwindow* ContextHandle, float XScale, float YScale);

		// Framebuffer Callbacks
		static void framebuffer_size_callback(GLFWwindow* ContextHandle, int FrameSizeX, int FrameSizeY);

		// Cursor Callback
		static void mouse_button_callback(GLFWwindow* ContextHandle, int Button, int Action, int Mods);
		static void cursor_position_callback(GLFWwindow* ContextHandle, double PosX, double PosY);
		static void cursor_enter_callback(GLFWwindow* ContextHandle, int Entered);
		static void scroll_callback(GLFWwindow* ContextHandle, double OffsetX, double OffsetY);

		// Keyboard Callback
		static void key_callback(GLFWwindow* ContextHandle, int Key, int Scancode, int Action, int Mods);
		static void character_callback(GLFWwindow* ContextHandle, uint Codepoint);

		// File drop
		static void file_drop_callback(GLFWwindow* ContextHandle, int PathCount, const char** Path);


	};

}

#endif // !GEODESUKA_CORE_OBJECT_SYSTEM_WINDOW_H
