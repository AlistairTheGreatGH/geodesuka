#pragma once
#ifndef GEODESUKA_BUILTIN_STAGE_DEMO3D_H
#define GEODESUKA_BUILTIN_STAGE_DEMO3D_H

#include <geodesuka/engine.h>

#include <geodesuka/builtin/object/gizmo.h>
#include <geodesuka/builtin/object/cube.h>

namespace geodesuka::builtin::stage {

	class demo3d : public core::stage::scene3d {
	public:

		demo3d(core::gcl::context* aContext);
		~demo3d();

	private:

	};

}

#endif // !GEODESUKA_BUILTIN_STAGE_DEMO3D_H
