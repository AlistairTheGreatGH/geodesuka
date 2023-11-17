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

#include "command_list.h"
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
		buffer(context* aContext, create_info aCreateInfo, int aVertexCount, variable aVertexLayout, void* aVertexData = NULL);
		buffer(context* aContext, uint aMemoryType, uint aBufferUsage, int aVertexCount, variable aVertexLayout, void* aVertexData = NULL);
		buffer(context* aContext, create_info aCreateInfo, size_t aBufferSize, void* aBufferData = NULL);
		buffer(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aBufferSize, void* aBufferData = NULL);
		buffer(buffer& aInput);
		buffer(buffer&& aInput) noexcept;
		~buffer();

		buffer& operator=(buffer& aRhs);
		buffer& operator=(buffer&& aRhs) noexcept;

		// Generates a transfer command which data between other buffers/images.
		command_list copy(buffer& aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize);
		command_list copy(buffer& aSourceData, std::vector<VkBufferCopy> aRegionList);
		command_list copy(image& aSourceData, VkImageLayout aImageLayout, VkBufferImageCopy aRegion);
		command_list copy(image& aSourceData, VkImageLayout aImageLayout, std::vector<VkBufferImageCopy> aRegionList);

		// Write to buffer from host memory data.
		VkResult write(void* aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize);
		VkResult write(void* aSourceData, std::vector<VkBufferCopy> aRegionList);

		// Read from buffer data into host memory.
		VkResult read(void* aDestinationData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize);
		VkResult read(void* aDestinationData, std::vector<VkBufferCopy> aRegionList);

		VkBuffer& handle();

		VkMemoryRequirements get_memory_requirements() const;

	private:

		context*					Context;
		
		size_t 						Size;
		uint 						Usage;
		VkBuffer					Handle;

		uint 						MemoryType;
		VkDeviceMemory				MemoryHandle;

		VkResult create(context* aContext, uint aMemoryType, uint aBufferUsage, size_t aMemorySize, void* aBufferData = NULL);
		VkBuffer create_handle(size_t aSize, uint aUsage);

		void clear_device_memory();

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_BUFFER_H
