#include <geodesuka/engine.h>
#include <geodesuka/core/app.h>

#include <omp.h>

namespace geodesuka::core {

	using namespace gcl;
	using namespace logic;
	using namespace util;

	app::app(engine* aEngine, double aTimeStep, const char* aName, version aVersion) {
		this->Engine 	= aEngine;
		this->TimeStep 	= aTimeStep;
		this->Name 		= aName;
		this->Version 	= aVersion;
	}

	// This is used engine side to generate thread for Application.
	void app::prerun() {
		// Thread Launch
		this->Engine->Log << log::message(log::INFO, log::SUCCESS, "Thread Startup", log::GEODESUKA, "engine", this->Name, "Terminal Thread Initiated!");
		// App is now ready to be run.
		this->ExitApp.store(false);
		// Initializes game loop.
		this->run();
		// Forces all threads to finish.
		this->Engine->ThreadController.terminate_all();
	}

	void app::update(double aDeltaTime) {
		
		object_list						Object;
		std::vector<logic::workload>	ObjectWorkload;
		std::vector<logic::workload>	StageWorkload;

		// Get all unique objects from each stage.
		for (stage_t* Stg : Stage.Handle) {
			Object |= Stg->Object;
		}

		// Calculate Workloads For Each Object & Stage
		ObjectWorkload = calculate_workloads(Object.count(), omp_get_max_threads());
		StageWorkload = calculate_workloads(Stage.count(), omp_get_max_threads());

		// Parallel Work For Each Object & Stage
		#pragma omp parallel
		{
			// Get Thread Index
			// Determine Workloads Regions For Each Thread.
			int idx = omp_get_thread_num();

			// Update All Stage Specific Qualities.
			size_t ObjectStartingIndex = ObjectWorkload[idx].Offset;
			size_t ObjectEndingIndex = ObjectWorkload[idx].Offset + ObjectWorkload[idx].Size;
			for (size_t i = ObjectStartingIndex; i < ObjectEndingIndex; i++) {
				Object[i]->update(aDeltaTime);
			}

			// Update All Stage Specific Qualities.
			size_t StageStartingIndex = StageWorkload[idx].Offset;
			size_t StageEndingIndex = StageWorkload[idx].Offset + StageWorkload[idx].Size;
			for (size_t i = StageStartingIndex; i < StageEndingIndex; i++) {
				Stage[i]->update(aDeltaTime);
			}

		}

		// Calcuate Collision Effects for 2D & 3D scene
		for (stage_t* Stg : Stage.Handle) {
			Stg->collision(aDeltaTime);
		}

	}

	std::map<context*, std::vector<VkSubmitInfo>> app::gather_transfer_operations() {
		std::map<context*, std::vector<VkSubmitInfo>> TSI;
		object_list UO;

		// Collect a list of unique game objects.
		for (stage_t* Stg : Stage.Handle) {
			UO |= Stg->Object;
		}

		// Gather all transfer operations from each stage.
		for (context* Ctx : Engine->Context.Handle) {
			TSI[Ctx] = std::vector<VkSubmitInfo>(0);

			// Gather all transfer operations from each unique object.
			for (object_t* Obj : UO.Handle) {
				if (Obj->Context == Ctx) {
					TSI[Ctx].push_back(Obj->transfer());
				}
			}

			// Gather all transfer operations from each stage.
			for (stage_t* Stg : Stage.Handle) {
				if (Stg->Context == Ctx) {
					TSI[Ctx].push_back(Stg->transfer());
				}
			}

		}

		return TSI;
	}

	std::map<context*, std::vector<VkSubmitInfo>> app::gather_compute_operations() {
		std::map<context*, std::vector<VkSubmitInfo>> CSI;
		object_list UO;

		// Collect a list of unique game objects.
		for (stage_t* Stg : Stage.Handle) {
			UO |= Stg->Object;
		}

		// Gather all compute operations from each stage.
		for (context* Ctx : Engine->Context.Handle) {
			CSI[Ctx] = std::vector<VkSubmitInfo>(0);

			// Gather all compute operations from each unique object.
			for (object_t* Obj : UO.Handle) {
				if (Obj->Context == Ctx) {
					CSI[Ctx].push_back(Obj->compute());
				}
			}

			// Gather all compute operations from each stage.
			for (stage_t* Stg : Stage.Handle) {
				if (Stg->Context == Ctx) {
					CSI[Ctx].push_back(Stg->compute());
				}
			}

		}

		return CSI;
	}

	std::map<gcl::context*, app::render_info> app::render() {
		std::map<gcl::context*, render_info> ARI;

		for (context* Ctx : Engine->Context.Handle) {
			ARI[Ctx].GraphicsAndCompute = std::vector<VkSubmitInfo>(0);
			ARI[Ctx].Presentation = std::vector<VkPresentInfoKHR>(0);
			for (stage_t* Stg : Stage.Handle) {
				if (Stg->Context == Ctx) {
					object::render_target::render_info SRI = Stg->render();
					ARI[Ctx].GraphicsAndCompute.insert(ARI[Ctx].GraphicsAndCompute.end(), SRI.SubmitInfo.begin(), SRI.SubmitInfo.end());
					ARI[Ctx].Presentation.insert(ARI[Ctx].Presentation.end(), SRI.PresentInfo.begin(), SRI.PresentInfo.end());
				}
			}
		}

		return ARI;
	}

}
