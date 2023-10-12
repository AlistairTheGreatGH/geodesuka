#include <geodesuka/core/gcl/buffer.h>

#include <cstdlib>
#include <cstring>
#include <assert.h>

#include <vector>
#include <algorithm>

#include <geodesuka/core/gcl/config.h>

// Used to interact with texture class
#include <geodesuka/core/gcl/image.h>

namespace geodesuka::core::gcl {

	buffer::create_info::create_info() {
		this->MemoryType 	= (device::memory)0u;
		this->BufferUsage 	= (buffer::usage)0u;
	}

	buffer::create_info::create_info(uint aMemoryType, uint aBufferUsage) {
		this->MemoryType = aMemoryType;
		this->BufferUsage = aBufferUsage;
	}

	buffer::buffer() {
		this->zero_out();
	}

	buffer::buffer(context* aContext, create_info aCreateInfo, int aVertexCount, variable aVertexLayout, void* aVertexData) {
		VkResult Result = VK_SUCCESS;
		size_t TotalSize = aVertexCount * aVertexLayout.size();
		this->zero_out();

		Result = this->create(aContext, aCreateInfo.MemoryType, aCreateInfo.BufferUsage, TotalSize, aVertexData);
	}

	buffer::buffer(context* aContext, uint aMemoryType, uint aBufferUsage, int aVertexCount, variable aVertexLayout, void* aVertexData) {
		VkResult Result = VK_SUCCESS;
		size_t TotalSize = aVertexCount * aVertexLayout.size();
		this->zero_out();

		Result = this->create(aContext, aMemoryType, aBufferUsage, TotalSize, aVertexData);
	}

	buffer::buffer(context* aContext, create_info aCreateInfo, size_t aBufferSize, void* aBufferData) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		// Allocate Device Memory and Write Buffer
		Result = this->create(aContext, aCreateInfo.MemoryType, aCreateInfo.BufferUsage, aBufferSize, aBufferData);
	}

	buffer::buffer(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aBufferSize, void* aBufferData) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		// Create memory for vertex buffer.
		Result = this->create(aContext, aMemoryType, aBufferUsage, aBufferSize, aBufferData);
	}

	buffer::buffer(buffer& aInput) {
		VkResult Result = VK_SUCCESS;

		// Zero out new object.
		this->zero_out();

		if (aInput.Context != nullptr) {
			Result = this->create(aInput.Context, this->MemoryType, this->Usage, aInput.Size);

			command_list TransferCommandList = this->copy(aInput, 0, 0, aInput.Size);

			Result = this->Context->execute_and_wait(device::operation::TRANSFER, TransferCommandList);

			this->Context->destroy_command_list(device::operation::TRANSFER, TransferCommandList);
		}
	}

	buffer::buffer(buffer&& aInput) noexcept {
		this->Context			= aInput.Context;
		this->Size 				= aInput.Size;
		this->Usage 			= aInput.Usage;
		this->Handle			= aInput.Handle;
		this->MemoryType 		= aInput.MemoryType;
		this->MemoryHandle		= aInput.MemoryHandle;
		aInput.zero_out();
	}

	buffer::~buffer() {
		this->clear_device_memory();
		this->zero_out();
	}

	// TODO: Optimize for memory recycling.
	buffer& buffer::operator=(buffer& aRhs) {
		if (this == &aRhs) return *this;
		VkResult Result = VK_SUCCESS;
		this->clear_device_memory();

		if (aRhs.Context != nullptr) {
			// Create buffer object.
			Result = this->create(aRhs.Context, aRhs.MemoryType, aRhs.Usage, aRhs.Size);

			command_list TransferCommandList = this->copy(aRhs, 0, 0, aRhs.Size);

			Result = this->Context->execute_and_wait(device::operation::TRANSFER, TransferCommandList);

			this->Context->destroy_command_list(device::operation::TRANSFER, TransferCommandList);
		}

		return *this;
	}

	buffer& buffer::operator=(buffer&& aRhs) noexcept {
		this->clear_device_memory();
		this->Context			= aRhs.Context;
		this->Size 				= aRhs.Size;
		this->Usage 			= aRhs.Usage;
		this->Handle			= aRhs.Handle;
		this->MemoryType 		= aRhs.MemoryType;
		this->MemoryHandle		= aRhs.MemoryHandle;
		aRhs.zero_out();
		return *this;
	}

	command_list buffer::copy(buffer& aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize) {
		VkBufferCopy Region{};
		Region.srcOffset		= aSourceOffset;
		Region.dstOffset		= aDestinationOffset;
		Region.size				= aRegionSize;
		std::vector<VkBufferCopy> RegionList;
		RegionList.push_back(Region);
		return this->copy(aSourceData, RegionList);
	}

	command_list buffer::copy(buffer& aSourceData, std::vector<VkBufferCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		command_list CommandList = this->Context->create_command_list(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext 				= NULL;
		BeginInfo.flags 				= 0;
		BeginInfo.pInheritanceInfo 		= NULL;

		// Fill out command buffer
		Result = vkBeginCommandBuffer(CommandList[0], &BeginInfo);
		vkCmdCopyBuffer(CommandList[0], aSourceData.Handle, this->Handle, aRegionList.size(), aRegionList.data());
		Result = vkEndCommandBuffer(CommandList[0]);

		return CommandList;
	}

	command_list buffer::copy(image& aSourceData, VkImageLayout aImageLayout, VkBufferImageCopy aRegion) {
		std::vector<VkBufferImageCopy> RegionList;
		RegionList.push_back(aRegion);
		return this->copy(aSourceData, aImageLayout, RegionList);
	}

	command_list buffer::copy(image& aSourceData, VkImageLayout aImageLayout, std::vector<VkBufferImageCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		command_list CommandList = this->Context->create_command_list(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext 				= NULL;
		BeginInfo.flags 				= 0;
		BeginInfo.pInheritanceInfo 		= NULL;

		// Fill out command buffer
		Result = vkBeginCommandBuffer(CommandList[0], &BeginInfo);
		vkCmdCopyImageToBuffer(CommandList[0], aSourceData.Handle, aImageLayout, this->Handle, aRegionList.size(), aRegionList.data());
		Result = vkEndCommandBuffer(CommandList[0]);

		return CommandList;
	}

	VkResult buffer::write(void* aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize) {
		std::vector<VkBufferCopy> RegionList;
		VkBufferCopy Region{ aSourceOffset, aDestinationOffset, aRegionSize };
		RegionList.push_back(Region);
		return this->write(aSourceData, RegionList);
	}

	VkResult buffer::write(void* aSourceData, std::vector<VkBufferCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		if ((this->MemoryType & device::memory::HOST_VISIBLE) == device::memory::HOST_VISIBLE) {
			// Host Visible, can be written to directly.
			void* HostMemory = NULL;
			Result = vkMapMemory(this->Context->handle(), this->MemoryHandle, 0, VK_WHOLE_SIZE, 0, &HostMemory);
			for (size_t i = 0; i < aRegionList.size(); i++) {
				// Calculate offset addresses
				uintptr_t TargetAddress = (uintptr_t)HostMemory + aRegionList[i].dstOffset;
				uintptr_t SourceAddress = (uintptr_t)aSourceData + aRegionList[i].srcOffset;
				// Copy specified targets.
				memcpy((void*)TargetAddress, (void*)SourceAddress, aRegionList[i].size);
			}
			vkUnmapMemory(this->Context->handle(), this->MemoryHandle);
		} else {
			// Not Host Visible, use staging buffer. For large uploads, we will try something different.
			// Say we have a large model, we want to use a staging buffer that is small to upload chunks
			// to device memory.
			size_t StagingBufferSize = GCL_TRANSFER_CHUNK_SIZE;

			// Not Host Visible, use staging buffer.
			buffer StagingBuffer(
				Context,
				device::memory::HOST_VISIBLE | device::memory::HOST_COHERENT,
				buffer::TRANSFER_SRC,
				StagingBufferSize
			);

			// Copy each memory region to target buffer.
			for (size_t i = 0; i < aRegionList.size(); i++) {

				// size_t TotalSize = aRegionList[i].size;
				// size_t RemainderSize = TotalSize % ChunkSize; // Calculates last remainder of data.
				// size_t ChunkCount = ((TotalSize - RemainderSize) / ChunkSize) + 1;
				
				size_t TotalSize = aRegionList[i].size;
				size_t Remainder = TotalSize;
				do {

					// Calculate offsets for transfer.
					size_t Offset = TotalSize - Remainder;

					// Insure that we do not exceed the staging buffer size.
					size_t ChunkSize = std::clamp(Remainder, (size_t)0, StagingBufferSize);

					// Write data to staging buffer, if less than ChunkSize, then we are done.
					StagingBuffer.write(aSourceData, aRegionList[i].srcOffset + Offset, 0, ChunkSize);

					// generate transfer commands.
					command_list CommandList = this->copy(StagingBuffer, 0, aRegionList[i].dstOffset + Offset, ChunkSize);

					// Execute command buffer
					this->Context->execute_and_wait(device::operation::TRANSFER, CommandList);

					// Recalculate remaining data to send.
					Remainder -= ChunkSize;

				} while (Remainder > 0);

			}

		}
		return Result;
	}

	VkResult buffer::read(void* aDestinationData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize) {
		std::vector<VkBufferCopy> RegionList;
		VkBufferCopy Region{ aSourceOffset, aDestinationOffset, aRegionSize };
		RegionList.push_back(Region);
		return this->read(aDestinationData, RegionList);
	}

	VkResult buffer::read(void* aDestinationData, std::vector<VkBufferCopy> aRegionList) {

	}


	VkBuffer& buffer::handle() {
		return this->Handle;
	}

	VkMemoryRequirements buffer::get_memory_requirements() const {
		return this->Context->get_buffer_memory_requirements(this->Handle);
	}

	VkResult buffer::create(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aMemorySize, void* aBufferData) {
		VkResult Result = VK_SUCCESS;

		this->Context = aContext;
		this->Size = aMemorySize;
		this->Usage = aBufferUsage;
		this->MemoryType = aMemoryType;

		// Create Buffer Object
		this->Handle = this->create_handle(aMemorySize, aBufferUsage);

		// Get buffer memory requirements.
		VkMemoryRequirements MemoryRequirements = this->get_memory_requirements();

		// Allocate memory for buffer.
		this->MemoryHandle = this->Context->allocate_memory(MemoryRequirements, aMemoryType);

		Result = vkBindBufferMemory(this->Context->handle(), this->Handle, this->MemoryHandle, 0);

		this->write(aBufferData, 0, 0, aMemorySize);

		return Result;
	}

	VkBuffer buffer::create_handle(size_t aSize, uint aUsage) {
		VkResult Result = VK_SUCCESS;
		VkBuffer NewHandle = VK_NULL_HANDLE;
		VkBufferCreateInfo CreateInfo{};
		// Create Buffer Object
		CreateInfo.sType					= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext					= NULL;
		CreateInfo.flags					= 0; // Ignore.
		CreateInfo.size						= aSize;
		CreateInfo.usage					= (VkBufferUsageFlags)(aUsage | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST); // Enable Transfer
		CreateInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount	= 0;
		CreateInfo.pQueueFamilyIndices		= NULL;
		Result = vkCreateBuffer(this->Context->handle(), &CreateInfo, NULL, &NewHandle);
		return NewHandle;
	}

	void buffer::clear_device_memory() {
		if (this->Context != nullptr) {
			if (this->Handle != VK_NULL_HANDLE) {
				vkDestroyBuffer(this->Context->handle(), this->Handle, NULL);
				this->Handle = VK_NULL_HANDLE;
			}
			if (this->MemoryHandle != VK_NULL_HANDLE) {
				vkFreeMemory(this->Context->handle(), this->MemoryHandle, NULL);
				this->MemoryHandle = VK_NULL_HANDLE;
			}
		}
		this->MemoryType 	= 0;		
		this->Usage 		= 0;
		this->Size 			= 0;
		this->Context		= nullptr;
	}

	void buffer::zero_out() {
		this->Context		= nullptr;
		this->Size 			= 0;
		this->Usage 		= 0;
		this->Handle 		= VK_NULL_HANDLE;
		this->MemoryType 	= 0;
		this->MemoryHandle 	= VK_NULL_HANDLE;
	}

}
