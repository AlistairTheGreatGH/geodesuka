#pragma once
#ifndef GEODESUKA_CORE_GCL_BUFFER_H
#define GEODESUKA_CORE_GCL_BUFFER_H

/*
* Usage:
*	When using this with other functions as non pointer stack type, please pass
*	by reference/pointer. If you pass by value, the constructor/assignment methods
*	will be called and you will unintentionally create, copy and move data on the 
*	device needlessly.
* 
* TODO:
*	-Figure out how to schedule mem transfers with engine backend.
*	-Add an option to use dynamically created staging buffer.
*/

#include "../math.h"

#include "variable.h"

#include "device.h"
#include "context.h"

namespace geodesuka::core::gcl {

	class image;

	class buffer {
	public:

		friend class image;

		enum usage {
			TRANSFER_SRC			= 0x00000001,
			TRANSFER_DST			= 0x00000002,
			UNIFORM_TEXEL			= 0x00000004,
			STORAGE_TEXEL			= 0x00000008,
			UNIFORM					= 0x00000010,
			STORAGE					= 0x00000020,
			INDEX					= 0x00000040,
			VERTEX					= 0x00000080,
			INDIRECT				= 0x00000100,
			SHADER_DEVICE_ADDRESS	= 0x00020000
		};

		struct create_info {
			uint MemoryType;
			uint BufferUsage;
			create_info();
			create_info(uint aMemoryType, uint aBufferUsage);
		};

		buffer();
		buffer(context* aContext, create_info aCreateInfo, int aVertexCount, variable aVertexLayout);
		buffer(context* aContext, create_info aCreateInfo, int aVertexCount, variable aVertexLayout, void* aVertexData);
		buffer(context* aContext, create_info aCreateInfo, size_t aBufferSize);
		buffer(context* aContext, create_info aCreateInfo, size_t aBufferSize, void* aBufferData);
		buffer(context* aContext, uint aMemoryType, uint aBufferUsage, int aCount, variable aMemoryLayout, void* aBufferData);
		buffer(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aMemorySize, void* aBufferData);
		buffer(buffer& aInput);
		buffer(buffer&& aInput) noexcept;
		~buffer();

		buffer& operator=(buffer& aRhs);
		buffer& operator=(buffer&& aRhs) noexcept;
		
		// TRANSFER
		VkCommandBuffer operator<<(buffer& aRhs);
		// TRANSFER
		VkCommandBuffer operator>>(buffer& aRhs);
		// TRANSFER
		VkCommandBuffer operator<<(image& aRhs);
		// TRANSFER
		VkCommandBuffer operator>>(image& aRhs);

		util::list<VkCommandBuffer> copy_to(buffer& aRhs, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize);
		util::list<VkCommandBuffer> copy_to(buffer& aRhs, std::vector<VkBufferCopy> aRegionList);
		util::list<VkCommandBuffer> copy_to(image& aRhs, VkImageLayout dstImageLayout, std::vector<VkBufferImageCopy> aRegionList);

		// Use Command Buffers to update. vkCmdUpdateBuffer() 64kB limit.
		// Has to be host memory to be used.
		VkResult write(size_t aBufferSize, void* aBufferData);
		VkResult write(size_t aSrcOffset, size_t aDstOffset, size_t aRegionSize, size_t aBufferSize, void* aBufferData);
		VkResult write(uint32_t aRegionCount, VkBufferCopy *aRegionList, size_t aBufferSize, void *aBufferData);

		VkResult read(size_t aBufferSize, void* aBufferData);
		VkResult read(size_t aSrcOffset, size_t aDstOffset, size_t aRegionSize, size_t aBufferSize, void* aBufferData);
		VkResult read(uint32_t aRegionCount, VkBufferCopy* aRegionList, size_t aBufferSize, void* aBufferData);

		// Total memory size of the image. (Does not include mip levels)
		size_t get_memory_size() const;

		VkBuffer& handle();

	private:

		int							VertexCount;
		variable					VertexLayout;
		context*					Context;
		VkBufferCreateInfo			CreateInfo{};
		VkBuffer					Handle;
		VkMemoryAllocateInfo		AllocateInfo{};
		VkDeviceMemory				MemoryHandle;

		VkResult create_device_memory(context* aContext, create_info aCreateInfo, size_t aMemorySize);
		VkBufferCreateInfo make_buffer_create_info(size_t aMemorySize, int aBufferUsage);
		VkMemoryAllocateInfo make_buffer_allocate_info(context* aContext, device* aDevice, int aMemoryType);

		void clear_device_memory();

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_BUFFER_H
