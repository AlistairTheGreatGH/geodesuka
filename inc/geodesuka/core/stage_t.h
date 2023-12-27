#pragma once
#ifndef GEODESUKA_CORE_STAGE_T_H
#define GEODESUKA_CORE_STAGE_T_H

//
// ------------------------- stage_t ------------------------- //
// A stage is simply a collection of objects that share the
// same space and therefore the same coordinate vectors. While
// the engine holds all created objects and updates, to be rendered
// they must be in the same coordinate space along with possible
// render targets. 
// 
// A stage is a collection of objects that firstly, share the same space
// which gives meaning to their position vector. Secondly, since they share
// the same space, they can be interpreted to interact with one and another.
//
// It is a assumed that stages and the context they create share the same
// rendering and compute context.

#include <vector>

#include <mutex>

#include "logic/thread_tools.h"

#include "gcl/context.h"

#include "object_t.h"
#include "object_list.h"

namespace geodesuka::core {

	class stage_t {
	public:

		friend class geodesuka::core::app;

		enum state {
			FAILURE = -1,
			CREATION,
			READY,
			DESTRUCTION
		};


		std::mutex						Mutex;
		gcl::context*					Context;
		object_list						Object;


		~stage_t();

		// Runtime discrimination of stages for render
		// target to determine how they wish to render
		// the entire stage. This is to include methods
		// that acknowledge lighting and shadows.
		virtual int id() = 0;

	protected:

		stage_t(engine* aEngine, gcl::context* aContext);

		// This function can be called after all objects and render targets have been created.
		// It will then generate draw commands for each object_t/render_target pair on the stage.
		void generate_render_operations();

		// -------------------- update loop -------------------- //

		// Base class method does nothing, but can be overriden to update stage resources.

		// Host Memory Operations
		virtual void update(double aDeltaTime);
		virtual void collision(double aDeltaTime);

		// Device Context Operations
		virtual VkSubmitInfo transfer();
		virtual VkSubmitInfo compute();
		virtual std::vector<VkSubmitInfo> render();
		virtual std::vector<VkPresentInfoKHR> present();

		// -------------------- render loop -------------------- //

		/*
		* Steps for generic rendering within the geodesuka engine.
		*
		* The stage checks whether or not a render_target needs to be rendered to, either
		* by event triggering, or by time ellapsing. The base class will then call an implementation
		* defined method to determine how a stage will be rendered. Ordering with sky boxes first, and
		* so on.
		*/

		// This is fixed by the engine, and cannot be overriden.
		// will iterate through all render targets to determine whether
		// any of them need to be rendered.

	};

}

#endif // !GEODESUKA_CORE_STAGE_T_H
