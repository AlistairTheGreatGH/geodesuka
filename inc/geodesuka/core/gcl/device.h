#pragma once
#ifndef GEODESUKA_CORE_GCL_DEVICE_H
#define GEODESUKA_CORE_GCL_DEVICE_H

#include "../math/type.h"

#include <vector>

#include "config.h"

namespace geodesuka::core::gcl {

	class device {
	public:

		friend class engine;
		friend class context;

		enum operation : uint {
   			GRAPHICS       			= VK_QUEUE_GRAPHICS_BIT,
   			COMPUTE        			= VK_QUEUE_COMPUTE_BIT,
			GRAPHICS_AND_COMPUTE 	= GRAPHICS | COMPUTE,
   			TRANSFER       			= VK_QUEUE_TRANSFER_BIT,
   			SPARSE_BINDING 			= VK_QUEUE_SPARSE_BINDING_BIT,
   			DRM_PROTECTED      		= VK_QUEUE_PROTECTED_BIT,
   			VIDEO_DECODE   			= VK_QUEUE_VIDEO_DECODE_BIT_KHR,
#ifdef VK_ENABLE_BETA_EXTENSIONS
			VIDEO_ENCODE 			= VK_QUEUE_VIDEO_ENCODE_BIT_KHR,
#endif
			OPTICAL_FLOW 			= VK_QUEUE_OPTICAL_FLOW_BIT_NV,
			PRESENT 				= 0x80000000
		};

		enum memory : uint {
      		DEVICE_LOCAL     		= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
      		HOST_VISIBLE     		= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
      		HOST_COHERENT    		= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
      		HOST_CACHED      		= VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
      		LAZILY_ALLOCATED 		= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT,
      		PROTECTED        		= VK_MEMORY_PROPERTY_PROTECTED_BIT,
      		DEVICE_COHERENT  		= VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD,
      		DEVICE_UNCACHED  		= VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD,
      		RDMA_CAPABLE     		= VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV,
		};
		
		struct qfp {
			uint SupportedOperations; 		// Contains List of Operations supported by this Queue Family.
			uint OperationCount;			// Total Number of Unique Operations in the Queue Family.
			uint QueueCount;				// Total Number of Queues in the Queue Family
		};

		// Managed by engine, should be generated by engine.
		device(engine* aEngine, VkPhysicalDevice aPhysicalDevice);
		~device();

		// ----- Query ----- //

		std::vector<qfp> get_queue_family_properties() const;
		std::vector<VkExtensionProperties> get_extensions() const;
		VkPhysicalDeviceProperties get_properties() const;
		VkPhysicalDeviceFeatures get_features() const;
		VkPhysicalDeviceMemoryProperties get_memory_properties() const;

		int get_memory_type_index(VkMemoryRequirements aMemoryRequirements, uint aMemoryType) const;
		int get_memory_type(int aMemoryTypeIndex);

		VkSurfaceCapabilitiesKHR get_surface_capabilities(VkSurfaceKHR aSurface);
		std::vector<VkSurfaceFormatKHR> get_surface_format(VkSurfaceKHR aSurface);
		std::vector<VkPresentModeKHR> get_surface_present_mode(VkSurfaceKHR aSurface);

		//VkImageFormatProperties image_format_properties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags);

		// User can provide a list of extensions to this method to check if they are all
		// supported. Will return true if provided list of extensions is supported. Will
		// return false if at least one extension is not supported.
		bool is_extension_list_supported(std::vector<const char*> aExtension) const;

		// This funcion merely checks if the operations provided is supported by device instance.
		bool available(uint aOperation) const;

		// Function will attempt to find a Queue Family Index (QFI) that supports the desired operations, 
		// exclusively while minimizing the number of extraneous operations. This is to insure the exclusivity
		// of different execution queues for a device context. This function will return -1 if this device
		// does not support the desired operation.
		int qfi(uint aOperation) const;

		// ----- Handles ----- //

		VkInstance inst();
		VkPhysicalDevice handle();

	private:

		//std::vector<object::system_display*> ChildDisplay;

		VkInstance 								ParentInstance;
		VkPhysicalDevice 						Handle;
		std::vector<qfp> 						QueueFamily;

		// Internally calculates the number of operations in qfi operation.
		uint operation_count(uint aOperation) const;
		
	};

	// device::operation operator|(device::operation aLhs, device::operation aRhs);
	// device::operation operator&(device::operation aLhs, device::operation aRhs);

	// device::memory operator|(device::memory aLhs, device::memory aRhs);	
	// device::memory operator&(device::memory aLhs, device::memory aRhs);	

}

#endif // !GEODESUKA_CORE_GCL_DEVICE_H
