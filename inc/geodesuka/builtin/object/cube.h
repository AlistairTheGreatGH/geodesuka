#pragma once
#ifndef GEODESUKA_BUILTIN_OBJECT_CUBE_H
#define GEODESUKA_BUILTIN_OBJECT_CUBE_H

/*
// -------------------------------------------------- cube.h -------------------------------------------------- \\
This object should serve as an example of how to extend object_t to create custom objects which can be used
with the game engine. As of right now there are four types of windows that can be drawn to. When object_t is
extended, the derivative class has full control over how the particular derived class is drawn to the target
window.


\\ -------------------------------------------------- cube.h -------------------------------------------------- //
*/

#include <geodesuka/engine.h>

namespace geodesuka::builtin::object {

	class cube : public core::object_t {
	public:

		cube(engine* aEngine, core::gcl::context* aContext, core::stage::scene3d* aScene3D, const char* aName);
		~cube();

	protected:

	private:

	};

}

#endif // !GEODESUKA_BUILTIN_OBJECT_CUBE_H
