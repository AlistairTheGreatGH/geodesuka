#include <geodesuka/core/gcl/command_list.h>
#include <geodesuka/core/gcl/context.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include <geodesuka/core/gcl/config.h>

namespace geodesuka::core::gcl {

	command_list::command_list() { }

	command_list::~command_list() {
		for (size_t i = 0; i < this->WaitSemaphore.size(); i++) {
			this->Context->destroy_semaphore(this->WaitSemaphore[i]);
		}
		for (size_t i = 0; i < this->SignalSemaphore.size(); i++) {
			this->Context->destroy_semaphore(this->SignalSemaphore[i]);
		}
	}

	command_list::command_list(VkCommandBuffer aCommandBuffer) {
		this->Handle.push_back(aCommandBuffer);
	}

	command_list::command_list(uint32_t aCommandBufferCount) {
		this->Handle.resize(aCommandBufferCount);
		for (size_t i = 0; i < this->Handle.size(); i++) {
			this->Handle[i] = VK_NULL_HANDLE;
		}
	}

	command_list::command_list(uint32_t aCommandBufferCount, VkCommandBuffer* aCommandBufferList) {
		this->Handle.resize(aCommandBufferCount);
		memcpy(this->Handle.data(), aCommandBufferList, aCommandBufferCount * sizeof(VkCommandBuffer));
	}

	command_list command_list::prune() const {
		size_t NullCount = 0;

		for (size_t i = 0; i < Handle.size(); i++) {
			if (Handle[i] == VK_NULL_HANDLE) {
				NullCount += 1;
			}
		}

		if (NullCount == 0) return *this;

		size_t j = 0;
		command_list Pruned(Handle.size() - NullCount);
		for (uint32_t i = 0; i < Handle.size(); i++) {
			if (Handle[i] != VK_NULL_HANDLE) {
				Pruned[j] = Handle[i];
				j += 1;
			}
		}

		Pruned.WaitSemaphore			= this->WaitSemaphore;
		Pruned.WaitStage				= this->WaitStage;
		Pruned.SignalSemaphore			= this->SignalSemaphore;

		return Pruned;
	}

	// Due to the nature of semaphores, a single semaphore cannot signal multiple waiting command
	// lists. Therefore a unique semaphore must exist between ever command_list that wishes to wait
	// on another. If that is the case, then we might as well generate a semaphore everytime we want
	// to designate a dependency between command_lists.
	void command_list::depends_on(command_list& aProducer, VkPipelineStageFlags aProducerStageFlag) {
		// Check if command_lists were created by the same device context, if not, throw exception.
		if ((this->Context == aProducer.Context) && (this->Context != nullptr)) {
			throw std::runtime_error("command_list::depends_on: requires that both command_lists share the same device context that they were created by!");
		}
		// Create new semaphore.
		VkSemaphore NewSemaphore = this->Context->create_semaphore();
		// Add the new semaphore to their respective signal/wait lists.
		aProducer.SignalSemaphore.push_back(NewSemaphore);
		this->WaitSemaphore.push_back(NewSemaphore);
	}

	VkSubmitInfo command_list::build() const {
		VkSubmitInfo SubmissionBuild{};
		SubmissionBuild.sType					= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmissionBuild.pNext					= NULL;
		SubmissionBuild.waitSemaphoreCount		= this->WaitSemaphore.size();
		SubmissionBuild.pWaitSemaphores			= this->WaitSemaphore.data();
		SubmissionBuild.pWaitDstStageMask		= this->WaitStage.data();
		SubmissionBuild.commandBufferCount		= this->Handle.size();
		SubmissionBuild.pCommandBuffers			= this->Handle.data();
		SubmissionBuild.signalSemaphoreCount	= this->SignalSemaphore.size();
		SubmissionBuild.pSignalSemaphores		= this->SignalSemaphore.data();
		return SubmissionBuild;
	}

	std::vector<VkSubmitInfo> convert(const std::vector<command_list>& aCommandList) {
		std::vector<VkSubmitInfo> SubmissionList(aCommandList.size());
		for (size_t i = 0; i < aCommandList.size(); i++) {
			SubmissionList[i] = aCommandList[i].build();
		}
		return SubmissionList;
	}

}
