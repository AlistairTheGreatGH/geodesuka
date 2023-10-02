#pragma once
#ifndef GEODESUKA_CORE_OBJECT_CAMERA2D_H
#define GEODESUKA_CORE_OBJECT_CAMERA2D_H

#include "../gcl/context.h"

#include "../object_t.h"
#include "camera.h"

namespace geodesuka::core::object {

	class camera2d : public camera {
	public:

		static constexpr int ID = 4;

		//camera2d(engine* aEngine, gcl::context* aContext, stage::scene2d* aScene2D, const char *aName);
		~camera2d();

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		virtual int id() override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- object_t methods -------------------- //

		// -------------------- render_target methods --------------- //

		//virtual std::vector<VkSubmitInfo> draw(const object_list& aObject) override;

		// -------------------- window methods ---------------------- //

		// -------------------- native methods ---------------------- //

	};

}

#endif // !GEODESUKA_CORE_OBJECT_CAMERA2D_H
