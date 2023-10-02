#pragma once
#ifndef GEODESUKA_CORE_GCL_COMMAND_LIST_H
#define GEODESUKA_CORE_GCL_COMMAND_LIST_H

#include <vector>

#include "../util/list.h"

#include "config.h"

namespace geodesuka::core::gcl {

	// ----- command_list ----- //
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

		command_list();
		command_list(VkCommandBuffer aCommandBuffer);
		command_list(uint32_t aCommandBufferCount);
		command_list(uint32_t aCommandBufferCount, VkCommandBuffer* aCommandBufferList);

		// Removes VK_NULL_HANDLE elements.
		command_list prune() const;

		// ---------- Dependency/Synchronization ---------- //
		// This section is rather short, because setting up 
		// dependencies between command_lists should be easy.
		// This function logically operates as follows:
		// Command List "A" Depends On The Output of "Fragment Shader" Stage of Command List "B"
		// A.depends_on(S, B, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
		void depends_on(VkSemaphore aDependencyLink, command_list& aProducer, VkPipelineStageFlags aProducerStageFlag);

		void wait_on(VkSemaphore aWaitSemaphore, VkPipelineStageFlags aProducerStageFlag);

		void signal_to(VkSemaphore aSignalSemaphore);

		// A VkSubmitInfo object is created to reference the contents
		// within command_list.
		VkSubmitInfo build() const;

	private:

		context* 								Context;
		std::vector<VkSemaphore>				WaitSemaphore;
		std::vector<VkPipelineStageFlags>		WaitStage;
		std::vector<VkSemaphore>				SignalSemaphore;

		bool is_waiting_on(VkSemaphore aSemaphore);
		bool is_signalling(VkSemaphore aSemaphore);

	};

	std::vector<VkSubmitInfo> convert(const std::vector<command_list>& aCommandList);

}

#endif // !GEODESUKA_CORE_GCL_COMMAND_LIST_H
