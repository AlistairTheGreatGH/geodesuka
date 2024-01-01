#include <geodesuka/engine.h>
#include <geodesuka/core/stage_t.h>

#include <omp.h>

namespace geodesuka::core {

	using namespace logic;

	// Base destructor.
	stage_t::~stage_t() {
		// Generate a list of uniquely owned objects by stage.
		object_list OwnedObject = Object.gather_by(this);

		// Deleted owned objects
		for (int i = 0; i < OwnedObject.count(); i++) {
			delete OwnedObject[i];
			OwnedObject[i] = nullptr;
		}
		OwnedObject.clear();
		Context = nullptr;

	}

	// Base constructor.
	stage_t::stage_t(engine* aEngine, gcl::context* aContext) {
		Context	= aContext;
	}

	void stage_t::update(double aDeltaTime) {}

	void stage_t::collision(double aDeltaTime) {

		// Collision Math Done here.


	}

	VkSubmitInfo stage_t::transfer() {
		VkSubmitInfo TransferBatch{};
		TransferBatch.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		TransferBatch.pNext					= NULL;
		TransferBatch.waitSemaphoreCount	= 0;
		TransferBatch.pWaitSemaphores		= NULL;
		TransferBatch.pWaitDstStageMask		= NULL;
		TransferBatch.commandBufferCount	= 0;
		TransferBatch.pCommandBuffers		= NULL;
		TransferBatch.signalSemaphoreCount	= 0;
		TransferBatch.pSignalSemaphores		= NULL;
		return TransferBatch;
	}

	VkSubmitInfo stage_t::compute() {
		VkSubmitInfo ComputeBatch{};
		ComputeBatch.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		ComputeBatch.pNext					= NULL;
		ComputeBatch.waitSemaphoreCount		= 0;
		ComputeBatch.pWaitSemaphores		= NULL;
		ComputeBatch.pWaitDstStageMask		= NULL;
		ComputeBatch.commandBufferCount		= 0;
		ComputeBatch.pCommandBuffers		= NULL;
		ComputeBatch.signalSemaphoreCount	= 0;
		ComputeBatch.pSignalSemaphores		= NULL;
		return ComputeBatch;
	}

}

