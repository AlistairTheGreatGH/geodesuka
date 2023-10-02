#pragma once
#ifndef GEODESUKA_BUILTIN_STAGE_DEMO_CANVAS_H
#define GEODESUKA_BUILTIN_STAGE_DEMO_CANVAS_H

#include <geodesuka/engine.h>

#include <geodesuka/builtin/object/triangle.h>

namespace geodesuka::builtin::stage {

	class demo_canvas : public core::stage::canvas {
	public:

		demo_canvas(core::gcl::context* aContext);
		~demo_canvas();

	private:

	};

}

#endif // !GEODESUKA_BUILTIN_STAGE_DEMO_CANVAS_H
