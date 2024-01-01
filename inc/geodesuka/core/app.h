#pragma once
#ifndef GEODESUKA_CORE_APP_H
#define GEODESUKA_CORE_APP_H

#include <vector>
#include <mutex>
#include <atomic>

#include "logic/thread_tools.h"

#include "../config.h"

namespace geodesuka::core {

	struct version {
		int Major;
		int Minor;
		int Revision;
	};

	class app {
	public:

		friend class geodesuka::engine;

		struct render_info {
			std::vector<VkSubmitInfo> GraphicsAndCompute;
			std::vector<VkPresentInfoKHR> Presentation;
		};

		std::atomic<bool>				ExitApp;

		virtual ~app() = default;

	protected:

		const char* 					Name;
		version 						Version;		

		std::mutex						Mutex;
		engine* 						Engine;
		std::atomic<double> 			TimeStep;
		size_t							EngineThreadCount;
		size_t 							AppThreadCount;
		stage_list						Stage;

		app(engine* aEngine, double aTimeStep, const char* aName, version aVersion);

		virtual void run() = 0;

	private:

		void prerun();
		void update(double aDeltaTime);
		std::map<gcl::context*, std::vector<VkSubmitInfo>> gather_transfer_operations();
		std::map<gcl::context*, std::vector<VkSubmitInfo>> gather_compute_operations();
		std::map<gcl::context*, render_info> render();

	};

}

#endif // !GEODESUKA_CORE_APP_H

