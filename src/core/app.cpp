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
		// This where all objects/stages and their state are updated by the engine.
		object_list						Object;
		std::vector<logic::workload>	ObjectWorkload;
		std::vector<logic::workload>	StageWorkload;

		for (size_t i = 0; i < Stage.count(); i++) {
			Object |= Stage[i]->Object;
		}

		ObjectWorkload = calculate_workloads(Object.count(), this->EngineThreadCount);
		StageWorkload = calculate_workloads(Stage.count(), this->EngineThreadCount);
		//CollisionWorkload = logic::calculate_workloads(Stage.count(), this->WorkerThreadCount);

		// Parallel Work For Each Object & Stage
		#pragma omp parallel
		{
			int idx = omp_get_thread_num();
			// Determine Workloads Regions For Each Thread.
			size_t LocalStartObject = ObjectWorkload[idx].Offset;
			size_t LocalEndObject = ObjectWorkload[idx].Offset + ObjectWorkload[idx].Size;
			size_t LocalStartStage = StageWorkload[idx].Offset;
			size_t LocalEndStage = StageWorkload[idx].Offset + StageWorkload[idx].Size;

			// Update All Stage Specific Qualities.
			for (size_t i = LocalStartObject; i < LocalEndObject; i++) {
				//this->Object[i]->update(TimeStep);
			}

			// Update All Stage Specific Qualities.
			for (size_t i = LocalStartStage; i < LocalEndStage; i++) {
				this->Stage[i]->update(aDeltaTime);

				this->Stage[i]->collision_setup();
			}

		}

		// Calcuate Collision Effects for 2D & 3D scene
		for (size_t i = 0; i < Stage.count(); i++) {
			Stage[i]->collision_detection();
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
