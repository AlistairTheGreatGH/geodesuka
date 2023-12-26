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

	buffer::buffer(context* aContext, create_info aCreateInfo, int aVertexCount, variable aVertexLayout, void* aVertexData) 
		: buffer(aContext, aCreateInfo.MemoryType, aCreateInfo.BufferUsage, aVertexCount * aVertexLayout.size(), aVertexData) {
	}

	buffer::buffer(context* aContext, uint aMemoryType, uint aBufferUsage, int aVertexCount, variable aVertexLayout, void* aVertexData)
		: buffer(aContext, aMemoryType, aBufferUsage, aVertexCount * aVertexLayout.size(), aVertexData) {
	}

	buffer::buffer(context* aContext, create_info aCreateInfo, size_t aBufferSize, void* aBufferData) 
		: buffer(aContext, aCreateInfo.MemoryType, aCreateInfo.BufferUsage, aBufferSize, aBufferData) {
	}

	buffer::buffer(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aBufferSize, void* aBufferData) : buffer() {
		VkResult Result = VK_SUCCESS;

		Context 								= aContext;

		CreateInfo.sType						= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		CreateInfo.pNext						= NULL;
		CreateInfo.flags						= 0;
		CreateInfo.size							= aBufferSize;
		CreateInfo.usage						= (VkBufferUsageFlags)(aBufferUsage);// | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST);
		CreateInfo.sharingMode					= VK_SHARING_MODE_EXCLUSIVE;
		CreateInfo.queueFamilyIndexCount		= 0;
		CreateInfo.pQueueFamilyIndices			= NULL;

		// Create Buffer Object
		Result = vkCreateBuffer(Context->handle(), &CreateInfo, NULL, &Handle);

		// Get Memory Requirements for Buffer.
		VkMemoryRequirements MemoryRequirements = this->memory_requirements();

		// Allocate memory for buffer.
		MemoryType = aMemoryType;
		MemoryHandle = Context->allocate_memory(MemoryRequirements, aMemoryType);

		// Bind Buffer to allocated Memory.
		Result = vkBindBufferMemory(this->Context->handle(), this->Handle, this->MemoryHandle, 0);

		// Write data to buffer object.
		if (aBufferData != NULL) {
			this->write(aBufferData, 0, 0, aBufferSize);
		}
	}

	buffer::buffer(buffer& aInput) : buffer(aInput.Context, aInput.MemoryType, aInput.CreateInfo.usage, aInput.CreateInfo.size, NULL) {
		this->copy(aInput, 0, 0, aInput.CreateInfo.size);
	}

	buffer::buffer(buffer&& aInput) noexcept {
		this->Context			= aInput.Context;
		this->CreateInfo 		= aInput.CreateInfo;
		this->Handle			= aInput.Handle;
		this->MemoryType 		= aInput.MemoryType;
		this->MemoryHandle		= aInput.MemoryHandle;
		aInput.zero_out();
	}

	buffer::~buffer() {
		this->clear();
	}

	// TODO: Optimize for memory recycling.
	buffer& buffer::operator=(buffer& aRhs) {
		if (this == &aRhs) return *this;

		VkResult Result = VK_SUCCESS;

		this->clear();

		*this = buffer(aRhs.Context, aRhs.MemoryType, aRhs.CreateInfo.usage, aRhs.CreateInfo.size, NULL);

		Result = this->copy(aRhs, 0, 0, aRhs.CreateInfo.size);

		return *this;
	}

	buffer& buffer::operator=(buffer&& aRhs) noexcept {
		this->clear();
		this->Context			= aRhs.Context;
		this->CreateInfo 		= aRhs.CreateInfo;
		this->Handle			= aRhs.Handle;
		this->MemoryType 		= aRhs.MemoryType;
		this->MemoryHandle		= aRhs.MemoryHandle;
		aRhs.zero_out();
		return *this;
	}

	void buffer::copy(VkCommandBuffer aCommandBuffer, buffer& aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize) {
		VkBufferCopy Region{};
		Region.srcOffset		= aSourceOffset;
		Region.dstOffset		= aDestinationOffset;
		Region.size				= aRegionSize;
		std::vector<VkBufferCopy> RegionList;
		RegionList.push_back(Region);
		this->copy(aCommandBuffer, aSourceData, RegionList);
	}

	void buffer::copy(VkCommandBuffer aCommandBuffer, buffer& aSourceData, std::vector<VkBufferCopy> aRegionList) {
		vkCmdCopyBuffer(aCommandBuffer, aSourceData.Handle, this->Handle, aRegionList.size(), aRegionList.data());
	}

	void buffer::copy(VkCommandBuffer aCommandBuffer, image& aSourceData, VkBufferImageCopy aRegion) {
		std::vector<VkBufferImageCopy> RegionList;
		RegionList.push_back(aRegion);
		this->copy(aCommandBuffer, aSourceData, RegionList);
	}

	void buffer::copy(VkCommandBuffer aCommandBuffer, image& aSourceData, std::vector<VkBufferImageCopy> aRegionList) {
		vkCmdCopyImageToBuffer(
			aCommandBuffer, 
			aSourceData.Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
			this->Handle, 
			aRegionList.size(), aRegionList.data()
		);
	}
	
	VkResult buffer::copy(buffer& aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize) {
		VkBufferCopy Region{};
		Region.srcOffset		= aSourceOffset;
		Region.dstOffset		= aDestinationOffset;
		Region.size				= aRegionSize;
		std::vector<VkBufferCopy> RegionList;
		RegionList.push_back(Region);
		return this->copy(aSourceData, RegionList);
	}

	VkResult buffer::copy(buffer& aSourceData, std::vector<VkBufferCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		VkCommandBuffer CommandBuffer = Context->create_command_buffer(device::operation::TRANSFER);
		Result = Context->begin(CommandBuffer);
		this->copy(CommandBuffer, aSourceData, aRegionList);
		Result = Context->end(CommandBuffer);
		Result = Context->execute_and_wait(device::operation::TRANSFER, CommandBuffer);
		Context->destroy_command_buffer(device::operation::TRANSFER, CommandBuffer);
		return Result;
	}

	VkResult buffer::copy(image& aSourceData, VkBufferImageCopy aRegion) {
		std::vector<VkBufferImageCopy> RegionList;
		RegionList.push_back(aRegion);
		return this->copy(aSourceData, RegionList);
	}

	VkResult buffer::copy(image& aSourceData, std::vector<VkBufferImageCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		VkCommandBuffer CommandBuffer = Context->create_command_buffer(device::operation::TRANSFER);
		Result = Context->begin(CommandBuffer);
		this->copy(CommandBuffer, aSourceData, aRegionList);
		Result = Context->end(CommandBuffer);
		Result = Context->execute_and_wait(device::operation::TRANSFER, CommandBuffer);
		Context->destroy_command_buffer(device::operation::TRANSFER, CommandBuffer);
		return Result;
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
			for (size_t i = 0; i < aRegionList.size(); i++) {
				// Calculate offset addresses
				void* ptr = NULL;
				Result = vkMapMemory(this->Context->handle(), this->MemoryHandle, aRegionList[i].dstOffset, aRegionList[i].size, 0, &ptr);
				//uintptr_t TargetAddress = (uintptr_t)ptr + aRegionList[i].dstOffset;
				uintptr_t SourceAddress = (uintptr_t)aSourceData + aRegionList[i].srcOffset;
				// Copy specified targets.
				memcpy((void*)ptr, (void*)SourceAddress, aRegionList[i].size);
				vkUnmapMemory(this->Context->handle(), this->MemoryHandle);
			}
		} else {
			// Not Host Visible, use staging buffer. For large uploads, we will try something different.
			// Say we have a large model, we want to use a staging buffer that is small to upload chunks
			// to device memory.
			size_t StagingBufferSize = GCL_TRANSFER_GRANULARITY_SIZE;

			// Not Host Visible, use staging buffer.
			buffer StagingBuffer(
				Context,
				device::memory::HOST_VISIBLE | device::memory::HOST_COHERENT,
				buffer::TRANSFER_SRC,
				StagingBufferSize
			);

			// Copy each memory region to target buffer.
			for (size_t i = 0; i < aRegionList.size(); i++) {
				
				size_t Remainder = aRegionList[i].size;
				do {

					// Calculate chunk offsets for region for transfer.
					size_t ChunkOffset = aRegionList[i].size - Remainder;

					// Insure that we do not exceed the staging buffer size.
					size_t ChunkSize = std::clamp(Remainder, (size_t)0, StagingBufferSize);

					// Write data to staging buffer, if less than ChunkSize, then we are done.
					StagingBuffer.write(aSourceData, aRegionList[i].srcOffset + ChunkOffset, 0, ChunkSize);

					// Execute transfer operation.
					this->copy(StagingBuffer, 0, aRegionList[i].dstOffset + ChunkOffset, ChunkSize);

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
		VkResult Result = VK_SUCCESS;
		if ((this->MemoryType & device::memory::HOST_VISIBLE) == device::memory::HOST_VISIBLE) {
			// Host Visible, can be written to directly.
			for (size_t i = 0; i < aRegionList.size(); i++) {
				// Calculate offset addresses
				void* ptr = NULL;
				Result = vkMapMemory(this->Context->handle(), this->MemoryHandle, aRegionList[i].srcOffset, aRegionList[i].size, 0, &ptr);
				uintptr_t TargetAddress = (uintptr_t)aDestinationData + aRegionList[i].dstOffset;
				// Copy specified targets.
				memcpy((void*)TargetAddress, ptr, aRegionList[i].size);
				vkUnmapMemory(this->Context->handle(), this->MemoryHandle);
			}
		} else {
			// Not Host Visible, use staging buffer. For large uploads, we will try something different.
			// Say we have a large model, we want to use a staging buffer that is small to upload chunks
			// to device memory.
			size_t StagingBufferSize = GCL_TRANSFER_GRANULARITY_SIZE;

			// Not Host Visible, use staging buffer.
			buffer StagingBuffer(
				Context,
				device::memory::HOST_VISIBLE | device::memory::HOST_COHERENT,
				buffer::TRANSFER_SRC,
				StagingBufferSize
			);

			// Copy each memory region to aDestinationData.
			for (size_t i = 0; i < aRegionList.size(); i++) {
				
				size_t Remainder = aRegionList[i].size;
				do {

					// Calculate chunk offsets for region for transfer.
					size_t ChunkOffset = aRegionList[i].size - Remainder;

					// Insure that we do not exceed the staging buffer size.
					size_t ChunkSize = std::clamp(Remainder, (size_t)0, StagingBufferSize);

					// Copy *this into staging buffer.
					StagingBuffer.copy(*this, aRegionList[i].srcOffset + ChunkOffset, 0, ChunkSize);

					// Read staging buffer and copy into host memory aDestination Data.
					StagingBuffer.read(aDestinationData, 0, aRegionList[i].dstOffset + ChunkOffset, ChunkSize);

					// Recalculate remaining data to send.
					Remainder -= ChunkSize;

				} while (Remainder > 0);

			}

		}

		return Result;
	}

	VkBufferMemoryBarrier buffer::memory_barrier(
			uint aSrcAccess, uint aDstAccess,
			size_t aOffset, size_t aSize
	) const {
		VkBufferMemoryBarrier MemoryBarrier{};
		MemoryBarrier.sType						= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		MemoryBarrier.pNext						= NULL;
		MemoryBarrier.srcAccessMask				= aSrcAccess;
		MemoryBarrier.dstAccessMask				= aDstAccess;
		MemoryBarrier.srcQueueFamilyIndex		= VK_QUEUE_FAMILY_IGNORED;
		MemoryBarrier.dstQueueFamilyIndex		= VK_QUEUE_FAMILY_IGNORED;
		MemoryBarrier.buffer					= this->Handle;
		MemoryBarrier.offset					= aOffset;
		MemoryBarrier.size						= std::min(aSize, this->CreateInfo.size - aOffset);
		return MemoryBarrier;
	}

	VkMemoryRequirements buffer::memory_requirements() const {
		return this->Context->get_buffer_memory_requirements(this->Handle);
	}

	VkBuffer& buffer::handle() {
		return this->Handle;
	}

	void buffer::clear() {
		if (Context != nullptr) {
			if (Handle != VK_NULL_HANDLE) {
				vkDestroyBuffer(Context->handle(), Handle, NULL);
			}
			Context->free_memory(MemoryHandle);
		}
		this->zero_out();
	}

	void buffer::zero_out() {
		this->Context		= nullptr;
		this->CreateInfo 	= {};
		this->Handle 		= VK_NULL_HANDLE;
		this->MemoryType 	= 0;
		this->MemoryHandle 	= VK_NULL_HANDLE;
	}

}
