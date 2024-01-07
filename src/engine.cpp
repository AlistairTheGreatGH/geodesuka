#include <geodesuka/engine.h>

/* --------------- Standard C Libraries --------------- */
#include <cstdlib>
#include <cstring>
#include <cassert>

/* --------------- Standard C++ Libraries --------------- */

#include <sstream>
#include <vector>
#include <map>
#include <chrono>

/* --------------- Third Party Libraries --------------- */

#include <omp.h>

//#include <portaudio.h>

#include <GLFW/glfw3.h>

namespace geodesuka {

	bool engine::ThirdPartyLibrariesInitialized = false;
	int engine::EngineInstanceCount = 0;

	using namespace core;

	using namespace util;
	using namespace logic;
	using namespace io;
	using namespace hid;
	using namespace gcl;
	using namespace graphics;
	using namespace object;
	using namespace stage;

	engine::engine(int aCmdArgCount, const char** aCmdArgList, int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList) {
		bool isVulkanReady				= false;
		bool isEngineResourcesReady 	= false;

		EngineInstanceCount				+= 1;
		this->Name 						= "Geodesuka Engine";
		this->Handle					= VK_NULL_HANDLE;

		// --------------- Third Party Library Initialization Process --------------- //
		// Initializes all third party libraries the geodesuka engine relies on. This
		// is done on a per-process basis, so it is treated globally.

		if (!ThirdPartyLibrariesInitialized) {
			ThirdPartyLibrariesInitialized = this->initialize_third_party_libraries();
			if (ThirdPartyLibrariesInitialized) {
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Third Party Libraries: Initialization Success!");
			}
			else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Third Party Libraries: Initialization Failure!");
			}
		} else {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Third Party Libraries: Already Initialized!");
		}

		// --------------- Vulkan Initialization Process --------------- //
		// Creates a Vulkan instance for Graphics & Computation operations
		// to be performed.

		// (Vulkan) Load required window extensions.
		if (ThirdPartyLibrariesInitialized) {
			isVulkanReady = this->initialize_vulkan_instance(aLayerCount, aLayerList, aExtensionCount, aExtensionList);
		}

		// --------------- Engine Resource Initialization Process --------------- //
		// Initialize all starting engine resources such as desktop stages, system_displays
		// and system_terminal.

		// APIs initialized, query for hardware options.
		if (ThirdPartyLibrariesInitialized && isVulkanReady) {
			isEngineResourcesReady = this->initialize_engine_resources();
			if (isEngineResourcesReady) {
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Engine Resources: Initialization Success!");
			} else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Engine Resources: Initialization Failure!");
			}
		}

		// Is ready if startup condition = success.
		this->State = ThirdPartyLibrariesInitialized && isVulkanReady && isEngineResourcesReady ? state::READY : state::FAILURE;
		if (this->State == state::READY) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Geodesuka Engine: Startup Successful!");
		} else {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Geodesuka Engine: Startup Failure!");
		}
	}

	engine::~engine() {

		EngineInstanceCount -= 1;

		this->State = state::DESTRUCTION;

		this->destroy_engine_resources();

		this->destroy_vulkan_instance();

		if (EngineInstanceCount == 0) {
			this->terminate_third_party_libraries();
		}

	}

	void engine::operator>>(core::util::string& aRhs) {
		*this->SystemTerminal >> aRhs;
	}

	void engine::operator<<(core::util::string& aRhs) {
		*this->SystemTerminal << aRhs;
	}

	void engine::operator<<(const char* aRhs) {
		*this->SystemTerminal << aRhs;
	}

	void engine::operator<<(const core::util::log::message aNewLogMessage) {
		// Add new log to engine log.
		this->Log << aNewLogMessage;
	}

	file* engine::open(const char* aFilePath) {
		return this->File.open(aFilePath);
	}

	void engine::close(file* aFile) {

	}

	VkInstance engine::handle() {
		return Handle;
	}

	core::version engine::get_version() {
		return Version;
	}

	int engine::get_date() {
		return Date;
	}

	bool engine::is_ready() {
		return (this->State == state::READY);
	}

	int engine::return_value() {
		*this->SystemTerminal << this->Log;
		return this->Log[this->Log.count() - 1].Code;
	}

	bool engine::initialize_third_party_libraries() {
		//PaError PortAudioErrorCode 		= paNoError;
		bool isGLSLANGReady				= false;
		bool isFreeTypeReady			= false;
		bool isFreeImageReady			= false;
		bool isAssimpReady				= false;
		bool isPortAudioReady			= false;
		bool isGLFWReady				= false;

		// glslang - SPIRV compiler
		isGLSLANGReady = shader::initialize();
		if (isGLSLANGReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GLSLANG, "engine", this->Name.Handle, "glslang (SPIRV Compiler Tools): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GLSLANG, "engine", this->Name.Handle, "glslang (SPIRV Compiler Tools): Initialization Failure!");
		}

		// freetype - Font Loading Library
		isFreeTypeReady = font::initialize();
		if (isFreeTypeReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::FREETYPE, "engine", this->Name.Handle, "FreeType (Font Loading Library): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::FREETYPE, "engine", this->Name.Handle, "FreeType (Font Loading Library): Initialization Failure!");
		}

		// // freeimage - Image Loading Library.
		// isFreeImageReady = image::initialize();
		// if (isFreeImageReady) {
		// 	this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::FREEIMAGE, "engine", this->Name.Handle, "FreeImage (Image Loading Library): Initialization Success!");
		// } else {
		// 	this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::FREEIMAGE, "engine", this->Name.Handle, "FreeImage (Image Loading Library): Initialization Failure!");
		// }

		// Initialize Model Loading Library.
		isAssimpReady = model::initialize();
		if (isAssimpReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::ASSIMP, "engine", this->Name.Handle, "Assimp (Model Loading Library): Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::ASSIMP, "engine", this->Name.Handle, "Assimp (Model Loading Library): Initialization Failure!");
		}

		//PortAudioErrorCode = Pa_Initialize();
		//isPortAudioReady = (PortAudioErrorCode == paNoError);
		//if (isPortAudioReady) {
		//	this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::PORTAUDIO, "engine", this->Name.Handle, "PortAudio (System Audio Interface): Initialization Success!");
		//} else {
		//	this->Log << log::message(PortAudioErrorCode, "Engine Startup", log::PORTAUDIO, "engine", this->Name.Handle, "PortAudio (System Audio Interface): Initialization Failure!");
		//}

		// glfw - operating system window handling.
		isGLFWReady = system_window::initialize();
		if (isGLFWReady) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GLFW, "engine", this->Name.Handle, "GLFW (System Window Interface): Initialization Success!");
		} else {
			const char* ReturnError;
			int ReturnErrorCode = glfwGetError(&ReturnError);
			this->Log << log::message(ReturnErrorCode, "Engine Startup", log::GLFW, "engine", this->Name.Handle, "GLFW (System Window Interface): Initialization Failure!");
			this->Log << log::message(ReturnErrorCode, "Engine Startup", log::GLFW, "engine", this->Name.Handle, ReturnError);
		}

		return (isGLSLANGReady && isFreeTypeReady /*&& isFreeImageReady*/ && isAssimpReady /*&& isPortAudioReady*/ && isGLFWReady);
	}

	void engine::terminate_third_party_libraries() {
		system_window::terminate();

		//Pa_Terminate();

		model::terminate();

		//image::terminate();

		font::terminate();

		shader::terminate();
	}

	bool engine::initialize_vulkan_instance(int aLayerCount, const char** aLayerList, int aExtensionCount, const char** aExtensionList) {

		// Adds WSI instance extensions to list.
		for (size_t i = 0; i < system_window::RequiredInstanceExtension.size(); i++) {
			Extension.push_back(system_window::RequiredInstanceExtension[i]);
		}

		// Adds proposed layers to list.
		for (int i = 0; i < aLayerCount; i++) {
			Layer.push_back(aLayerList[i]);
		}

		// Adds proposed extensions.
		for (int i = 0; i < aExtensionCount; i++) {
			Extension.push_back(aExtensionList[i]);
		}

		//"VK_KHR_display";
		// Does not work on Windows OS right now, only Linux
		//this->Extension.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);		

		AppInfo.sType						= VK_STRUCTURE_TYPE_APPLICATION_INFO;
		AppInfo.pNext						= NULL;
		AppInfo.pApplicationName			= ""; // TODO: Funnel App Name here?
		AppInfo.applicationVersion			= VK_MAKE_VERSION(1, 0, 0);
		AppInfo.pEngineName					= (const char*)this->Name.Handle;
		AppInfo.engineVersion				= VK_MAKE_VERSION(Version.Major, Version.Minor, Version.Revision);
		AppInfo.apiVersion					= VK_MAKE_VERSION(1, 3, 0);

		CreateInfo.sType					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		CreateInfo.pNext					= NULL;
		CreateInfo.flags					= 0;
		CreateInfo.pApplicationInfo			= &AppInfo;
		CreateInfo.enabledLayerCount		= (uint32_t)Layer.size();
		CreateInfo.ppEnabledLayerNames		= Layer.data();
		CreateInfo.enabledExtensionCount	= (uint32_t)Extension.size();
		CreateInfo.ppEnabledExtensionNames	= Extension.data();

		VkResult Result = vkCreateInstance(&CreateInfo, NULL, &Handle);
		if (Result == VK_SUCCESS) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::VULKAN, "engine", this->Name.Handle, "Vulkan (Graphics & Computation API): Initialization Success!");
			return true;
		} else {
			this->Log << log::message(Result, "Engine Startup", log::VULKAN, "engine", this->Name.Handle, "Vulkan (Graphics & Computation API): Initialization Failure!");
			return false;
		}
	}

	void engine::destroy_vulkan_instance() {
		if (this->Handle != VK_NULL_HANDLE) {
			vkDestroyInstance(Handle, NULL);
			this->Handle = VK_NULL_HANDLE;
		}
	}

	bool engine::initialize_engine_resources() {
		// Checklist:
		// DisplayCount > 0
		// DeviceCount > 0
		// At least one device supports TCGP

		bool isSystemDisplayAvailable		= false;
		bool isGCDeviceAvailable			= false;

		this->PrimaryDevice 				= nullptr;
		this->PrimaryDisplay 				= nullptr;

		// Create System Terminal for logging output and getting user commands. 
		this->SystemTerminal = new system_terminal(this, "SystemTerminal");
		if (this->SystemTerminal != nullptr) {
			this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "System Terminal: Initialization Success!");
		} else {
			this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "System Terminal: Initialization Failure!");
			return false;
		}

		// Use GLFW API to query for all physical displays.
		{
			int PhysicalMonitorCount = 0;
			glfw_monitor** PhysicalMonitor = NULL;
			PhysicalMonitor = glfwGetMonitors(&PhysicalMonitorCount);
			if (PhysicalMonitorCount > 0) {
				std::stringstream MainStream;
				MainStream << "System Display Count: " << PhysicalMonitorCount;
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "system_display", "", MainStream.str().c_str());
				this->Display.resize(PhysicalMonitorCount);
				for (int i = 0; i < PhysicalMonitorCount; i++) {
					// Create New Display
					object::system_display* NewDisplay = new system_display(this, PhysicalMonitor[i], "");
					if (NewDisplay == nullptr) {
						this->Log << log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "SystemDisplay: Insuffient Memory for Display Instance!");
						return false;
					}

					// Add to list.
					this->Display[i] = NewDisplay;
					if (PhysicalMonitor[i] == glfwGetPrimaryMonitor()) {
						this->PrimaryDisplay = NewDisplay;
					}
				}
			}
			else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "No System Displays have been detected!");
				return false;
			}
		}

		// Use Vulkan API to query for all Graphics & Computation capable devices.
		{
			uint32_t PhysicalDeviceCount = 0;
			VkResult Result = vkEnumeratePhysicalDevices(this->Handle, &PhysicalDeviceCount, NULL);
			if (PhysicalDeviceCount > 0) {
				std::stringstream MainStream;
				std::vector<VkPhysicalDevice> PhysicalDevice(PhysicalDeviceCount);

				MainStream << "Physical Device Count: " << PhysicalDeviceCount;
				this->Log << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "device", "Device", MainStream.str().c_str());
				Result = vkEnumeratePhysicalDevices(this->Handle, &PhysicalDeviceCount, PhysicalDevice.data());
				this->Device.resize(PhysicalDeviceCount);
				for (uint32_t i = 0; i < PhysicalDeviceCount; i++) {
					this->Device[i] = new device(this, PhysicalDevice[i]);
					if (this->Device[i] == nullptr) {
						this->Log << log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "Insuffient Memory for Physical Device Instance!");
						return false;
					}
				}
			}
			else {
				this->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "engine", this->Name.Handle, "No Graphics & Computation Devices Detected!");
				return false;
			}
		}

		// Determine Primary Device.
		for (size_t i = 0; i < this->Device.count(); i++) {
			if (this->Device[i]->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
				this->PrimaryDevice = this->Device[i];
				break;
			}
		}

		//// Construct Desktop Stages.
		//this->Desktop.resize(this->Display.count());
		//for (size_t i = 0; i < this->Desktop.count(); i++) {
		//	this->Desktop[i] = new desktop(this, (object::system_display*)Display[i]);
		//	this->Display[i]->Stage = this->Desktop[i];
		//}

		return ((Display.count() > 0) && (Device.count() > 0));
	}

	void engine::destroy_engine_resources() {

		//for (int i = 0; i < File.count(); i++) {
		//	delete File[i];
		//	File[i] = nullptr;
		//}

		//File.clear();

		for (int i = 0; i < Device.count(); i++) {
			delete Device[i];
			Device[i] = nullptr;
		}

		delete SystemTerminal;
	}

	// Engine will run designated user defined app.
	int engine::run(app* aApp) {

		State = state::RUNNING;

		// Spawn Core Engine Threads.
		Thread |= new std::thread(&engine::render, this, aApp);
		//Thread |= new std::thread(&engine::audio, this);
		Thread |= new std::thread(&engine::terminal, this, aApp);
		Thread |= new std::thread(&app::prerun, aApp);

		omp_set_num_threads(omp_get_max_threads());

		// Gather Thread ID 
		ThreadID |= std::this_thread::get_id();
		for (size_t i = 0; i < Thread.count(); i++) {
			ThreadID |= Thread[i]->get_id();
		}

		// Launch Engine Main Thread
		this->update(aApp);

		// Delete All Thread Objects
		for (size_t i = 0; i < Thread.count(); i++) {
			Thread[i]->join();
			delete Thread[i];
			Thread[i] = nullptr;
		}
		ThreadID.clear();
		Thread.clear();

		this->State = state::READY;

		return 0;
	}

	// --------------- Engine Main/Update Thread --------------- //
	// This is the main thread of the application proper, and the update 
	// thread of the engine. It updates all engine resources host side, and
	// is responsible for handling user input. The update thread also
	// manages system window objects as only the main thread is capable
	// of handling user input. The update thread has an enforced time step
	// where the thread will attempt to honor the designated time step. 
	// The thread will be put to sleep if it finishes work early, and if
	// it runs late the time difference will increase.
	void engine::update(core::app* aApp) {
		VkResult Result = VK_SUCCESS;
		std::map<context*, std::vector<VkSubmitInfo>> Transfer;
		std::map<context*, std::vector<VkSubmitInfo>> Compute;

		this->Log << log::message(log::INFO, log::SUCCESS, "Thread Startup", log::GEODESUKA, "engine", this->Name.Handle, "Update Thread Initiated!");

		while (ThreadController.cycle(aApp->TimeStep.load())) {
			double dt = aApp->TimeStep.load();

			// Process system_window Creation/Destruction calls.
			system_window::process_window_handle_call();

			// Poll Input Events.
			system_window::poll_events();

			// --------------- Host work is done here --------------- //

			aApp->Mutex.lock();
			
			// Updates all resources owned by user application.
			aApp->update(dt);

			Transfer = aApp->gather_transfer_operations();

			Compute = aApp->gather_compute_operations();

			aApp->Mutex.unlock();

			// --------------- Per Device Context work is done here --------------- //

			for (context* Ctx : Context.Handle) {
				// Lock Context for execution.
				Ctx->Mutex.lock();

				// Wait for other inflight operations to finish.
				Result = Ctx->engine_wait({ device::operation::TRANSFER, device::operation::COMPUTE, device::operation::GRAPHICS_AND_COMPUTE });

				// Execute all transfer device operations.
				Result = Ctx->engine_execute(device::operation::TRANSFER, Transfer[Ctx]);

				// Execute all compute device operations.
				Result = Ctx->engine_execute(device::operation::COMPUTE, Compute[Ctx]);

				// Unlock device context.
				Ctx->Mutex.unlock();
			}

		}

	}

	// --------------- Engine Render Thread --------------- //
	// The render thread of the geodesuka engine is responsible
	// for honoring the designated frame rate of every render_target
	// in existence. There is no enforced time step, but the render
	// thread will iterate through all existing stages and render_targets
	// and check if a render_target is ready to issue render commands.
	void engine::render(core::app* aApp) {
		VkResult Result = VK_SUCCESS;
		std::map<gcl::context*, app::render_info> RenderInfo;

		this->Log << log::message(log::INFO, log::SUCCESS, "Thread Startup", log::GEODESUKA, "engine", this->Name.Handle, "Render Thread Initiated!");

		while (ThreadController.cycle(0.0)) {

			aApp->Mutex.lock();

			RenderInfo = aApp->render();

			aApp->Mutex.unlock();

			// --------------- Per Device Context work is done here --------------- //

			for (context* Ctx : Context.Handle) {
				// Lock Context for execution.
				Ctx->Mutex.lock();

				// Wait for other inflight operations to finish.
				Result = Ctx->engine_wait({ device::operation::TRANSFER, device::operation::COMPUTE, device::operation::GRAPHICS_AND_COMPUTE });

				// Execute all transfer device operations.
				Result = Ctx->engine_execute(device::operation::GRAPHICS_AND_COMPUTE, RenderInfo[Ctx].GraphicsAndCompute);

				// Unlock device context.
				Ctx->Mutex.unlock();

				// Execute all system window presentation operations.
				Result = Ctx->execute(RenderInfo[Ctx].Presentation);
			}

		}

	}

	// --------------- System Terminal Thread --------------- //
	// Will be used for runtime debugging of engine using terminal.
	// --------------- System Terminal Thread --------------- //
	void engine::terminal(core::app* aApp) {

		this->Log << log::message(log::INFO, log::SUCCESS, "Thread Startup", log::GEODESUKA, "engine", this->Name.Handle, "Terminal Thread Initiated!");

		while (ThreadController.cycle(0.0)) {
			util::string Command;

			*SystemTerminal << "Geodesuka Engine>";

			*SystemTerminal >> Command;

			if (Command == "exit") {
				this->ThreadController.terminate_all();
			}
		}

	}

}
