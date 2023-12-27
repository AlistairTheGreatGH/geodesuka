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

	std::vector<std::vector<VkSubmitInfo>> app::gather_transfer_operations() {
		std::vector<std::vector<VkSubmitInfo>> Transfer(Engine->Context.count());

		// Gather all transfer operations from each unique object.
		object_list Object;
		for (size_t i = 0; i < Stage.count(); i++) {
			Object |= Stage[i]->Object;
		}

		// Gather all transfer operations from each stage.
		for (size_t i = 0; i < Engine->Context.count(); i++) {
			for (size_t j = 0; j < this->Stage.count(); j++) {
				Transfer;
			}
		}

		return Transfer;
	}

	std::vector<std::vector<VkSubmitInfo>> app::gather_compute_operations() {
		std::vector<std::vector<VkSubmitInfo>> Compute(Engine->Context.count());

		// Gather all compute operations from each unique object.

		// Gather all compute operations from each stage.

		return Compute;
	}

	std::vector<std::vector<VkSubmitInfo>> app::gather_graphics_and_compute_operations() {
		std::vector<std::vector<VkSubmitInfo>> GraphicsAndCompute(Engine->Context.count());

		for (size_t i = 0; i < Engine->Context.count(); i++) {
			for (size_t j = 0; j < this->Stage.count(); j++) {
				if (Engine->Context[i] == this->Stage[j]->Context) {
					std::vector<VkSubmitInfo> Submission = this->Stage[j]->render();
					size_t Start = GraphicsAndCompute[i].size();
					GraphicsAndCompute[i].resize(Start + Submission.size());
					for (size_t k = 0; k < Submission.size(); k++) {
						GraphicsAndCompute[i][k + Start] = Submission[k];
					}
				}
			}
		}

		return GraphicsAndCompute;
	}

	std::vector<std::vector<VkPresentInfoKHR>> app::gather_presentation_operations() {
		std::vector<std::vector<VkPresentInfoKHR>> Present(Engine->Context.count());

		// Get all presentation operations per system window.

		return Present;
	}

	//void app::gather_render_operations() {
	//	// Aggregate all render operations from each stage to each context.
	//	for (int i = 0; i < Engine->Context.count(); i++) {
	//		// Go to next context if not ready.
	//		if (!Engine->Context[i]->EnableProcessing.load()) continue;
	//		for (int j = 1; j < Stage.count(); j++) {
	//			// Go to next stage if stage is not ready.
	//			if ((!this->Stage[j]->State == stage_t::state::READY) && (this->Stage[j]->Engine->Context == Engine->Context[i])) {
	//				Engine->Context.Handle[i]->BackBatch[2] += this->Stage[j]->render();
	//			}
	//		}
	//	}
	//}

}
