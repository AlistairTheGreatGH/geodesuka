#pragma once
#ifndef GEODESUKA_CORE_STAGE_CANVAS_H
#define GEODESUKA_CORE_STAGE_CANVAS_H

#include <vector>

#include "../object_t.h"
#include "../object/render_target.h"
#include "../object/window.h"

#include "../stage_t.h"

namespace geodesuka::core::stage {

	class canvas : public stage_t {
	public:

		static constexpr int ID = 2;

		canvas(gcl::context* aContext, object::window* aWindow);
		~canvas();

		// -------------------- stage_t methods -------------------- //

		virtual int id() override;

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- stage_t methods -------------------- //

		// -------------------- native methods ---------------------- //

		canvas(gcl::context* aContext);

		//virtual batch render() override;

	private:

		std::vector<object::window*> Window;

		virtual std::vector<VkSubmitInfo> render() override;

	};

}

#endif // !GEODESUKA_CORE_STAGE_CANVAS_H
