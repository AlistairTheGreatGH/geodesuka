#include <geodesuka/engine.h>
#include <geodesuka/core/gcl/context.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
//#include <assert.h>

#include <vector>

#include <GLFW/glfw3.h>


namespace geodesuka::core::gcl {

	using namespace util;

	context::context(engine* aEngine, device* aDevice, std::vector<const char*> aLayer, std::vector<const char*> aExtension, std::vector<device::operation> aDesiredOperations) {
		VkResult 								Result = VK_SUCCESS;
		std::vector<int> 					 	QFI(aDesiredOperations.size());
		std::vector<int> 					 	UQFI;
		std::map<int, int> 					 	RQC;
		std::map<int, int> 					 	DQC;
		std::vector<std::vector<float>> 	 	QP;
		std::vector<VkDeviceQueueCreateInfo> 	QCI;
		VkDeviceCreateInfo 					 	CI;

		this->Engine = nullptr;
		this->Device = nullptr;
		this->Handle = VK_NULL_HANDLE;

		if ((aEngine == nullptr) || (aDevice == nullptr)) return;

		this->Engine = aEngine;
		this->Device = aDevice;

		for (size_t i = 0; i < aDesiredOperations.size(); i++) {
			QFI[i] = this->Device->qfi(aDesiredOperations[i]);
		}

		for (int i : QFI) {
			if (i == -1) continue;
			if (UQFI.size() != 0) {
				bool AlreadyExistsInSet = false;
				for (int j : UQFI) {
					AlreadyExistsInSet |= (j == i);
				}
				if (!AlreadyExistsInSet) {
					UQFI.push_back(i);
				}
			}
			else {
				UQFI.push_back(i);
			}
		}

		for (int i : UQFI) {
			RQC[i] = 0;
			for (int j : QFI) {
				if (i == j) {
					RQC[i]++;
				}
			}
			DQC[i] = this->Device->qfc(i);
		}

		QP.resize(UQFI.size());
		for (size_t i = 0; i < UQFI.size(); i++) {
			QP[i].resize(std::min(RQC[UQFI[i]], DQC[UQFI[i]]));
			for (size_t j = 0; j < QP[i].size(); j++) {
				QP[i][j] = 1.0f;
			}
		}

		QCI.resize(UQFI.size());
		for (size_t i = 0; i < UQFI.size(); i++) {
			QCI[i].sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			QCI[i].pNext				= NULL;
			QCI[i].flags				= 0;
			QCI[i].queueFamilyIndex		= UQFI[i];
			QCI[i].queueCount			= QP[i].size();
			QCI[i].pQueuePriorities		= QP[i].data();
		}

		CI.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		CI.pNext						= NULL;
		CI.flags						= 0;
		CI.queueCreateInfoCount			= QCI.size();
		CI.pQueueCreateInfos			= QCI.data();
		CI.enabledLayerCount			= aLayer.size();
		CI.ppEnabledLayerNames			= aLayer.data();
		if (Device->is_extension_list_supported(aExtension)) {
			CI.enabledExtensionCount		= aExtension.size();
			CI.ppEnabledExtensionNames		= aExtension.data();
		}
		else {
			CI.enabledExtensionCount		= 0;
			CI.ppEnabledExtensionNames		= NULL;
		}
		CI.pEnabledFeatures				= &aDevice->Features;

		Result = vkCreateDevice(Device->handle(), &CI, NULL, &this->Handle);
		if (Result == VK_SUCCESS) {
			*Engine << log::message(log::INFO, log::SUCCESS, "Context Creation", log::GEODESUKA, "context", "instance", "Device Context Creation Successful!");
		}
		else {
			*Engine << log::message(Result, "Context Creation", log::GEODESUKA, "context", "instance", "Device Context Creation Failed!");
		}

		for (device::operation aOp : aDesiredOperations) {
			Queue[aOp] = VK_NULL_HANDLE;
		}

		for (const auto& Pair : Queue) {
			// Operation Index in aDesiredOperations list.
			size_t OI = std::distance(aDesiredOperations.begin(), std::find(aDesiredOperations.begin(), aDesiredOperations.end(), Pair.first));
			// Queue Family Index of Desired Operation.
			int i = this->Device->qfi(Pair.first);
			// Total Offset of Queue in Queue Family.
			int TO = -1;
			// Determines the offset of the queue in the queue family.
			for (size_t k = 0; k < QFI.size(); k++) {
				if (i == QFI[k]) {
					TO++;
				}
				// Once index has been found, break.
				if (OI == k) break;
			}
			// Get Queue Index in Queue Family.
			int j = TO % std::min(RQC[i], DQC[i]);
			vkGetDeviceQueue(Handle, i, j, &Queue[Pair.first]);
		}

		for (device::operation Op : aDesiredOperations) {
			CommandPool[Op] = new command_pool(this, 0, Op);
		}

		this->Engine->Context |= this;

	}

	context::~context() {

		this->Engine->Context -= this;

		// Free all memory handles on destruction of context.
		for (size_t i = 0; this->Memory.count(); i++){
			vkFreeMemory(this->Handle, this->Memory[i], NULL);
		}

		// Destroy all fences.
		for (size_t i = 0; i < this->Fence.count(); i++) {
			vkDestroyFence(this->Handle, this->Fence[i], NULL);
		}

		// Destroy all semaphores.
		for (size_t i = 0; i < this->Semaphore.count(); i++){
			vkDestroySemaphore(this->Handle, this->Semaphore[i], NULL);
		}

		for (auto& Pair : CommandPool) {
			delete Pair.second;
			Pair.second = nullptr;
		}

		vkDestroyDevice(this->Handle, NULL);
	}

	void context::operator<<(const log::message aNewLogMessage) {
		*Engine << aNewLogMessage;
	}

	VkCommandBuffer context::create_command_buffer(device::operation aDeviceOperation, VkCommandBufferLevel aLevel) {
		return CommandPool[aDeviceOperation]->allocate(aLevel);
	}

	command_list context::create_command_list(device::operation aDeviceOperation, VkCommandBufferLevel aLevel, uint aCount) {
		return CommandPool[aDeviceOperation]->allocate(aLevel, aCount);
	}

	void context::destroy_command_buffer(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer) {
		CommandPool[aDeviceOperation]->release(aCommandBuffer);
	}

	void context::destroy_command_list(device::operation aDeviceOperation, command_list& aCommandList) {
		CommandPool[aDeviceOperation]->release(aCommandList);
	}

	VkResult context::begin(VkCommandBuffer aCommandBuffer) {
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = 0;
		BeginInfo.pInheritanceInfo = NULL;
		return vkBeginCommandBuffer(aCommandBuffer, &BeginInfo);
	}

	VkResult context::end(VkCommandBuffer aCommandBuffer) {
		return vkEndCommandBuffer(aCommandBuffer);
	}

	VkSemaphore context::create_semaphore() {
		util::list<VkSemaphore> Semaphore = this->create_semaphore(1);
		return Semaphore[0];
	}

	util::list<VkSemaphore> context::create_semaphore(int aCount) {
		VkResult Result = VK_SUCCESS;
		VkSemaphoreCreateInfo CreateInfo{};
		util::list<VkSemaphore> NewSemaphore(aCount);
		CreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = 0;
		for (int i = 0; i < aCount; i++){
			Result = vkCreateSemaphore(this->Handle, &CreateInfo, NULL, &NewSemaphore[i]);
		}
		this->Semaphore |= NewSemaphore;
		return NewSemaphore;
	}

	void context::destroy_semaphore(VkSemaphore aSemaphore) {
		this->destroy_semaphore(util::list<VkSemaphore>(aSemaphore));
	}

	void context::destroy_semaphore(util::list<VkSemaphore> aSemaphoreList) {
		this->Semaphore -= aSemaphoreList;
		for (size_t i = 0; i < aSemaphoreList.count(); i++){
			if (aSemaphoreList[i] != VK_NULL_HANDLE){
				vkDestroySemaphore(this->Handle, aSemaphoreList[i], NULL);
			}
		}
	}

	bool context::does_semaphore_exist(VkSemaphore aSemaphore) {
		return this->Semaphore.exists(aSemaphore);
	}

	VkFence context::create_fence(VkFenceCreateFlags aFenceCreateFlags) {
		util::list<VkFence> Fence = this->create_fence(1, aFenceCreateFlags);
		return Fence[0];
	}

	util::list<VkFence> context::create_fence(int aCount, VkFenceCreateFlags aFenceCreateFlags) {
		VkResult Result = VK_SUCCESS;
		VkFenceCreateInfo CreateInfo{};
		util::list<VkFence> NewFence(aCount);
		CreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		CreateInfo.pNext = NULL;
		CreateInfo.flags = aFenceCreateFlags;
		for (int i = 0; i < aCount; i++) {
			Result = vkCreateFence(this->Handle, &CreateInfo, NULL, &NewFence[i]);
		}
		this->Fence |= NewFence;
		return NewFence;
	}

	void context::destroy_fence(VkFence aFence) {
		this->destroy_fence(util::list<VkFence>(1, &aFence));
	}

	void context::destroy_fence(util::list<VkFence> aFenceList) {
		this->Fence -= aFenceList;
		for (size_t i = 0; i < aFenceList.count(); i++) {
			if (aFenceList[i] != VK_NULL_HANDLE) {
				vkDestroyFence(this->Handle, aFenceList[i], NULL);
			}
		}
	}

	VkMemoryRequirements context::get_buffer_memory_requirements(VkBuffer aBufferHandle) const {
		VkMemoryRequirements MemoryRequirements;
		vkGetBufferMemoryRequirements(this->Handle, aBufferHandle, &MemoryRequirements);
		return MemoryRequirements;
	}

	VkMemoryRequirements context::get_image_memory_requirements(VkImage aImageHandle) const {
		VkMemoryRequirements MemoryRequirements;
		vkGetImageMemoryRequirements(this->Handle, aImageHandle, &MemoryRequirements);
		return MemoryRequirements;
	}

	// Memory Allocation.
	VkDeviceMemory context::allocate_memory(VkMemoryRequirements aMemoryRequirements, uint aMemoryType) {
		VkResult Result = VK_SUCCESS;
		VkDeviceMemory MemoryHandle = VK_NULL_HANDLE;
		VkMemoryAllocateInfo AllocateInfo{};
		AllocateInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocateInfo.pNext				= NULL;
		AllocateInfo.allocationSize		= aMemoryRequirements.size;
		AllocateInfo.memoryTypeIndex	= this->Device->get_memory_type_index(aMemoryRequirements, aMemoryType);
		Result = vkAllocateMemory(this->Handle, &AllocateInfo, NULL, &MemoryHandle);
		this->Memory |= MemoryHandle;
		return MemoryHandle;
	}

	void context::free_memory(VkDeviceMemory aMemoryHandle) {
		// Check if memory exists in list.
		if (this->Memory.exists(aMemoryHandle)) {
			// If exists; remove, then delete.
			this->Memory -= aMemoryHandle;
			vkFreeMemory(this->Handle, aMemoryHandle, NULL);
		}
	}

	VkResult context::wait(VkFence aFence) {
		std::vector<VkFence> FenceList = { aFence };
		return this->wait(FenceList, true);
	}

	VkResult context::wait(util::list<VkFence> aFenceList, VkBool32 aWaitOnAll) {
		return this->wait(aFenceList.Handle, aWaitOnAll);
	}

	VkResult context::wait(std::vector<VkFence> aFenceList, VkBool32 aWaitOnAll) {
		return vkWaitForFences(this->Handle, aFenceList.size(), aFenceList.data(), aWaitOnAll, UINT64_MAX);
	}

	VkResult context::wait(uint aDeviceOperation) {
		VkResult Result = VK_SUCCESS;
		for (int i = 0; i < 32; i++) {
			uint Operation = aDeviceOperation & (1 << i);
			if ((Operation != 0) && (Queue.count((device::operation)Operation) > 0)) {
				Result = vkQueueWaitIdle(Queue[(device::operation)Operation]);
			}
		}
		return Result;
	}

	VkResult context::reset(std::vector<VkFence> aFenceList) {
		return vkResetFences(this->Handle, aFenceList.size(), aFenceList.data());
	}

	VkResult context::wait_and_reset(std::vector<VkFence> aFenceList, VkBool32 aWaitOnAll) {
		VkResult Result = VK_SUCCESS;
		Result = this->wait(aFenceList, aWaitOnAll);
		Result = this->reset(aFenceList);
		return Result;
	}

	VkResult context::execute(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer, VkFence aFence) {
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.pNext				= NULL;
		SubmitInfo.waitSemaphoreCount	= 0;
		SubmitInfo.pWaitSemaphores		= NULL;
		SubmitInfo.pWaitDstStageMask	= NULL;
		SubmitInfo.commandBufferCount	= 1;
		SubmitInfo.pCommandBuffers		= &aCommandBuffer;
		SubmitInfo.signalSemaphoreCount	= 0;
		SubmitInfo.pSignalSemaphores	= NULL;
		std::vector<VkSubmitInfo> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute(aDeviceOperation, Submission, aFence);
	}

	VkResult context::execute(device::operation aDeviceOperation, const command_list& aCommandList, VkFence aFence) {
		VkSubmitInfo SubmitInfo = aCommandList.build();
		std::vector<VkSubmitInfo> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute(aDeviceOperation, Submission, aFence);
	}

	VkResult context::execute(device::operation aDeviceOperation, const std::vector<gcl::command_list>& aCommandList, VkFence aFence) {
		std::vector<VkSubmitInfo> SubmissionList = convert(aCommandList);
		return this->execute(aDeviceOperation, SubmissionList, aFence);
	}

	VkResult context::execute(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList, VkFence aFence) {
		VkResult Result = VK_SUCCESS;
		return this->execute(device::operation::PRESENT, aSubmissionList, std::vector<VkPresentInfoKHR>(0), aFence);
	}

	VkResult context::execute(const std::vector<VkPresentInfoKHR>& aPresentationList) {
		VkResult Result = VK_SUCCESS;
		return this->execute(device::operation::PRESENT, std::vector<VkSubmitInfo>(0), aPresentationList, VK_NULL_HANDLE);
	}

	VkResult context::execute(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList, const std::vector<VkPresentInfoKHR>& aPresentationList, VkFence aFence) {
		VkResult Result = VK_SUCCESS;

		if ((aSubmissionList.size() == 0) && (aPresentationList.size() == 0)) return Result;

		switch (aDeviceOperation) {
		case device::operation::PRESENT:
			for (size_t k = 0; k < aPresentationList.size(); k++) {
				Result = vkQueuePresentKHR(Queue[aDeviceOperation], &aPresentationList[k]);
			}
			break;
		default:
			Result = vkQueueSubmit(Queue[aDeviceOperation], aSubmissionList.size(), aSubmissionList.data(), aFence);
			break;
		}

		return Result;
	}

	VkResult context::execute_and_wait(device::operation aDeviceOperation, VkCommandBuffer aCommandBuffer) {
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.pNext				= NULL;
		SubmitInfo.waitSemaphoreCount	= 0;
		SubmitInfo.pWaitSemaphores		= NULL;
		SubmitInfo.pWaitDstStageMask	= NULL;
		SubmitInfo.commandBufferCount	= 1;
		SubmitInfo.pCommandBuffers		= &aCommandBuffer;
		SubmitInfo.signalSemaphoreCount	= 0;
		SubmitInfo.pSignalSemaphores	= NULL;
		std::vector<VkSubmitInfo> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute_and_wait(aDeviceOperation, Submission);
	}

	VkResult context::execute_and_wait(device::operation aDeviceOperation, const command_list& aCommandList) {
		VkSubmitInfo SubmitInfo = aCommandList.build();
		std::vector<VkSubmitInfo> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute_and_wait(aDeviceOperation, Submission);
	}

	VkResult context::execute_and_wait(device::operation aDeviceOperation, const std::vector<gcl::command_list>& aCommandBatch) {
		std::vector<VkSubmitInfo> SubmissionList = convert(aCommandBatch);
		return this->execute_and_wait(aDeviceOperation, SubmissionList);
	}

	VkResult context::execute_and_wait(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList) {
		return this->execute_and_wait(aDeviceOperation, aSubmissionList, std::vector<VkPresentInfoKHR>(0));
	}

	VkResult context::execute_and_wait(device::operation aDeviceOperation, const std::vector<VkSubmitInfo>& aSubmissionList, const std::vector<VkPresentInfoKHR>& aPresentationList) {
		VkResult Result = VK_SUCCESS;
		VkFence Fence = this->create_fence();
		Result = this->execute(aDeviceOperation, aSubmissionList, aPresentationList, Fence);
		Result = this->wait(Fence);
		this->destroy_fence(Fence);
		return Result;
	}

	engine* context::parent_engine() {
		return this->Engine;
	}

	device* context::parent_device() {
		return this->Device;
	}

	VkInstance context::inst() {
		return this->Engine->handle();
	}

	device* context::parent() {
		return this->Device;
	}

	VkDevice context::handle() {
		return this->Handle;
	}

}
