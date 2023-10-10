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

	context::context(engine* aEngine, device* aDevice, util::list<const char*> aLayer, util::list<const char*> aExtension) {
		VkResult Result = VK_SUCCESS;

		// List of operations.
		// Check for support of required extensions requested i
		// 1: Check for extensions.
		// 2: Queue Create Info.
		// 3: Create Logical Device.

		if ((aEngine == nullptr) || (aDevice == nullptr)) return;

		this->Engine = aEngine;
		this->Device = aDevice;

		// Generate a list of supported operations.
		this->QFI[0] = this->Device->qfi(device::operation::TRANSFER);
		this->QFI[1] = this->Device->qfi(device::operation::COMPUTE);
		this->QFI[2] = this->Device->qfi(device::operation::GRAPHICS);
		this->QFI[3] = this->Device->qfi(device::operation::GRAPHICS_AND_COMPUTE);
		this->QFI[4] = this->Device->qfi(device::operation::PRESENT);

		// Compose a set of Unique Queue Family Indices (UQFI), for Device Creation.
		for (size_t i = 0; i < this->QFI.size(); i++) {
			if (QFI[i] == -1 ) continue;
			if (UQFI.size () != 0) {
				bool AlreadyExistsInSet = false;
				for (size_t j = 0; j < UQFI.size(); j++) {
					AlreadyExistsInSet |= (UQFI[j] == QFI[i]);
				}
				if (!AlreadyExistsInSet) {
					UQFI.push_back(QFI[i]);
				}
			}
			else {
				UQFI.push_back(QFI[i]);
			}
		}

		this->QueueCreateInfo.resize(UQFI.size());

		std::vector<device::qfp> QueueFamily = this->Device->get_queue_family_properties();

		for (size_t i = 0; i < UQFI.size(); i++) {
			Queue.emplace_back(QueueFamily[UQFI[i]]);
		}

		for (size_t i = 0; i < Queue.size(); i++) {
			VkDeviceQueueCreateInfo NewQueueCreateInfo{};
			NewQueueCreateInfo.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			NewQueueCreateInfo.pNext				= NULL;
			NewQueueCreateInfo.flags				= 0;
			NewQueueCreateInfo.queueFamilyIndex		= UQFI[i];
			NewQueueCreateInfo.queueCount			= Queue[i].Priority.size();
			NewQueueCreateInfo.pQueuePriorities		= Queue[i].Priority.data();
			QueueCreateInfo.push_back(NewQueueCreateInfo);
		}

		this->Features = Device->get_features();

		// Load VkDevice Create Info.
		CreateInfo.sType						= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		CreateInfo.pNext						= NULL;
		CreateInfo.flags						= 0;
		CreateInfo.queueCreateInfoCount			= QueueCreateInfo.size();
		CreateInfo.pQueueCreateInfos			= QueueCreateInfo.data();
		CreateInfo.enabledLayerCount			= aLayer.count();
		CreateInfo.ppEnabledLayerNames			= aLayer.Handle.data();
		if (Device->is_extension_list_supported(aExtension.Handle)) {
			CreateInfo.enabledExtensionCount		= aExtension.count();
			CreateInfo.ppEnabledExtensionNames		= aExtension.Handle.data();
		}
		else {
			CreateInfo.enabledExtensionCount		= 0;
			CreateInfo.ppEnabledExtensionNames		= NULL;
		}
		CreateInfo.pEnabledFeatures				= &this->Features;

		// Create Vulkan Logical Device.
		Result = vkCreateDevice(Device->handle(), &this->CreateInfo, NULL, &this->Handle);
		if (Result == VK_SUCCESS) {
			*Engine << log::message(log::INFO, log::SUCCESS, "Context Creation", log::GEODESUKA, "context", "instance", "Device Context Creation Successful!");
		}
		else {
			*Engine << log::message(Result, "Context Creation", log::GEODESUKA, "context", "instance", "Device Context Creation Failed!");
		}

		// Get all create queues
		for (size_t i = 0; i < Queue.size(); i++) {
			for (size_t j = 0; j < Queue[i].count(); j++) {
				vkGetDeviceQueue(Handle, UQFI[i], j, &Queue[i][j]);
			}
		}

		this->CommandPool[0] = new command_pool(this, 0, device::operation::TRANSFER);
		this->CommandPool[1] = new command_pool(this, 0, device::operation::COMPUTE);
		this->CommandPool[2] = new command_pool(this, 0, device::operation::GRAPHICS);
		this->CommandPool[3] = new command_pool(this, 0, device::operation::GRAPHICS_AND_COMPUTE);

		ExecutionFence[0] = this->create_fence();
		ExecutionFence[1] = this->create_fence();
		ExecutionFence[2] = this->create_fence();

	}

	context::~context() {

		// Clear all data on queues.
		Queue.clear();

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

		delete this->CommandPool[0];
		delete this->CommandPool[1];
		delete this->CommandPool[2];
		delete this->CommandPool[3];

		vkDestroyDevice(this->Handle, NULL);

	}

	void context::operator<<(const log::message aNewLogMessage) {
		*Engine << aNewLogMessage;
	}

	VkCommandBuffer context::create_command_buffer(device::operation aDeviceOperation, VkCommandBufferLevel aLevel) {
		switch (aDeviceOperation) {
		case device::operation::TRANSFER:
			return this->CommandPool[0]->allocate(aLevel);
		case device::operation::COMPUTE:
			return this->CommandPool[1]->allocate(aLevel);
		case device::operation::GRAPHICS:
			return this->CommandPool[2]->allocate(aLevel);
		case device::operation::GRAPHICS_AND_COMPUTE:
			return this->CommandPool[3]->allocate(aLevel);
		default:
			return VK_NULL_HANDLE;
		}
	}

	command_list context::create_command_list(device::operation aDeviceOperation, VkCommandBufferLevel aLevel, uint aCount) {
		command_list List;
		switch (aDeviceOperation) {
		case device::operation::TRANSFER:
			List = this->CommandPool[0]->allocate(aLevel, aCount);
			break;
		case device::operation::COMPUTE:
			List = this->CommandPool[1]->allocate(aLevel, aCount);
			break;
		case device::operation::GRAPHICS:
			List = this->CommandPool[2]->allocate(aLevel, aCount);
			break;
		case device::operation::GRAPHICS_AND_COMPUTE:
			List = this->CommandPool[3]->allocate(aLevel, aCount);
			break;
		default:
			break;
		}
		return List;
	}

	void context::destroy_command_buffer(device::operation aQueueFamilySupportOption, VkCommandBuffer aCommandBuffer) {
		switch (aQueueFamilySupportOption) {
		case device::operation::TRANSFER:
			this->CommandPool[0]->release(aCommandBuffer);
			break;
		case device::operation::COMPUTE:
			this->CommandPool[1]->release(aCommandBuffer);
			break;
		case device::operation::GRAPHICS:
			this->CommandPool[2]->release(aCommandBuffer);
			break;
		case device::operation::GRAPHICS_AND_COMPUTE:
			this->CommandPool[3]->release(aCommandBuffer);
			break;
		default:
			break;
		}
	}

	void context::destroy_command_list(device::operation aQueueFamilySupportOption, command_list& aCommandList) {
		switch (aQueueFamilySupportOption) {
		case device::operation::TRANSFER:
			this->CommandPool[0]->release(aCommandList);
			break;
		case device::operation::COMPUTE:
			this->CommandPool[1]->release(aCommandList);
			break;
		case device::operation::GRAPHICS:
			this->CommandPool[2]->release(aCommandList);
			break;
		case device::operation::GRAPHICS_AND_COMPUTE:
			this->CommandPool[3]->release(aCommandList);
			break;
		default:
			break;
		}
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
		util::list<VkFence> Fence = this->create_fence(aFenceCreateFlags, 1);
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

	VkMemoryRequirements context::get_buffer_memory_requirements(VkBuffer aBufferHandle) {
		VkMemoryRequirements MemoryRequirements;
		vkGetBufferMemoryRequirements(this->Handle, aBufferHandle, &MemoryRequirements);
		return MemoryRequirements;
	}

	VkMemoryRequirements context::get_image_memory_requirements(VkImage aImageHandle) {
		VkMemoryRequirements MemoryRequirements;
		vkGetImageMemoryRequirements(this->Handle, aImageHandle, &MemoryRequirements);
		return MemoryRequirements;
	}

	// Memory Allocation.
	VkDeviceMemory context::allocate_memory(VkMemoryRequirements aMemoryRequirements, uint aMemoryType) {
		VkDeviceMemory MemoryHandle = VK_NULL_HANDLE;
		VkMemoryAllocateInfo AllocateInfo{};
		AllocateInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocateInfo.pNext				= NULL;
		AllocateInfo.allocationSize		= aMemoryRequirements.size;
		AllocateInfo.memoryTypeIndex	= this->Device->get_memory_type_index(aMemoryRequirements, aMemoryType);
		vkAllocateMemory(this->Handle, &AllocateInfo, NULL, &MemoryHandle);
		return MemoryHandle;
	}

	void context::free_memory(VkDeviceMemory aMemoryHandle) {

	}

	VkResult context::execute(device::operation aQFEO, VkCommandBuffer aCommandBuffer, VkFence aFence) {
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
		return this->execute(aQFEO, Submission, aFence);
	}

	VkResult context::execute(device::operation aQFEO, const command_list& aCommandList, VkFence aFence) {
		VkSubmitInfo SubmitInfo = aCommandList.build();
		std::vector<VkSubmitInfo> Submission;
		Submission.push_back(SubmitInfo);
		return this->execute(aQFEO, Submission, aFence);
	}

	VkResult context::execute(device::operation aQFS, const std::vector<gcl::command_list>& aCommandList, VkFence aFence) {
		std::vector<VkSubmitInfo> SubmissionList = convert(aCommandList);
		return this->execute(aQFS, SubmissionList, aFence);
	}

	VkResult context::execute(device::operation aQFEO, const std::vector<VkSubmitInfo>& aSubmissionList, VkFence aFence) {
		VkResult Result = VK_SUCCESS;
		return this->execute(device::operation::PRESENT, aSubmissionList, std::vector<VkPresentInfoKHR>(0), aFence);
	}

	VkResult context::execute(const std::vector<VkPresentInfoKHR>& aPresentationList) {
		VkResult Result = VK_SUCCESS;
		return this->execute(device::operation::PRESENT, std::vector<VkSubmitInfo>(0), aPresentationList, VK_NULL_HANDLE);
	}

	VkResult context::execute(device::operation aQFEO, const std::vector<VkSubmitInfo>& aSubmissionList, const std::vector<VkPresentInfoKHR>& aPresentationList, VkFence aFence) {
		VkResult Result = VK_SUCCESS;

		int i = this->qfi_to_i(aQFEO);
		int j = 0;

		// Queue Operation Not supported.
		if (i == -1) return VK_ERROR_FEATURE_NOT_PRESENT;

		while (true) {
			if (Queue[i].Mutex[j].try_lock()) {
				switch (aQFEO) {
				case device::operation::TRANSFER:
				case device::operation::COMPUTE:
				case device::operation::GRAPHICS:
				case device::operation::GRAPHICS_AND_COMPUTE:
					Result = vkQueueSubmit(Queue[i][j], aSubmissionList.size(), aSubmissionList.data(), aFence);
					break;
				case device::operation::PRESENT:
					for (size_t k = 0; k < aPresentationList.size(); k++) {
						Result = vkQueuePresentKHR(Queue[i][j], &aPresentationList[k]);
					}
					break;
				default:
					Result = VK_ERROR_FEATURE_NOT_PRESENT;
					break;
				}
				Queue[i].Mutex[j].unlock();
				break;
			}
			j = ((j == (Queue[i].count() - 1)) ? 0 : (j + 1));
		}
		return Result;
	}

	VkResult context::wait(VkFence aFence, VkBool32 aWaitOnAll) {
		return vkWaitForFences(this->Handle, 1, &aFence, aWaitOnAll, UINT64_MAX);
	}

	VkResult context::wait(util::list<VkFence> aFenceList, VkBool32 aWaitOnAll) {
		return vkWaitForFences(this->Handle, aFenceList.count(), aFenceList.Handle.data(), aWaitOnAll, UINT64_MAX);
	}

	bool context::available(device::operation aOperation) {
		return (this->qfi(aOperation) != -1);
	}

	int context::qfi(device::operation aOperation) {
		switch (aOperation) {
		case device::operation::TRANSFER: 
			return this->QFI[0];
		case device::operation::COMPUTE: 
			return this->QFI[1];
		case device::operation::GRAPHICS: 
			return this->QFI[2];
		case device::operation::GRAPHICS_AND_COMPUTE: 
			return this->QFI[3];
		case device::operation::PRESENT: 
			return this->QFI[4];
		default: 
			return -1;
		}
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

	context::queue_family::queue_family(device::qfp aProperties) {
		Mutex = new std::mutex[aProperties.QueueCount];
		for (uint32_t i = 0; i < aProperties.QueueCount; i++) {
			this->Priority.push_back(1.0f);
			this->Handle.push_back(VK_NULL_HANDLE);
		}
	}

	context::queue_family::queue_family(const queue_family& aInput) {
		*this = aInput;
	}

	context::queue_family::queue_family(queue_family&& aInput) noexcept {
		this->Mutex		= aInput.Mutex;
		aInput.Mutex	= nullptr;
		this->Priority	= aInput.Priority;
		this->Handle	= aInput.Handle;
	}

	context::queue_family::~queue_family() {
		delete[] this->Mutex;
		this->Mutex = nullptr;
	}

	VkQueue& context::queue_family::operator[](size_t aIndex) {
		return this->Handle[aIndex];
	}

	context::queue_family& context::queue_family::operator=(const queue_family& aRhs) {
		if (this == &aRhs) return *this;
		delete[] this->Mutex;
		this->Mutex = new std::mutex[aRhs.Handle.size()];
		this->Priority = aRhs.Priority;
		this->Handle = aRhs.Handle;
		return *this;
	}

	context::queue_family& context::queue_family::operator=(queue_family&& aRhs) noexcept {
		this->Mutex		= aRhs.Mutex;
		aRhs.Mutex		= nullptr;
		this->Priority	= aRhs.Priority;
		this->Handle	= aRhs.Handle;
		return *this;
	}

	size_t context::queue_family::count() const {
		return this->Handle.size();
	}

	int context::qfi_to_i(device::operation aOperation) {
		for (size_t i = 0; i < UQFI.size(); i++) {
			if (this->qfi(aOperation) == UQFI[i]) {
				return i;
			}
		}
		return -1;
	}

}
