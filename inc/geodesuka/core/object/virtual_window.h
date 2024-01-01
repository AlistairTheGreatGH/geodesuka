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

		struct create_info {
			window::setting 		Window;
			float 					FrameRate;
			int 					FrameCount;
			gcl::image::format 		PixelFormat;
		};

		static constexpr int ID = 3;

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		virtual int id() override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

		virtual_window(gcl::context* aContext, stage_t* aStage, const char* aName, const create_info& aCreateInfo, math::vec2<int> aPosition, math::vec2<int> aSize);

		~virtual_window();

	protected:

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	};

}

#endif // !GEODESUKA_CORE_OBJECT_VIRTUAL_WINDOW_H
