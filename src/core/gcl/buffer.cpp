#include <geodesuka/core/gcl/buffer.h>

#include <cstdlib>
#include <cstring>
#include <assert.h>

#include <vector>

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

		// Allocate Device Memory.
		Result = this->create_device_memory(aContext, aCreateInfo, TotalSize);

		// Write to created buffer
		if (aVertexData != NULL) {
			Result = this->write(TotalSize, aVertexData);
		}
	}

	buffer::buffer(context* aContext, create_info aCreateInfo, size_t aBufferSize, void* aBufferData) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		// Allocate Device Memory.
		Result = this->create_device_memory(aContext, aCreateInfo, aBufferSize);

		// Write to created buffer
		if (aBufferData != NULL) {
			Result = this->write(aBufferSize, aBufferData);
		}
	}

	buffer::buffer(context* aContext, uint aMemoryType, uint aBufferUsage, int aVertexCount, variable aVertexLayout, void* aVertexData) {
		VkResult Result = VK_SUCCESS;
		size_t TotalSize = aVertexCount * aVertexLayout.size();
		this->zero_out();

		// Create memory for vertex buffer.
		Result = this->create_device_memory(aContext, create_info(aMemoryType, aBufferUsage), TotalSize);

		// Write to created buffer
		if (aVertexData != NULL) {
			Result = this->write(TotalSize, aVertexData);
		}		
	}

	buffer::buffer(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aBufferSize, void* aBufferData) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		// Create memory for vertex buffer.
		Result = this->create_device_memory(aContext, create_info(aMemoryType, aBufferUsage), aBufferSize);
		aContext->parent_engine();
		// Write to created buffer
		if (aBufferData != NULL) {
			Result = this->write(aBufferSize, aBufferData);
		}
	}

	buffer::buffer(buffer& aInput) {
		VkResult Result = VK_SUCCESS;

		// Zero out new object.
		this->zero_out();

		if (aInput.Context != nullptr) {
			create_info RecreateInfo;
			RecreateInfo.MemoryType		= aInput.Context->parent()->get_memory_type(aInput.AllocateInfo.memoryTypeIndex);
			RecreateInfo.BufferUsage	= aInput.CreateInfo.usage;
			this->VertexCount			= aInput.VertexCount;
			this->VertexLayout			= aInput.VertexLayout;
			Result = this->create_device_memory(aInput.Context, RecreateInfo, aInput.CreateInfo.size);
		}

		if (Result == VK_SUCCESS) {
			VkFence Fence = VK_NULL_HANDLE;
			VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;

			Fence = this->Context->create_fence();
			CommandBuffer = (*this << aInput);

			Result = this->Context->execute(device::operation::TRANSFER, CommandBuffer, Fence);
			Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

			this->Context->destroy_command_buffer(device::operation::TRANSFER, CommandBuffer);
			this->Context->destroy_fence(Fence);
		}
	}

	buffer::buffer(buffer&& aInput) noexcept {
		this->VertexCount	= aInput.VertexCount;
		this->VertexLayout	= aInput.VertexLayout;
		this->Context		= aInput.Context;
		this->CreateInfo	= aInput.CreateInfo;
		this->Handle		= aInput.Handle;
		this->AllocateInfo	= aInput.AllocateInfo;
		this->MemoryHandle	= aInput.MemoryHandle;
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
			create_info RecreateInfo;
			RecreateInfo.MemoryType		= aRhs.Context->parent()->get_memory_type(aRhs.AllocateInfo.memoryTypeIndex);
			RecreateInfo.BufferUsage	= aRhs.CreateInfo.usage;
			this->VertexCount			= aRhs.VertexCount;
			this->VertexLayout			= aRhs.VertexLayout;
			Result = this->create_device_memory(aRhs.Context, RecreateInfo, aRhs.CreateInfo.size);
		}

		if (Result == VK_SUCCESS) {
			VkFence Fence = this->Context->create_fence();
			VkCommandBuffer CommandBuffer = (*this << aRhs);

			Result = this->Context->execute(device::operation::TRANSFER, CommandBuffer, Fence);
			Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

			this->Context->destroy_command_buffer(device::operation::TRANSFER, CommandBuffer);
			this->Context->destroy_fence(Fence);
		}

		return *this;
	}

	buffer& buffer::operator=(buffer&& aRhs) noexcept {
		this->clear_device_memory();
		this->VertexCount	= aRhs.VertexCount;
		this->VertexLayout	= aRhs.VertexLayout;
		this->Context		= aRhs.Context;
		this->CreateInfo	= aRhs.CreateInfo;
		this->Handle		= aRhs.Handle;
		this->AllocateInfo	= aRhs.AllocateInfo;
		this->MemoryHandle	= aRhs.MemoryHandle;
		aRhs.zero_out();
		return *this;
	}

	VkCommandBuffer buffer::operator<<(buffer& aRhs) {
		VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
		// Both operands must share the same parent context, and have same memory size.
		if ((this->Context != aRhs.Context) || ((size_t)this->CreateInfo.size != aRhs.CreateInfo.size)) return CommandBuffer;
		// Left operand must have TRANSFER_DST flag enabled, and Right operand must have TRANSFER_SRC flag enabled.
		if (
			((this->CreateInfo.usage & buffer::usage::TRANSFER_DST) != buffer::usage::TRANSFER_DST)
			||
			((aRhs.CreateInfo.usage & buffer::usage::TRANSFER_SRC) != buffer::usage::TRANSFER_SRC)
		) return CommandBuffer;

		VkResult Result = VK_SUCCESS;
		VkCommandBufferBeginInfo BeginInfo{};
		VkBufferCopy Region{};

		BeginInfo.sType						= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext						= NULL;
		BeginInfo.flags						= 0;
		BeginInfo.pInheritanceInfo			= NULL;

		Region.srcOffset					= 0;
		Region.dstOffset					= 0;
		Region.size							= this->CreateInfo.size;

		CommandBuffer = this->Context->create_command_buffer(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		vkCmdCopyBuffer(CommandBuffer, aRhs.Handle, this->Handle, 1, &Region);
		Result = vkEndCommandBuffer(CommandBuffer);
		return CommandBuffer;
	}

	VkCommandBuffer buffer::operator>>(buffer& aRhs) {
		return (aRhs << *this);
	}

	VkCommandBuffer buffer::operator<<(image& aRhs) {
		VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
		// Must share the same parent context and have the same size.
		if ((this->Context != aRhs.Context) || ((size_t)this->CreateInfo.size != aRhs.get_memory_size())) return CommandBuffer;
		// buffer must have enabled, DST flag. texture must have SRC flag.
		if (
			((this->CreateInfo.usage & buffer::usage::TRANSFER_DST) != buffer::usage::TRANSFER_DST) 
			|| 
			((aRhs.CreateInfo.usage & image::usage::TRANSFER_SRC) != image::usage::TRANSFER_SRC)
		) return CommandBuffer;

		VkResult Result = VK_SUCCESS;

		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType							= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext							= NULL;
		BeginInfo.flags							= 0;
		BeginInfo.pInheritanceInfo				= NULL;

		VkBufferImageCopy Region{};
		Region.bufferOffset						= 0;
		Region.bufferRowLength					= 0;
		Region.bufferImageHeight				= 0;
		Region.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		Region.imageSubresource.mipLevel		= 0;
		Region.imageSubresource.baseArrayLayer	= 0;
		Region.imageSubresource.layerCount		= aRhs.CreateInfo.arrayLayers;
		Region.imageOffset						= { 0, 0, 0 };
		Region.imageExtent						= aRhs.CreateInfo.extent;

		CommandBuffer = this->Context->create_command_buffer(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		aRhs.cmd_transition(CommandBuffer,
			VK_ACCESS_MEMORY_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 1,
			0, aRhs.CreateInfo.arrayLayers,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		);
		vkCmdCopyImageToBuffer(CommandBuffer,
			aRhs.Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			this->Handle,
			1, &Region
		);
		Result = vkEndCommandBuffer(CommandBuffer);

		return CommandBuffer;
	}

	VkCommandBuffer buffer::operator>>(image& aRhs) {
		return (aRhs >> *this);
	}

	VkResult buffer::write(size_t aBufferSize, void* aBufferData) {
		return this->write(0, 0, aBufferSize, aBufferSize, aBufferData);
	}

	VkResult buffer::write(size_t aSrcOffset, size_t aDstOffset, size_t aRegionSize, size_t aBufferSize, void* aBufferData) {
		VkBufferCopy Region{};
		Region.srcOffset	= aSrcOffset;
		Region.dstOffset	= aDstOffset;
		Region.size			= aRegionSize;
		return this->write(1, &Region, aBufferSize, aBufferData);
	}

	VkResult buffer::write(uint32_t aRegionCount, VkBufferCopy* aRegionList, size_t aBufferSize, void* aBufferData) {
		if ((this->Context == nullptr) || (aRegionCount == 0) || (aRegionList == NULL) || (aBufferData == NULL)) return VK_ERROR_FORMAT_NOT_SUPPORTED;

		VkResult Result = VK_SUCCESS;
		if ((this->Context->parent()->get_memory_type(this->AllocateInfo.memoryTypeIndex) & device::memory::HOST_VISIBLE) == device::memory::HOST_VISIBLE) {
			// Host Visible, write to directly
			void* nptr = NULL;
			Result = vkMapMemory(this->Context->handle(), this->MemoryHandle, 0, VK_WHOLE_SIZE, 0, &nptr);
			if ((nptr == NULL) || (Result != VK_SUCCESS)) return VK_ERROR_FORMAT_NOT_SUPPORTED;
			for (uint32_t i = 0; i < aRegionCount; i++) {
				memcpy((void*)((uintptr_t)nptr + (uintptr_t)aRegionList[i].dstOffset), (void*)((uintptr_t)aBufferData + (uintptr_t)aRegionList[i].srcOffset), aRegionList[i].size);
			}
			vkUnmapMemory(this->Context->handle(), this->MemoryHandle);
		}
		else {
			// Not Host Visible, use staging buffer.
			buffer StagingBuffer(
				Context,
				device::memory::HOST_VISIBLE | device::memory::HOST_COHERENT,
				buffer::TRANSFER_SRC,
				aBufferSize,
				aBufferData
			);

			VkFence Fence = this->Context->create_fence();
			VkCommandBuffer CommandBuffer = this->Context->create_command_buffer(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			VkCommandBufferBeginInfo BeginInfo{};	
			BeginInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			BeginInfo.pNext					= NULL;
			BeginInfo.flags					= 0;
			BeginInfo.pInheritanceInfo		= NULL;

			Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
			vkCmdCopyBuffer(CommandBuffer, StagingBuffer.Handle, this->Handle, aRegionCount, aRegionList);
			Result = vkEndCommandBuffer(CommandBuffer);

			Result = this->Context->execute(device::operation::TRANSFER, CommandBuffer, Fence);
			Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

			this->Context->destroy_command_buffer(device::operation::TRANSFER, CommandBuffer);
			this->Context->destroy_fence(Fence);
		}
		return Result;
	}

	VkResult buffer::read(size_t aBufferSize, void* aBufferData) {
		return this->read(0, 0, aBufferSize, aBufferSize, aBufferData);
	}

	VkResult buffer::read(size_t aSrcOffset, size_t aDstOffset, size_t aRegionSize, size_t aBufferSize, void* aBufferData) {
		VkBufferCopy Region{};
		Region.srcOffset	= aSrcOffset;
		Region.dstOffset	= aDstOffset;
		Region.size			= aRegionSize;
		return this->read(1, &Region, aBufferSize, aBufferData);
	}

	// In the case of the read operation, the 
	VkResult buffer::read(uint32_t aRegionCount, VkBufferCopy* aRegionList, size_t aBufferSize, void* aBufferData) {
		if ((this->Context == nullptr) || (aRegionCount == 0) || (aRegionList == NULL) || (aBufferData == NULL)) return VK_ERROR_FORMAT_NOT_SUPPORTED;
		
		VkResult Result = VK_SUCCESS;
		if ((this->Context->parent()->get_memory_type(this->AllocateInfo.memoryTypeIndex) & device::memory::HOST_VISIBLE) == device::memory::HOST_VISIBLE) {
			// Visible, direct write through API
			void* nptr = NULL;
			Result = vkMapMemory(this->Context->handle(), this->MemoryHandle, 0, VK_WHOLE_SIZE, 0, &nptr);
			if ((nptr == NULL) || (Result != VK_SUCCESS)) return VK_ERROR_FORMAT_NOT_SUPPORTED;
			for (uint32_t i = 0; i < aRegionCount; i++) {
				memcpy((void*)((uintptr_t)aBufferData + (uintptr_t)aRegionList[i].dstOffset), (void*)((uintptr_t)nptr + (uintptr_t)aRegionList[i].srcOffset), aRegionList[i].size);
			}
			vkUnmapMemory(this->Context->handle(), this->MemoryHandle);
		}
		else {
			// Not Host Visible, use staging buffer.
			buffer StagingBuffer(
				Context,
				device::memory::HOST_VISIBLE | device::memory::HOST_COHERENT,
				buffer::TRANSFER_SRC,
				aBufferSize,
				NULL
			);

			VkFence Fence = this->Context->create_fence();
			VkCommandBuffer CommandBuffer = this->Context->create_command_buffer(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

			VkCommandBufferBeginInfo BeginInfo{};
			BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			BeginInfo.pNext = NULL;
			BeginInfo.flags = 0;
			BeginInfo.pInheritanceInfo = NULL;

			Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
			vkCmdCopyBuffer(CommandBuffer, this->Handle, StagingBuffer.Handle, aRegionCount, aRegionList);
			Result = vkEndCommandBuffer(CommandBuffer);

			// Copy data to staging buffer.
			Result = this->Context->execute(device::operation::TRANSFER, CommandBuffer, Fence);
			Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

			this->Context->destroy_command_buffer(device::operation::TRANSFER, CommandBuffer);
			this->Context->destroy_fence(Fence);

			// Read data from staging buffer.
			Result = StagingBuffer.read(aRegionCount, aRegionList, aBufferSize, aBufferData);
		}

		return Result;
	}

	size_t buffer::get_memory_size() const {
		return this->CreateInfo.size;
	}

	VkBuffer& buffer::handle() {
		return this->Handle;
	}

	VkResult buffer::create_device_memory(context* aContext, create_info aCreateInfo, size_t aMemorySize) {
		VkResult Result = VK_SUCCESS;

		// Return if an improper context is provided.
		if (aContext == nullptr) VK_ERROR_INITIALIZATION_FAILED;
		this->Context = aContext;

		// Create buffer handle
		if (Result == VK_SUCCESS) {
			this->CreateInfo = make_buffer_create_info(aMemorySize, aCreateInfo.BufferUsage);
			Result = vkCreateBuffer(this->Context->handle(), &this->CreateInfo, NULL, &this->Handle);
		}


		if (Result == VK_SUCCESS) {
			this->AllocateInfo = make_buffer_allocate_info(this->Context, this->Context->parent(), aCreateInfo.MemoryType);
			Result = vkAllocateMemory(this->Context->handle(), &this->AllocateInfo, NULL, &this->MemoryHandle);
		}
		else {
			// Buffer creatation failed.
			this->clear_device_memory();
			return Result;
		}

		if (Result == VK_SUCCESS) {
			Result = vkBindBufferMemory(this->Context->handle(), this->Handle, this->MemoryHandle, 0);
		}
		else {
			// Memory allocation failure.
			this->clear_device_memory();
			return Result;
		}

		return Result;
	}

	VkBufferCreateInfo buffer::make_buffer_create_info(size_t aMemorySize, int aBufferUsage) {
		VkBufferCreateInfo NewBufferCreateInfo{};
		NewBufferCreateInfo.sType						= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		NewBufferCreateInfo.pNext						= NULL;
		NewBufferCreateInfo.flags						= 0; // Ignore.
		NewBufferCreateInfo.size						= aMemorySize;
		NewBufferCreateInfo.usage						= (VkBufferUsageFlags)(aBufferUsage | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST); // Enable Transfer
		NewBufferCreateInfo.sharingMode					= VK_SHARING_MODE_EXCLUSIVE;
		NewBufferCreateInfo.queueFamilyIndexCount		= 0;
		NewBufferCreateInfo.pQueueFamilyIndices			= NULL;
		return NewBufferCreateInfo;
	}

	VkMemoryAllocateInfo buffer::make_buffer_allocate_info(context* aContext, device* aDevice, int aMemoryType) {
		VkMemoryAllocateInfo NewAllocateInfo{};
		VkMemoryRequirements MemoryRequirement = Context->get_buffer_memory_requirements(this->Handle);
		NewAllocateInfo.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		NewAllocateInfo.pNext			= NULL;
		NewAllocateInfo.allocationSize	= MemoryRequirement.size;
		NewAllocateInfo.memoryTypeIndex	= aDevice->get_memory_type_index(MemoryRequirement, aMemoryType);
		return NewAllocateInfo;
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
		this->Context = nullptr;
		this->CreateInfo = {};
		this->AllocateInfo = {};
		this->VertexCount = 0;
		this->VertexLayout = variable();
	}

	void buffer::zero_out() {
		this->Context = nullptr;
		this->CreateInfo = {};
		this->Handle = VK_NULL_HANDLE;
		this->AllocateInfo = {};
		this->MemoryHandle = VK_NULL_HANDLE;
		this->VertexCount = 0;
		this->VertexLayout = variable();
	}

}
