#pragma once
#ifndef GEODESUKA_CORE_OBJECT_CAMERA_H
#define GEODESUKA_CORE_OBJECT_CAMERA_H

// I'm still concerned how this will fit in as
// an object. It is an object, but is it a window?

// To be or not to be, is camera a type of window, 
// does it just contain a window.

// A camera shouldn't be a window since
// If it direct draws, it should contain windows.
// If it stream draws, it should 

/*

*/

#include <vector>
#include <string>

#include "../math.h"

//#include "../hid/mouse.h"
//#include "../hid/keyboard.h"
//#include "../hid/joystick.h"

//#include "../graphical/frame_buffer.h"

#include "../object_t.h"
#include "render_target.h"

namespace geodesuka::core::object {

	// TODO: Might delete later if no shared qualities.
	class camera : public render_target {
	public:

		~camera();

	protected:

		camera(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount);

	};

}
#endif // !GEODESUKA_CORE_OBJECT_CAMERA_H
