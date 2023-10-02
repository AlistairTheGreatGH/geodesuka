#pragma once
#ifndef GEODESUKA_CORE_OBJECT_VIRTUAL_WINDOW_H
#define GEODESUKA_CORE_OBJECT_VIRTUAL_WINDOW_H

#include "../math.h"

//#include "../hid/mouse.h"
//#include "../hid/keyboard.h"
//#include "../hid/joystick.h"

#include "../object_t.h"
#include "window.h"

//#include "../graphical/frame_buffer.h"

namespace geodesuka::core::object {

	class virtual_window : public window {
	public:

		static constexpr int ID = 3;

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

	};

}

#endif // !GEODESUKA_CORE_OBJECT_VIRTUAL_WINDOW_H
