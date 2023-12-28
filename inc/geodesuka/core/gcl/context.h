#pragma once
#ifndef GEODESUKA_CORE_GCL_CONTEXT_H
#define GEODESUKA_CORE_GCL_CONTEXT_H

#include <vector>
#include <map>
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

		context(engine* aEngine, device* aDevice, std::vector<const char*> aLayer, std::vector<const char*> aExtension, std::vector<device::operation> aDesiredOperations);
		~context();

		// Used to aggregate engine logs.
		void operator<<(const util::log::message aNewLogMessage);

		// --------------- Device Context Resource Creation --------------- //

		// Generates command buffers from builtin command pools associated with device context. Otherwise,
		// user can create a custom command pool to manages command buffers.
		VkCommandBuffer create_command_buffer(device::operation aDeviceOperation, VkCommandBufferLevel aLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		command_list create_command_list(device::operation aDeviceOperation, VkCommandBufferLevel aLevel, uint aCount);
		void destroy_command_buffer(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer);
		void destroy_command_list(device::operation aDeviceOperation, command_list& aCommandList);

		VkResult begin(VkCommandBuffer aCommandBuffer);
		VkResult end(VkCommandBuffer aCommandBuffer);

		// This series of methods are for the creation and destruction of semaphores for the synchronization
		// between command_lists and VkSubmitInfo structures.
		VkSemaphore create_semaphore();
		util::list<VkSemaphore> create_semaphore(int aCount);
		void destroy_semaphore(VkSemaphore aSemaphore);
		void destroy_semaphore(util::list<VkSemaphore> aSemaphoreList);
		bool does_semaphore_exist(VkSemaphore aSemaphore);

		// This series of methods are for the creation and destruction of fences for the synchronization
		// between execution of VkSubmitInfos and Presentation.
		VkFence create_fence(VkFenceCreateFlags aFenceCreateFlags = 0);
		util::list<VkFence> create_fence(int aCount, VkFenceCreateFlags aFenceCreateFlags = 0);
		void destroy_fence(VkFence aFence);
		void destroy_fence(util::list<VkFence> aFenceList);

		// Buffer memory requirements.
		// Image memory requirements.
		VkMemoryRequirements get_buffer_memory_requirements(VkBuffer aBufferHandle) const;
		VkMemoryRequirements get_image_memory_requirements(VkImage aImageHandle) const;

		// Memory Allocation.
		VkDeviceMemory allocate_memory(VkMemoryRequirements aMemoryRequirements, uint aMemoryType);
		void free_memory(VkDeviceMemory aMemoryHandle);

		// --------------- Execution --------------- //
		
		VkResult wait(VkFence aFence);
		VkResult wait(util::list<VkFence> aFenceList, VkBool32 aWaitOnAll = VK_TRUE);
		VkResult wait(std::vector<VkFence> aFenceList, VkBool32 aWaitOnAll = VK_TRUE);
		VkResult wait(uint aDeviceOperation);
		VkResult reset(std::vector<VkFence> aFenceList);
		VkResult wait_and_reset(std::vector<VkFence> aFenceList, VkBool32 aWaitOnAll = VK_TRUE);
		
		VkResult execute(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer, VkFence aFence = VK_NULL_HANDLE);
		VkResult execute(device::operation aDeviceOperation, const command_list& aCommandList, VkFence aFence = VK_NULL_HANDLE);
		VkResult execute(device::operation aDeviceOperation, const std::vector<gcl::command_list>& aCommandBatch, VkFence aFence = VK_NULL_HANDLE);
		VkResult execute(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList, VkFence aFence = VK_NULL_HANDLE);
		VkResult execute(const std::vector<VkPresentInfoKHR>& aPresentationList);
		VkResult execute(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList, const std::vector<VkPresentInfoKHR>& aPresentationList, VkFence aFence = VK_NULL_HANDLE);

		VkResult execute_and_wait(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer);
		VkResult execute_and_wait(device::operation aDeviceOperation, const command_list& aCommandList);
		VkResult execute_and_wait(device::operation aDeviceOperation, const std::vector<gcl::command_list>& aCommandBatch);
		VkResult execute_and_wait(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList);
		VkResult execute_and_wait(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList, const std::vector<VkPresentInfoKHR>& aPresentationList);

		engine* parent_engine();
		device* parent_device();

		VkInstance inst();
		device* parent();
		VkDevice handle();

	private:

		// -------------------- Context Data -------------------- //

		// { T, C, G, GC, P }
		std::mutex 												Mutex;
		engine* 												Engine;
		device* 												Device;
		VkDevice 												Handle;
		std::map<device::operation, VkQueue> 					Queue;

		// --------------- Managed Resources --------------- //

		std::map<device::operation, command_pool*>				CommandPool;
		util::list<VkSemaphore> 								Semaphore;
		util::list<VkFence> 									Fence;
		util::list<VkDeviceMemory> 								Memory;

	};

	typedef util::list<context*> context_list;

}

#endif // !GEODESUKA_CORE_GCL_CONTEXT_H
