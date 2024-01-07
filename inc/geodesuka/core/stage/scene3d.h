#pragma once
#ifndef GEODESUKA_CORE_STAGE_SCENE3D_H
#define GEODESUKA_CORE_STAGE_SCENE3D_H

#include "../object_t.h"
#include "../object/render_target.h"
#include "../object/camera3d.h"
#include "../object/skybox.h"
#include "../object/light.h"

#include "../stage_t.h"

namespace geodesuka::core::stage {

	class scene3d : public stage_t {
	public:

		static constexpr int ID = 4;

		~scene3d();

		// -------------------- stage_t methods -------------------- //

		virtual int id() override;

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- stage_t methods -------------------- //

		// -------------------- native methods ---------------------- //

		object::skybox* 				Skybox;
		std::vector<object::light*> 	Light;
		std::vector<object::camera3d*> 	Camera;

		virtual core::object::render_target::render_info render() override;

		scene3d(gcl::context* aContext);

	};

}

#endif // !GEODESUKA_CORE_STAGE_SCENE3D_H
