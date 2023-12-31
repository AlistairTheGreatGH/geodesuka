#pragma once
#ifndef GEODESUKA_ENGINE_H
#define GEODESUKA_ENGINE_H

/* --------------- Standard C Libraries --------------- */

/* --------------- Standard C++ Libraries --------------- */
#include <vector>
#include <map>
#include <chrono>

#include <thread>
#include <mutex>
#include <atomic>

// ----- Core Libraries of the Geodesuka Engine ----- //
#include "config.h"
#include "core/math.h"
#include "core/physics.h"
#include "core/util.h"
#include "core/logic.h"
#include "core/io.h"
#include "core/hid.h"
#include "core/gcl.h"
#include "core/graphics.h"
#include "core/sound.h"
#include "core/object.h"
#include "core/stage.h"
#include "core/app.h"

// The geodesuka engine is an engine that is a multi-threaded high performance engine
// designed for high performance games and simulations. The resources and data the
// engine is responsible for is File management, and the creation and destruction of
// vulkan logical devices, and by consequence all contexts. All files are host memory
// representations of data before being loaded into any particular context's device
// memory. First this prevents double loading of files that are already loaded into
// memory, and secondly, it allows for any object_t or stage_t to reference the 
// same data for the creation of their respective objects. The engine also manages
// all device contexts ever created. This is necessary so that the engine may also 
// process all vulkan commands generated by either user defined applications, user
// defined stages, or user defined objects.
//
//. The two three threads
// that are important to reference are the update thread, and the render thread. The
// update thread is just simply the main thread of the application proper itself. While
// the app.h class is a user defined application/game which is given a thread of its
// own known as the app thread. It is generated from the main/update thread. The reason
// the application-proper main thread becomes the update thread is because when it comes
// to the handling of inputs and windowing events. The creation and destruction of GLFW
// window handles must also be handled by the main thread. 
//
// update thread: The update thread is the application proper main thread (i.e. the thread
// which calls the main function). The reason the application-proper main thread becomes 
// the update thread is because when it comes to the handling of inputs and windowing events. 
// The creation and destruction of GLFW window handles must also be handled by the main thread.
// The update thread is also responsible for iterating through all stages owned by the user
// defined app.h derived class. The user defined derived app class will contain a collection
// of stages which will be iterated through by the update thread. The update thread will 
// perform all update operations to all stages and their respective objects they own. The
// order of opertations in which the update thread is supposed to operate is that:
// 1. Process all waiting window creation and destruction events.
// 2. Process all waiting input events.
// 3. It will update the user defined app, iterating through all stages and objects owned by the app.
// 4. It will then query all Transfer & Compute vulkan commands from all stages & objects.
// 5. It will then execute all aggregate Transfer & Compute vulkan commands.
// The update thread also has timestep enforcement such that if the update thread finishes
// its work early, say before the timestep has been reached, the thread will be put to sleep.
// If it finishes late, it will then use the extended timestep in the update calculations. The
// update thread is not only responsible for updating all stages and objects in time, but it is
// also responsible for performing per-stage physics operations. Examples are rigid body dynamics
// and collisons.
//
// render thread: The render thread is responsible for gathering all vulkan Graphics & Compute
// commands, along with presentation commands to be executed by the vulkan device and context.
//
// app thread: The app thread, not to be confused with the main thread, is the thread which is
// spawned from the main/update thread that will be handed off to the user defined app derived
// class at runtime. The engine (update and render thread) is designed to do most of the heavy 
// lifting, while the app thread is left for the user app to use for custom logic.
//

namespace geodesuka {

	class engine {
	public:

		// So Engine internals can be accessed.
		friend class core::gcl::context;
		friend class core::object_t;
		friend class core::object::system_window;
		friend class core::stage_t;
		friend class core::app;

		enum state {
			FAILURE = -1,
			CREATION,						// Engine instance is being constructed.
			READY,							// Engine instance is active, and ready to be used.
			RUNNING,						// Threads are launched and running backend.
			DESTRUCTION						// Engine is currently in destruction phase.
		};

		core::util::string 									Name;
		core::util::log										Log;
		core::util::string									WorkingDirectory;

		core::object::system_terminal*						SystemTerminal;
		core::gcl::device*									PrimaryDevice;
		core::object::system_display*						PrimaryDisplay;

		core::util::list<core::gcl::device*> 				Device;
		core::util::list<core::object::system_display*>		Display;

		std::atomic<state>									State;
		core::io::file_manager								File;
		core::gcl::context_list								Context;

		core::logic::thread_controller						ThreadController;
		core::util::list<std::thread::id>					ThreadID;
		core::util::list<std::thread*>						Thread;
		//size_t 												WorkerCount;

		engine(int aCmdArgCount, const char** aCmdArgList, int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList);
		~engine();

		// Returns true if input is gathered. Returns false if empty line.
		void operator>>(core::util::string& aRhs);

		// Prints output to terminal.
		void operator<<(core::util::string& aRhs);

		// Same as above, but good for string literals.
		void operator<<(const char* aRhs);

		// Used to aggregate engine logs.
		void operator<<(const core::util::log::message aNewLogMessage);

		// TODO: Reference Pools?
		//const char* get_process_directory();
		core::io::file* open(const char* aFilePath);
		void close(core::io::file* aFile);

		VkInstance handle();
		core::version get_version();
		int get_date();

 		bool is_ready();
		int run(core::app* aApp);
		int return_value();

	private:

		const core::version								Version = { 
			GEODESUKA_ENGINE_VERSION_MAJOR, 
			GEODESUKA_ENGINE_VERSION_MINOR, 
			GEODESUKA_ENGINE_VERSION_PATCH 
		};
		const int										Date = GEODESUKA_ENGINE_VERSION_BIRTH_DATE;
		std::vector<const char*>						Layer;
		std::vector<const char*>						Extension;
		VkApplicationInfo								AppInfo{};
		VkInstanceCreateInfo							CreateInfo{};
		VkInstance										Handle;

		static bool 									ThirdPartyLibrariesInitialized;
		static int 										EngineInstanceCount;

		// Initialize and terminate third party libraries.
		bool initialize_third_party_libraries();
		void terminate_third_party_libraries();

		// Initializes vulkan instance for engine.
		bool initialize_vulkan_instance(int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList);
		void destroy_vulkan_instance();

		// Initialize all engine resources.
		bool initialize_engine_resources();
		void destroy_engine_resources();

		// ------------------------------ Back end runtime ------------------------------ //

		void update(core::app* aApp);		// Updates all unique objects, and then stages.
		void render(core::app* aApp);		// Thread honors frame rates of respective targets.
		//void audio(core::app* aApp);		// Thread Handles audio streams.
		void terminal(core::app* aApp);		// Thread handles terminal input to the engine.

	};

}

#endif // !GEODESUKA_ENGINE_H
