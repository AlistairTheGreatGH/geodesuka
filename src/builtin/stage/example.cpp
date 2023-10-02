#include <geodesuka/builtin/stage/example.h>

#include <geodesuka/engine.h>

namespace geodesuka::builtin::stage {

	example::example(engine* aEngine, core::gcl::context* aContext) : stage_t(aEngine, aContext) {

		/*
		Object |= new cube(Engine, Context, this, "Cube0");
		Object |= new triangle(Engine, Context, this, "Triangle0");
		Object |= new cube(Engine, Context, this, "Cube1");
		Object |= new cube(Engine, Context, this, "Cube2");
		Object |= new camera3d(Engine, Context, this, "Player Camera");

		*/

		/*
		Property.Window.Resizable			= true;
		Property.Window.Decorated			= true;
		Property.Window.UserFocused			= true;
		Property.Window.AutoMinimize		= false;
		Property.Window.Floating			= false;
		Property.Window.Maximized			= false;
		Property.Window.Minimized			= false;
		Property.Window.Visible				= true;
		Property.Window.ScaleToMonitor		= false;
		Property.Window.CenterCursor		= false;
		Property.Window.FocusOnShow			= false;

		Property.Swapchain.FrameCount		= 3;
		Property.Swapchain.FrameRate		= 1.0;
		Property.Swapchain.ColorSpace		= core::object::system_window::colorspace::EXTENDED_SRGB_LINEAR;
		Property.Swapchain.Usage			= core::gcl::image::usage::COLOR_ATTACHMENT;
		Property.Swapchain.CompositeAlpha	= core::object::system_window::composite::ALPHA_OPAQUE;
		Property.Swapchain.PresentMode		= core::object::system_window::present_mode::MAILBOX;
		Property.Swapchain.Clipped			= true;
		Property.PixelFormat				= VK_FORMAT_R16G16B16A16_SFLOAT;
		Property.Position					= math::vec3<float>(0, 0, 0);
		Property.Size						= math::vec2<float>(0.3, 0.3);
		Property.Title						= "Triangle";

		SystemDisplay = Engine->get_primary_display();

		SystemWindow = new core::object::system_window(Engine, Context, SystemDisplay, Property);

		RenderTarget.push_back(SystemWindow);

		Triangle = new object::triangle(aEngine, aContext, this);

		Object.push_back(Triangle);

		isReadyToBeProcessed.store(true);
		*/
	}

	example::~example() {
		//EnableProcessing.store(false);
		delete Triangle;
		delete SystemWindow;
	}



}