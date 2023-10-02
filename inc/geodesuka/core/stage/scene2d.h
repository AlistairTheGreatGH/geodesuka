#pragma once
#ifndef GEODESUKA_CORE_STAGE_SCENE2D_H
#define GEODESUKA_CORE_STAGE_SCENE2D_H

#include <vector>

#include "../object_t.h"
#include "../object/render_target.h"
#include "../object/camera2d.h"

#include "../stage_t.h"

namespace geodesuka::core::stage {

	class scene2d : public stage_t {
	public:

		static constexpr int ID = 3;

		~scene2d();


		// -------------------- stage_t methods -------------------- //

		virtual int id() override;

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- stage_t methods -------------------- //

		// -------------------- native methods ---------------------- //

		std::vector<object::camera2d*> Camera;

		scene2d(gcl::context* aContext);

		//virtual batch render() override;

	private:

	};

}

#endif // !GEODESUKA_CORE_STAGE_SCENE2D_H
