#include <geodesuka/engine.h>
//#include <geodesuka/core/object_t.h>
//#include <geodesuka/core/object/system_display.h>

#include <math.h>
#include <string.h>

#include <sstream>

#include <GLFW/glfw3.h>

namespace geodesuka::core::object {

	// ------------------------------ public methods ------------------------------- //
	
	system_display::system_display(engine* aEngine, glfw_monitor* aMonitor, const char* aName) : 
		window(nullptr, nullptr, aName, math::vec3<uint>(1u, 1u, 1u), 0.0f, 0, 0) 
	{
		Engine = aEngine;
		Handle = aMonitor;

		Name = glfwGetMonitorName(Handle);
		Title = glfwGetMonitorName(Handle);
		

		// Gather video modes of monitor.
		int MonitorVideoModeCount = 0;
		const glfw_vidmode *MonitorVideoMode = glfwGetVideoModes(Handle, &MonitorVideoModeCount);

		VideoMode = (glfw_vidmode*)malloc(MonitorVideoModeCount * sizeof(glfw_vidmode));
		if (VideoMode != NULL) {
			memcpy(VideoMode, MonitorVideoMode, MonitorVideoModeCount * sizeof(glfw_vidmode));
		}

		CurrentVideoMode = glfwGetVideoMode(Handle);

		glfwGetMonitorPos(Handle, &PositionVSC.x, &PositionVSC.y);

		// Returns units in mm, converts to meters.
		math::vec2<int> PhysicalSize = math::vec2<int>(0, 0);
		glfwGetMonitorPhysicalSize(Handle, &PhysicalSize.x, &PhysicalSize.y);
		Size.x = (float)(((double)PhysicalSize.x) / 1000.0);
		Size.y = (float)(((double)PhysicalSize.y) / 1000.0);

		// Load resolution
		SizeVSC.x = CurrentVideoMode->width;
		SizeVSC.y = CurrentVideoMode->height;
		FrameResolution.x = CurrentVideoMode->width;
		FrameResolution.y = CurrentVideoMode->height;
		FrameResolution.z = 1u;
		FrameRate = CurrentVideoMode->refreshRate;
		
		std::stringstream StringStream;
		StringStream << "\n\t" << "Size: " << Size.x << "m, " << Size.y << "m";
		StringStream << "\n\t" << "Resolution: " << FrameResolution.x << "px, " << FrameResolution.y << "px";
		*Engine << util::log::message(util::log::INFO, util::log::SUCCESS, "Engine Startup", util::log::GEODESUKA, "system_display", this->Name.Handle, StringStream.str().c_str());
	}

	system_display::~system_display() {

	}

	int system_display::id() {
		return ID;
	}

	// ------------------------------ protected methods ---------------------------- //
	
	// ------------------------------ private methods ------------------------------ //

	//tex:
	// Centers system_display and system_window.
	// $$ \vec{r}_{vsc}^{w} = \vec{r}_{scr}^{w} + \frac{\vec{s}_{scr}^{w}}{2} - \Big(\vec{r}_{scr}^{m} + \frac{\vec{s}_{scr}^{m}}{2} \Big) $$

	//tex:
	// Centers system_display and system_window.
	// $$ \vec{r}_{scr}^{w} = \vec{r}_{vsc}^{w} - \frac{\vec{s}_{scr}^{w}}{2} + \Big(\vec{r}_{scr}^{m} + \frac{\vec{s}_{scr}^{m}}{2} \Big) $$

	math::vec3<float> system_display::convert_to_physical_position(math::vec2<int> aPosition) {
		math::vec2<float> TempPhysicalPosition = this->convert_to_physical(aPosition);
		return math::vec3<float>(TempPhysicalPosition.x, TempPhysicalPosition.y, 0.0f);
	}

	math::vec2<float> system_display::convert_to_physical_size(math::vec2<int> aSize) {
		return this->convert_to_physical(aSize);
	}

	// Takes physical position on display and converts to VSC
	math::vec2<int> system_display::convert_to_vsc_position(math::vec3<float> aPosition) {
		math::vec2<int> R;



		return R;
	}

	// Takes physical size on display and converts to VSC
	math::vec2<int> system_display::convert_to_vsc_size(math::vec2<float> aSize) {
		math::vec2<int> S;
		return S;
	}


	math::vec2<float> system_display::convert_to_physical(math::vec2<int> aVector) {
		math::vec2<float> Physical;
		Physical.x = (((float)aVector.x) / ((float)this->SizeVSC.x)) * this->Size.x;
		Physical.y = (((float)aVector.y) / ((float)this->SizeVSC.y)) * this->Size.y;
		return Physical;
	}

	math::vec2<int> system_display::convert_to_screen(math::vec2<float> aVector) {
		math::vec2<int> Screen;
		Screen.x = (int)round((aVector.x / this->Size.x) * (float)this->SizeVSC.x);
		Screen.y = (int)round((aVector.y / this->Size.y) * (float)this->SizeVSC.y);
		return Screen;
	}

	math::vec2<int> system_display::convert_to_display_position(math::vec2<int> aPosition, math::vec2<int> aSize) {
		math::vec2<int> DisplayPosition;
		DisplayPosition.x = (aPosition.x + (int)round(((float)aSize.x) / 2.0f)) - (this->PositionVSC.x + (int)round(((float)this->SizeVSC.x) / 2.0f));
		DisplayPosition.y = -((aPosition.y + (int)round(((float)aSize.y) / 2.0f)) - (this->PositionVSC.y + (int)round(((float)this->SizeVSC.y) / 2.0f)));
		return DisplayPosition;
	}

	math::vec2<int> system_display::convert_to_global_position(math::vec2<int> aPosition, math::vec2<int> aSize) {
		math::vec2<int> GlobalPosition;
		GlobalPosition.x = (aSize.x - (int)round(((float)aSize.x) / 2.0f)) + (this->PositionVSC.x + round(((float)this->SizeVSC.x) / 2.0f));
		GlobalPosition.y = -((aPosition.y + (int)round(((float)aSize.y) / 2.0f)) - (this->PositionVSC.y + (int)round(((float)this->SizeVSC.y) / 2.0f)));
		return GlobalPosition;
	}


}