#include <geodesuka/engine.h>
#include <geodesuka/core/app.h>

#include <omp.h>
namespace geodesuka::core {

	using namespace gcl;
	using namespace logic;

	app::app(engine* aEngine) {
		this->Engine = aEngine;
		this->TimeStep = 1.0;
		this->Name = "";
		this->Version = { 1, 0, 0 };
	}

	// This is used engine side to generate thread for Application.
	void app::prerun() {
		// App is now ready to be run.
		this->ExitApp.store(false);
		//this->Engine->AppThreadID = std::this_thread::get_id();
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
		for (size_t i = 0; i < Stage.count(); i++) {
			Object |= Stage[i]->Object;
		}

		// Get total free thread count.
		size_t WorkloadThreadCount = omp_get_max_threads() - Engine->Thread.count() - 1;

		// Calculate Workloads For Each Object & Stage
		ObjectWorkload = calculate_workloads(Object.count(), WorkloadThreadCount);
		StageWorkload = calculate_workloads(Stage.count(), WorkloadThreadCount);

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
		for (size_t i = 0; i < Stage.count(); i++) {
			Stage[i]->collision(aDeltaTime);
		}

	}

	std::map<context*, std::vector<VkSubmitInfo>> app::gather_transfer_operations() {
		std::map<context*, std::vector<VkSubmitInfo>> Transfer;
		object_list UniqueObject;

		// Collect a list of unique game objects.
		for (stage_t* Stg : Stage.Handle) {
			UniqueObject |= Stg->Object;
		}

		// Gather all transfer operations from each stage.
		for (context* Ctx : Engine->Context.Handle) {

			Transfer[Ctx] = std::vector<VkSubmitInfo>(0);

			// Gather all transfer operations from each unique object.
			for (object_t* Obj : UniqueObject.Handle) {
				if (Obj->Context == Ctx) {
					Transfer[Ctx].push_back(Obj->transfer());
				}
			}

			// Gather all transfer operations from each stage.
			for (stage_t* Stg : Stage.Handle) {
				if (Stg->Context == Ctx) {
					Transfer[Ctx].push_back(Stg->transfer());
				}
			}

		}

		return Transfer;
	}

	std::map<context*, std::vector<VkSubmitInfo>> app::gather_compute_operations() {
		std::map<context*, std::vector<VkSubmitInfo>> Compute;
		object_list UniqueObject;

		// Collect a list of unique game objects.
		for (stage_t* Stg : Stage.Handle) {
			UniqueObject |= Stg->Object;
		}

		// Gather all compute operations from each stage.
		for (context* Ctx : Engine->Context.Handle) {

			Compute[Ctx] = std::vector<VkSubmitInfo>(0);

			// Gather all compute operations from each unique object.
			for (object_t* Obj : UniqueObject.Handle) {
				if (Obj->Context == Ctx) {
					Compute[Ctx].push_back(Obj->compute());
				}
			}

			// Gather all compute operations from each stage.
			for (stage_t* Stg : Stage.Handle) {
				if (Stg->Context == Ctx) {
					Compute[Ctx].push_back(Stg->compute());
				}
			}

		}

		return Compute;
	}

	std::map<context*, std::vector<VkSubmitInfo>> app::gather_graphics_and_compute_operations() {
		std::map<context*, std::vector<VkSubmitInfo>> GraphicsAndCompute;
		return GraphicsAndCompute;
	}

	std::map<context*, std::vector<VkPresentInfoKHR>> app::gather_presentation_operations() {
		std::map<context*, std::vector<VkPresentInfoKHR>> Present;
		return Present;
	}

}
