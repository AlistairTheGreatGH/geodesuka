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
			TRANSIENT_BIT				= 0x00000001,
			RESET_COMMAND_BUFFER_BIT	= 0x00000002,
			PROTECTED_BIT				= 0x00000004,
		};

		enum level {
			PRIMARY,
			SECONDARY
		};

		// Use for command buffer writing.
		std::mutex Mutex;

		command_pool();
		command_pool(context* aContext, int aFlags, uint32_t aQueueFamilyIndex);
		command_pool(context* aContext, int aFlags, device::operation aQueueFamilySupport);
		~command_pool();

		// Creates a single command buffer from the chosen family support option.
		VkCommandBuffer allocate(VkCommandBufferLevel aLevel);

		// Creates a series of command buffers from selected family support option.
		command_list allocate(VkCommandBufferLevel aLevel, uint32_t aCommandBufferCount);

		// Destroys a single command buffer.
		bool release(VkCommandBuffer aCommandBuffer);

		// Destroys a command list.
		bool release(command_list& aCommandList);

	private:

		context* Context;
		VkCommandPoolCreateInfo CreateInfo{};
		VkCommandPool Handle;
		command_list CommandList;

	};

}

#endif // !GEODESUKA_CORE_GCL_COMMAND_POOL_H
