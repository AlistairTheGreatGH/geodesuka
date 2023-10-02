#pragma once
#ifndef GEODESUKA_CORE_GCL_CONTEXT_H
#define GEODESUKA_CORE_GCL_CONTEXT_H

#include <vector>
#include <atomic>
#include <mutex>

#include "../util/list.h"
#include "../util/log.h"

#include "config.h"

#include "command_list.h"
#include "device.h"

// context.h
// To run anything on a gpu, one must create a device execution 
// context for that device. A device context is neccessary for 
// the manipulation of all resources held on that device, under 
// the command of the host.
//

namespace geodesuka::core::gcl {

	class command_pool;

	class context {
	public:

		//friend class geodesuka::core::state;
		friend class geodesuka::engine;

		context(engine* aEngine, device* aDevice, util::list<const char*> aLayer, util::list<const char*> aExtension);
		~context();

		// Used to aggregate engine logs.
		void operator<<(const util::log::message aNewLogMessage);

		// --------------- Device Context Resource Creation --------------- //

		// Generates command buffers from builtin command pools associated with device context. Otherwise,
		// user can create a custom command pool to manages command buffers.
		VkCommandBuffer create_command_buffer(device::operation aDeviceOperation, VkCommandBufferLevel aLevel);
		command_list create_command_list(device::operation aDeviceOperation, VkCommandBufferLevel aLevel, uint aCount);
		void destroy_command_buffer(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer);
		void destroy_command_list(device::operation aDeviceOperation, command_list& aCommandList);

		// This series of methods are for the creation and destruction of semaphores for the synchronization
		// between command_lists and VkSubmitInfo structures.
		VkSemaphore create_semaphore();
		util::list<VkSemaphore> create_semaphore(int aCount);
		void destroy_semaphore(VkSemaphore aSemaphore);
		void destroy_semaphore(util::list<VkSemaphore> aSemaphoreList);

		// This series of methods are for the creation and destruction of fences for the synchronization
		// between execution of VkSubmitInfos and Presentation.
		VkFence create_fence(VkFenceCreateFlags aFenceCreateFlags = 0);
		util::list<VkFence> create_fence(int aCount, VkFenceCreateFlags aFenceCreateFlags = 0);
		void destroy_fence(VkFence aFence);
		void destroy_fence(util::list<VkFence> aFenceList);

		// Buffer memory requirements.
		// Image memory requirements.
		VkMemoryRequirements get_buffer_memory_requirements(VkBuffer aBufferHandle);
		VkMemoryRequirements get_image_memory_requirements(VkImage aImageHandle);

		// Memory Allocation.
		VkDeviceMemory allocate_memory(size_t aSize, VkMemoryRequirements aMemoryRequirements, device::memory aMemoryType);
		void free_memory(VkDeviceMemory aMemoryHandle);

		// --------------- Execution --------------- //
		
		VkResult execute(device::operation aQFS, VkCommandBuffer aCommandBuffer, VkFence aFence);
		VkResult execute(device::operation aQFS, const command_list& aCommandList, VkFence aFence);
		VkResult execute(device::operation aQFS, const std::vector<gcl::command_list>& aCommandBatch, VkFence aFence);
		VkResult execute(device::operation aQFS, const std::vector<VkSubmitInfo>& aSubmissionList, VkFence aFence);
		VkResult execute(const std::vector<VkPresentInfoKHR>& aPresentationList);
		VkResult execute(device::operation aQFS, const std::vector<VkSubmitInfo>& aSubmissionList, const std::vector<VkPresentInfoKHR>& aPresentationList, VkFence aFence);

		VkResult wait(VkFence aFence, VkBool32 aWaitOnAll);
		VkResult wait(util::list<VkFence> aFenceList, VkBool32 aWaitOnAll);

		bool available(device::operation aOperation);
		int qfi(device::operation aOperation);

		engine* parent_engine();
		device* parent_device();

		VkInstance inst();
		device* parent();
		VkDevice handle();

		std::mutex 			Mutex;

	//private:

		// -------------------- Engine Metadata -------------------- //
		
		// This data is used for engine execution.
		std::mutex 						ExecutionMutex;
		VkFence 						ExecutionFence[3];
		std::vector<VkSubmitInfo> 		BackBatch[3];
		std::vector<VkPresentInfoKHR>	BackPresentBatch;
		std::vector<VkSubmitInfo> 		Submission[3];
		std::vector<VkPresentInfoKHR>	Presentation;

		struct queue_family {
			std::mutex*				Mutex;
			std::vector<float>		Priority;
			std::vector<VkQueue>	Handle;
			queue_family(device::qfp aProperties);
			queue_family(const queue_family& aInput);
			queue_family(queue_family&& aInput) noexcept;
			~queue_family();
			VkQueue& operator[](size_t aIndex);
			queue_family& operator=(const queue_family& aRhs);
			queue_family& operator=(queue_family&& aRhs) noexcept;
			size_t count() const;
		};

		// -------------------- Context Data -------------------- //

		// { T, C, G, GC, P }
		engine* 									Engine;
		device* 									Device;
		std::vector<int> 							QFI;					// Queue Family Index List
		std::vector<int> 							UQFI;					// Unique Queue Family Index List
		VkPhysicalDeviceFeatures 					Features;
		std::vector<VkDeviceQueueCreateInfo>		QueueCreateInfo;
		VkDeviceCreateInfo 							CreateInfo{};
		VkDevice 									Handle;

		// --------------- Managed Resources --------------- //
		
		command_pool*								CommandPool[4];
		util::list<VkSemaphore> 					Semaphore;
		util::list<VkFence> 						Fence;
		util::list<VkDeviceMemory> 					Memory;

		std::vector<queue_family> 					Queue;

		int qfi_to_i(device::operation aOperation);

		void zero_out();

	};

	typedef util::list<context*> context_list;

}

#endif // !GEODESUKA_CORE_GCL_CONTEXT_H
