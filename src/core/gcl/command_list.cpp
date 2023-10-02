#include <geodesuka/core/gcl/command_list.h>
#include <geodesuka/core/gcl/context.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include <geodesuka/core/gcl/config.h>

namespace geodesuka::core::gcl {

	command_list::command_list() { }

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

	void command_list::depends_on(VkSemaphore aDependencyLink, command_list& aProducer, VkPipelineStageFlags aProducerStageFlag) {
		if (!aProducer.is_signalling(aDependencyLink)) {
			aProducer.SignalSemaphore.push_back(aDependencyLink);
		}
		if (!this->is_waiting_on(aDependencyLink)) {
			this->WaitSemaphore.push_back(aDependencyLink);
			this->WaitStage.push_back(aProducerStageFlag);
		}
	}

	void command_list::wait_on(VkSemaphore aWaitSemaphore, VkPipelineStageFlags aProducerStageFlag) {
		if (!this->is_waiting_on(aWaitSemaphore)) {
			this->WaitSemaphore.push_back(aWaitSemaphore);
			this->WaitStage.push_back(aProducerStageFlag);
		}
	}

	void command_list::signal_to(VkSemaphore aSignalSemaphore) {
		if (!this->is_signalling(aSignalSemaphore)) {
			this->SignalSemaphore.push_back(aSignalSemaphore);
		}
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

	bool command_list::is_waiting_on(VkSemaphore aSemaphore) {
		for (size_t i = 0; i < WaitSemaphore.size(); i++) {
			if (WaitSemaphore[i] == aSemaphore) return true;
		}
		return false;
	}

	bool command_list::is_signalling(VkSemaphore aSemaphore) {
		for (size_t i = 0; i < SignalSemaphore.size(); i++) {
			if (SignalSemaphore[i] == aSemaphore) return true;
		}
		return false;
	}

	std::vector<VkSubmitInfo> convert(const std::vector<command_list>& aCommandList) {
		std::vector<VkSubmitInfo> SubmissionList(aCommandList.size());
		for (size_t i = 0; i < aCommandList.size(); i++) {
			SubmissionList[i] = aCommandList[i].build();
		}
		return SubmissionList;
	}

}
