#pragma once
#ifndef GEODESUKA_CORE_GCL_COMMAND_POOL_H
#define GEODESUKA_CORE_GCL_COMMAND_POOL_H

#include <mutex>

#include "config.h"
#include "command_list.h"
#include "device.h"
#include "context.h"

namespace geodesuka::core::gcl {

	class command_pool {
	public:

		friend class command_pool;

		enum flags {
    		ONE_TIME_SUBMIT_BIT 			= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
    		RENDER_PASS_CONTINUE_BIT 		= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
    		SIMULTANEOUS_USE_BIT 			= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		};

		enum level {
    		PRIMARY 						= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    		SECONDARY 						= VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		};

		// Use for command buffer writing.
		std::mutex Mutex;

		command_pool();
		command_pool(context* aContext, uint32_t aQueueFamilyIndex, int aFlags = 0);
		command_pool(context* aContext, device::operation aDeviceOperation, int aFlags = 0);
		~command_pool();

		// Creates a single command buffer from the chosen family support option.
		VkCommandBuffer allocate(VkCommandBufferLevel aLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		// Creates a series of command buffers from selected family support option.
		command_list allocate(uint32_t aCommandBufferCount, VkCommandBufferLevel aLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		// Destroys a single command buffer.
		bool release(VkCommandBuffer aCommandBuffer);

		// Destroys a command list.
		bool release(command_list& aCommandList);

	private:

		context* Context;
		VkCommandPoolCreateInfo CreateInfo{};
		VkCommandPool Handle;
		util::list<VkCommandBuffer> CommandList;

	};

}

#endif // !GEODESUKA_CORE_GCL_COMMAND_POOL_H
