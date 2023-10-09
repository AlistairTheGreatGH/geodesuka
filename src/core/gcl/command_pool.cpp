#include <geodesuka/core/gcl/command_pool.h>
#include <geodesuka/core/gcl/context.h>

#include <assert.h>

namespace geodesuka::core::gcl {

	command_pool::command_pool() {
		this->Context = nullptr;
		this->Handle = VK_NULL_HANDLE;
	}

	command_pool::command_pool(context* aContext, int aFlags, uint32_t aQueueFamilyIndex) {
		this->Context = aContext;
		this->Handle = VK_NULL_HANDLE;
		if (this->Context == nullptr) return;
		this->CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		this->CreateInfo.pNext = NULL;
		this->CreateInfo.flags = aFlags;
		this->CreateInfo.queueFamilyIndex = aQueueFamilyIndex;
		vkCreateCommandPool(aContext->handle(), &this->CreateInfo, NULL, &this->Handle);
	}

	command_pool::command_pool(context* aContext, int aFlags, device::operation aDeviceOperation) {
		this->Context = aContext;
		this->Handle = VK_NULL_HANDLE;
		if (this->Context == nullptr) return;
		this->CreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		this->CreateInfo.pNext = NULL;
		this->CreateInfo.flags = aFlags;
		this->CreateInfo.queueFamilyIndex = aContext->parent()->qfi(aDeviceOperation);
		vkCreateCommandPool(aContext->handle(), &this->CreateInfo, NULL, &this->Handle);
	}

	command_pool::~command_pool() {
		if (Context != nullptr) {
			if (CommandList.count() > 0) {
				vkFreeCommandBuffers(Context->handle(), Handle, CommandList.Handle.size(), CommandList.Handle.data());
			}
			if (Handle != VK_NULL_HANDLE) {
				vkDestroyCommandPool(Context->handle(), Handle, NULL);
			}
		}
		Context = nullptr;
	}

	VkCommandBuffer command_pool::allocate(VkCommandBufferLevel aLevel) {
		VkCommandBuffer ReturnCommandBuffer = VK_NULL_HANDLE;
		VkCommandBufferAllocateInfo AllocationInfo{};
		AllocationInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocationInfo.pNext				= NULL;
		AllocationInfo.commandPool			= this->Handle;
		AllocationInfo.level				= aLevel;
		AllocationInfo.commandBufferCount	= 1;
		VkResult Result = vkAllocateCommandBuffers(this->Context->handle(), &AllocationInfo, &ReturnCommandBuffer);
		assert(Result == VK_SUCCESS);
		this->CommandList |= ReturnCommandBuffer;
		return ReturnCommandBuffer;
	}

	command_list command_pool::allocate(VkCommandBufferLevel aLevel, uint32_t aCommandBufferCount) {
		command_list NewCommandList(aCommandBufferCount);
		VkCommandBufferAllocateInfo AllocationInfo{};
		AllocationInfo.sType				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocationInfo.pNext				= NULL;
		AllocationInfo.commandPool			= this->Handle;
		AllocationInfo.level				= aLevel;
		AllocationInfo.commandBufferCount	= aCommandBufferCount;
		VkResult Result = vkAllocateCommandBuffers(this->Context->handle(), &AllocationInfo, NewCommandList.Handle.data());
		assert(Result == VK_SUCCESS);
		this->CommandList |= NewCommandList;
		return NewCommandList;
	}

	bool command_pool::release(VkCommandBuffer aCommandBuffer) {
		vkFreeCommandBuffers(this->Context->handle(), this->Handle, 1, &aCommandBuffer);
		this->CommandList -= aCommandBuffer;
		return false;
	}

	bool command_pool::release(command_list& aCommandList) {
		vkFreeCommandBuffers(this->Context->handle(), this->Handle, aCommandList.count(), aCommandList.Handle.data());
		this->CommandList -= aCommandList;
		return false;
	}

}
