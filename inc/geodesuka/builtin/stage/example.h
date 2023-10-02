#pragma once
#ifndef GEODESUKA_BUILTIN_STAGE_EXAMPLE_H
#define GEODESUKA_BUILTIN_STAGE_EXAMPLE_H

#include <geodesuka/engine.h>

#include <geodesuka/builtin/object/triangle.h>

namespace geodesuka::builtin::stage {

	class example : core::stage_t {
	public:

		example(engine* aEngine, core::gcl::context* aContext);
		~example();

	protected:

	private:

		object::triangle* Triangle;
		core::object::system_display* SystemDisplay;
		core::object::system_window* SystemWindow;

	};

}

#endif // !GEODESUKA_BUILTIN_STAGE_EXAMPLE_H
