#pragma once
#ifndef GEODESUKA_BUILTIN_APP_UNIT_TEST_H
#define GEODESUKA_BUILTIN_APP_UNIT_TEST_H

#include <geodesuka/engine.h>

#include <geodesuka/builtin/object/triangle.h>
#include <geodesuka/builtin/object/quad.h>
#include <geodesuka/builtin/object/cube.h>
#include <geodesuka/builtin/object/gizmo.h>

#include <geodesuka/builtin/stage/demo_canvas.h>
#include <geodesuka/builtin/stage/demo2d.h>
#include <geodesuka/builtin/stage/demo3d.h>

// unit_test.h
// This class is derived from app to test the core
// libraries of the geodesuka engine.
//

namespace geodesuka::builtin::app {

	class unit_test : public core::app {
	public:

		unit_test(engine* aEngine);
		~unit_test();

		virtual void run() override;

	private:

		core::gcl::context* 			Context;		// Device Context which will be used for all graphics and computation operations.
		core::object::system_display* 	Display;		// Primary Display.
		core::object::system_window* 	Window;			// Main Application Window.

		core::stage::canvas* 			Compositor;
		core::stage::scene3d* 			Demo3D;

		void math_unit_test();
		void util_unit_test();
		void logic_unit_test();
		void io_unit_test();
		void hid_unit_test();
		void gcl_unit_test();
		void object_unit_test();
		void stage_unit_test();
		void app_unit_test();

	};

}

#endif // !GEODESUKA_BUILTIN_APP_UNIT_TEST_H
