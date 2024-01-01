#pragma once
#ifndef GEODESUKA_CORE_GCL_COMMAND_LIST_H
#define GEODESUKA_CORE_GCL_COMMAND_LIST_H

#include <vector>

#include "../util/list.h"

#include "config.h"

namespace geodesuka::core::gcl {

	// --------------- command_list --------------- //
	// The command_list object is a container class for VkCommandBuffer
	// but with some additional features for the management of VkCommandBuffer
	// objects. Consider them quality of life features to reduce code redundancy.
	// A command_list is also a flexible container class, used to ultimately
	// construct VkSubmitInfo objects for command execution. The command_list
	// also contains VkSemaphore objects to manage depedencies between command_lists.
	// A function is provided which created a VkSemaphore object to synchronize
	// between two lists.

	// command_list is just simply a container class with the ultimate
	// goal of creating VkSubmitInfo structures. The nature of 
	// VkSubmitInfo is that its members are unmodifiable. Therefore
	// this class has been created because it contains a set of methods
	// for modifying the members of VkSubmitInfo before it is built.
	// This includes semaphore linking between lists. Since command_list
	// is simply just a conatainer class, the user must bring his/her
	// own vk_command_buffers and vk_semaphores. command_list will
	// not create/destroy these resources. In fact, in the geodesuka engine
	// it is encouraged that all resources be owned/managed/possessed by
	// the parent object_t of the resource.
	// 
	class command_list : public util::list<VkCommandBuffer> {
	public:

		friend class command_pool;

		// Requirement is that command lists can only be created by command_pools.
		command_list();
		command_list(VkCommandBuffer aCommandBuffer);
		command_list(uint32_t aCommandBufferCount);
		command_list(uint32_t aCommandBufferCount, VkCommandBuffer* aCommandBufferList);
		~command_list();

		// Removes VK_NULL_HANDLE elements.
		command_list prune() const;

		// ---------- Dependency/Synchronization ---------- //
		// command_lists can have designated dependencies between each other.
		// The only requirement is that the command_lists in question must
		// share the same device context. 
		//
		// This function syntatically operates as follows:
		// Command list "A" depends on the "Fragment Shader" pipeline stage of command list "B"
		// A.depends_on(B, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
		void depends_on(command_list& aProducer, VkPipelineStageFlags aProducerStageFlag);

		// A VkSubmitInfo object is created to reference the contents
		// within command_list.
		VkSubmitInfo build() const;

	private:

		context* 								Context;
		std::vector<VkSemaphore>				WaitSemaphore;
		std::vector<VkPipelineStageFlags>		WaitStage;
		std::vector<VkSemaphore>				SignalSemaphore;

	};

	std::vector<VkSubmitInfo> convert(const std::vector<command_list>& aCommandList);

}

#endif // !GEODESUKA_CORE_GCL_COMMAND_LIST_H
