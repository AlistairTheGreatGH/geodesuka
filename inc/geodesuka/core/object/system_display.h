#pragma once
#ifndef GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H
#define GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H

#include "../object_t.h"
#include "window.h"

struct GLFWmonitor;
struct GLFWvidmode;

typedef GLFWmonitor glfw_monitor;
typedef GLFWvidmode glfw_vidmode;

namespace geodesuka::core::object {

	class system_display : public window {
	public:

		friend class geodesuka::engine;
		friend class system_window;

		static constexpr int ID = 1;

		// TODO: Move this constructor to private.
		system_display(engine *aEngine, glfw_monitor* aMonitor, const char* aName);
		~system_display();

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		virtual int id() override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	private:

		glfw_monitor* Handle;
		//VkDisplayKHR Handle;

		int VideoModeCount;
		glfw_vidmode* VideoMode;
		const glfw_vidmode* CurrentVideoMode;

		// Used internally to interact with OS size and positioning.
		math::vec2<int> PositionVSC;
		math::vec2<int> SizeVSC;

		math::vec3<float> convert_to_physical_position(math::vec2<int> aPosition);
		math::vec2<float> convert_to_physical_size(math::vec2<int> aSize);
		math::vec2<int> convert_to_vsc_position(math::vec3<float> aPosition);
		math::vec2<int> convert_to_vsc_size(math::vec2<float> aSize);

		math::vec2<float> convert_to_physical(math::vec2<int> aVector);
		math::vec2<int> convert_to_screen(math::vec2<float> aVector);

		// Converts GLFW screen coordinates to local display coordinates.
		math::vec2<int> convert_to_display_position(math::vec2<int> aPosition, math::vec2<int> aSize);
		math::vec2<int> convert_to_global_position(math::vec2<int> aPosition, math::vec2<int> aSize);

	};

}

#endif // !GEODESUKA_CORE_OBJECT_SYSTEM_DISPLAY_H
