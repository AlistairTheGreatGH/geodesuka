#pragma once
#ifndef GEODESUKA_CORE_STAGE_DESKTOP_H
#define GEODESUKA_CORE_STAGE_DESKTOP_H

#include "../../config.h"

#include "../gcl/config.h"
#include "../gcl/device.h"
#include "../gcl/context.h"

#include "../object/system_display.h"
#include "../object/system_window.h"

#include "../stage_t.h"
#include "canvas.h"

namespace geodesuka::core::stage {

	class desktop : public stage_t {
	public:

		friend class geodesuka::engine;

		static constexpr int ID = 1;

		~desktop();

		// -------------------- stage_t methods -------------------- //

		virtual int id() override;

		// -------------------- native methods ---------------------- //

	protected:

		// -------------------- stage_t methods -------------------- //

		// -------------------- native methods ---------------------- //

	private:

		gcl::device* Device;

		desktop(engine* aEngine, gcl::device* aParentDevice, object::system_display* aParentDisplay);
		desktop(engine* aEngine, object::system_display* aParentDisplay);

	};

}
#endif // !GEODESUKA_CORE_STAGE_DESKTOP_H
