#include <geodesuka/engine.h>
//#include <geodesuka/core/gcl/device.h>

#include <cstdlib>
#include <cstring>

#include <sstream>

#include <GLFW/glfw3.h>

#include <geodesuka/core/util/log.h>

namespace geodesuka::core::gcl {

	using namespace util;

	device::device(engine* aEngine, VkPhysicalDevice aPhysicalDevice) {
		VkResult Result = VK_SUCCESS;
		uint32_t Count = 0;

		this->ParentInstance = aEngine->handle();
		this->Handle = aPhysicalDevice;

		// Gather all queue families.
		/*Result =*/vkGetPhysicalDeviceQueueFamilyProperties(this->Handle, &Count, NULL);
		this->QueueFamily = std::vector<qfp>(Count);
		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(Count);
		/*Result =*/vkGetPhysicalDeviceQueueFamilyProperties(this->Handle, &Count, QueueFamilyProperties.data());

		// Create Temp Window
		// This section is neccessary to create dummy surface
		// to test for presentation support. This makes the assumption
		// that presentation support does not depend on the parameters
		// of the surface created.
		glfwWindowHint(GLFW_RESIZABLE,			GLFW_TRUE);
		glfwWindowHint(GLFW_DECORATED,			GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUSED,			GLFW_TRUE);
		glfwWindowHint(GLFW_AUTO_ICONIFY,		GLFW_TRUE);
		glfwWindowHint(GLFW_FLOATING,			GLFW_FALSE);
		glfwWindowHint(GLFW_MAXIMIZED,			GLFW_FALSE);
		glfwWindowHint(GLFW_VISIBLE,			GLFW_FALSE);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR,	GLFW_FALSE);
		glfwWindowHint(GLFW_CENTER_CURSOR,		GLFW_TRUE);
		glfwWindowHint(GLFW_FOCUS_ON_SHOW,		GLFW_TRUE);
		glfwWindowHint(GLFW_CLIENT_API,			GLFW_NO_API);
		glfwWindowHint(GLFW_REFRESH_RATE,		GLFW_DONT_CARE);

		// Create OS window handle.
		GLFWwindow* DummyWindow = glfwCreateWindow(640, 480, "Dummy Window", NULL, NULL);

		if (DummyWindow == NULL) {
			// If window creation fails, abort.
			aEngine->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "device", "", "Failed to create dummy window to test for PRESENT SUPPORT!" );
			return;
		}

		// Reset hints to insure no lingering parameters.
		glfwDefaultWindowHints();

		// Since this code only relies on an instance, maybe query in device class?
		VkSurfaceKHR DummySurface = VK_NULL_HANDLE;
		Result = glfwCreateWindowSurface(aEngine->handle(), DummyWindow, NULL, &DummySurface);

		if (DummySurface == VK_NULL_HANDLE){
			// If surface creation fails, abort.
			aEngine->Log << log::message(log::ERROR, log::INITIALIZATION_FAILED, "Engine Startup", log::GEODESUKA, "device", "", "Failed to create dummy surface to test for PRESENT SUPPORT!");
			glfwDestroyWindow(DummyWindow);
			DummyWindow = NULL;
			return;
		}

		// Get all supported operation types.
		for (size_t i = 0; i < QueueFamilyProperties.size(); i++) {

			// Loads all supported operations
			this->QueueFamily[i].SupportedOperations = QueueFamilyProperties[i].queueFlags;
			VkBool32 PresentSupport = VK_FALSE;
			Result = vkGetPhysicalDeviceSurfaceSupportKHR(aPhysicalDevice, i, DummySurface, &PresentSupport);
			if (PresentSupport == VK_TRUE) {
				this->QueueFamily[i].SupportedOperations |= operation::PRESENT;
			}

			// Counts 
			this->QueueFamily[i].OperationCount = this->operation_count(this->QueueFamily[i].SupportedOperations);
			this->QueueFamily[i].QueueCount = QueueFamilyProperties[i].queueCount;
		}

		vkGetPhysicalDeviceFeatures(this->Handle, &this->Features);

		// Clear up Dummy stuff.
		vkDestroySurfaceKHR(aEngine->handle(), DummySurface, NULL);
		DummySurface = VK_NULL_HANDLE;
		glfwDestroyWindow(DummyWindow);
		DummyWindow = NULL;

		/*
		vkGetPhysicalDeviceProperties(this->Handle, &this->Properties);
		vkGetPhysicalDeviceFeatures(this->Handle, &this->Features);
		vkGetPhysicalDeviceMemoryProperties(this->Handle, &this->MemoryProperties);

		std::stringstream StringStream;
		StringStream << "\n\t" << "Queue Operations Supported: ";
		if (this->qfi(device::operation::TRANSFER) != -1) {
			StringStream << "T";
		}
		if (this->qfi(device::operation::COMPUTE) != -1) {
			StringStream << "C";
		}
		if (this->qfi(device::operation::GRAPHICS) != -1) {
			StringStream << "G";
		}
		if (this->qfi(device::operation::PRESENT) != -1) {
			StringStream << "P";
		}

		StringStream << "\n\t" << "API Version:\t" << Properties.apiVersion;
		StringStream << "\n\t" << "Driver Version:\t" << Properties.driverVersion;
		StringStream << "\n\t" << "Vendor ID:\t" << Properties.vendorID;
		StringStream << "\n\t" << "Device ID:\t" << Properties.deviceID;
		StringStream << "\n\t" << "Device Type:\t";
		switch (Properties.deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			StringStream << "Other";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			StringStream << "Integrated GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			StringStream << "Discrete GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			StringStream << "Virtual GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			StringStream << "CPU";
			break;
		default:
			StringStream << "Unknown";
			break;
		}
		StringStream << "\n\t" << "Device Name:\t" << Properties.deviceName;

		for (int i = 0; i < MemoryProperties.memoryHeapCount; i++) {
			StringStream << "\n\t" << "Heap[" << i << "] Size:\t" << MemoryProperties.memoryHeaps[i].size << " bytes";
			for (int j = 0; j < MemoryProperties.memoryTypeCount; j++) {
				if ((MemoryProperties.memoryTypes[j].heapIndex == i) && (MemoryProperties.memoryTypes[j].propertyFlags != 0)) {
					StringStream << "\n\t\t" << "Type[" << j << "]: | ";
					for (int k = 0; k < 9; k++) {
						switch (MemoryProperties.memoryTypes[j].propertyFlags & (1 << k)) {
						case VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:
							StringStream << "Device Local";
							break;
						case VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT:
							StringStream << "Host Visible";
							break;
						case VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:
							StringStream << "Host Coherent";
							break;
						case VK_MEMORY_PROPERTY_HOST_CACHED_BIT:
							StringStream << "Host Cached";
							break;
						case VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT:
							StringStream << "Lazily Allocated";
							break;
						case VK_MEMORY_PROPERTY_PROTECTED_BIT:
							StringStream << "Protected";
							break;
						case VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD:
							StringStream << "Device Coherent (AMD)";
							break;
						case VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD:
							StringStream << "Device Uncached (AMD)";
							break;
						case VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV:
							StringStream << "RDMA Capable (NV)";
							break;
						default:
							break;
						}
						if (MemoryProperties.memoryTypes[j].propertyFlags & (1 << k))
							StringStream << " | ";
					}
				}
			}
		}

		*aEngine << log::message(log::INFO, log::SUCCESS, "Engine Startup", log::GEODESUKA, "device", Properties.deviceName, StringStream.str().c_str());
		//*/

	}

	device::~device() {}

	std::vector<device::qfp> device::get_queue_family_properties() const {
		return this->QueueFamily;
	}

	std::vector<VkExtensionProperties> device::get_extensions() const {
		VkResult Result = VK_SUCCESS;
		uint32_t Count = 0;
		// Gather all extensions.
		Result = vkEnumerateDeviceExtensionProperties(this->Handle, NULL, &Count, NULL);
		std::vector<VkExtensionProperties> Extension(Count);
		Result = vkEnumerateDeviceExtensionProperties(this->Handle, NULL, &Count, Extension.data());
		return Extension;
	}

	VkPhysicalDeviceProperties device::get_properties() const {
		VkPhysicalDeviceProperties temp{};
		if (this->Handle != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceProperties(this->Handle, &temp);
		}
		return temp;
	}

	VkPhysicalDeviceFeatures device::get_features() const {
		VkPhysicalDeviceFeatures temp{};
		if (this->Handle != VK_NULL_HANDLE) {
			vkGetPhysicalDeviceFeatures(this->Handle, &temp);
		}
		return temp;
	}

	VkPhysicalDeviceMemoryProperties device::get_memory_properties() const {
		VkPhysicalDeviceMemoryProperties temp;
		vkGetPhysicalDeviceMemoryProperties(this->Handle, &temp);
		return temp;
	}

	// This function iterates through VkPhysicalDeviceMemoryProperties which is structure object containing
	// a list of memory types and heaps available from the device, along with a parameter of aMemoryRequirements
	// which is the memory requirements of the object in question. VkPhysicalDeviceMemoryProperties contains
	// a list of memory types available, and a detailed list of memory heaps which can be allocated from.
	// aMemoryRequirements contains a bit mask of valid memory types that can be used with the object in question.
	// This function iterates through a list of available memory types and selects based on the requirements
	// of aMemoryRequirements, and the chosen memory type. It will first search for an exact memory type,
	// but if does not have exact memory types to suit the object, it will search for an approximate memory type
	// which may have additional features. Will return -1 if no such memory type heap exists.
	int device::get_memory_type_index(VkMemoryRequirements aMemoryRequirements, uint aMemoryType) const {
		int MemoryTypeIndex = -1;
		VkPhysicalDeviceMemoryProperties MemoryProperties = this->get_memory_properties();

		// Search for exact memory type index.
		for (uint32_t i = 0; i < MemoryProperties.memoryTypeCount; i++) {
			if (((aMemoryRequirements.memoryTypeBits & (1 << i)) >> i) && (MemoryProperties.memoryTypes[i].propertyFlags == aMemoryType)) {
				MemoryTypeIndex = i;
				break;
			}
		}

		// Search for approximate memory type index.
		if (MemoryTypeIndex == -1) {
			for (uint32_t i = 0; i < MemoryProperties.memoryTypeCount; i++) {
				if (((aMemoryRequirements.memoryTypeBits & (1 << i)) >> i) && ((MemoryProperties.memoryTypes[i].propertyFlags & aMemoryType) == aMemoryType)) {
					MemoryTypeIndex = i;
					break;
				}
			}
		}

		return MemoryTypeIndex;
	}

	int device::get_memory_type(int aMemoryTypeIndex) {
		if (aMemoryTypeIndex < 0) return 0;
		VkPhysicalDeviceMemoryProperties MemoryProperties = this->get_memory_properties();
		return MemoryProperties.memoryTypes[aMemoryTypeIndex].propertyFlags;
	}

	VkSurfaceCapabilitiesKHR device::get_surface_capabilities(VkSurfaceKHR aSurface) {
		VkSurfaceCapabilitiesKHR SurfaceCapabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->Handle, aSurface, &SurfaceCapabilities);
		return SurfaceCapabilities;
	}

	std::vector<VkSurfaceFormatKHR> device::get_surface_format(VkSurfaceKHR aSurface) {
		VkResult Result = VK_SUCCESS;
		uint32_t VectorCount = 0;
		Result = vkGetPhysicalDeviceSurfaceFormatsKHR(this->Handle, aSurface, &VectorCount, NULL);
		std::vector<VkSurfaceFormatKHR> SurfaceFormat(VectorCount);
		Result = vkGetPhysicalDeviceSurfaceFormatsKHR(this->Handle, aSurface, &VectorCount, SurfaceFormat.data());
		return SurfaceFormat;
	}

	std::vector<VkPresentModeKHR> device::get_surface_present_mode(VkSurfaceKHR aSurface) {
		VkResult Result = VK_SUCCESS;
		uint32_t VectorCount = 0;
		Result = vkGetPhysicalDeviceSurfacePresentModesKHR(this->Handle, aSurface, &VectorCount, NULL);
		std::vector<VkPresentModeKHR> PresentMode(VectorCount);
		Result = vkGetPhysicalDeviceSurfacePresentModesKHR(this->Handle, aSurface, &VectorCount, PresentMode.data());
		return PresentMode;
	}

	bool device::is_extension_list_supported(std::vector<const char*> aExtension) const {
		// Checks if ExtensionList is a subset of existing extensions.
		bool isSupported = true;
		const std::vector<VkExtensionProperties> Extension = this->get_extensions();
		for (const char* L : aExtension) {
			bool ExistsInSet = false;
			for (const VkExtensionProperties& R : Extension) {
				ExistsInSet = (strcmp(L, R.extensionName) == 0);
				if (ExistsInSet) break;
			}
			isSupported &= ExistsInSet;
			if (!isSupported) break;
		}
		return isSupported;
	}

	bool device::available(uint aOperation) const {
		return (this->qfi(aOperation) != -1);
	}

	int device::qfi(uint aOp) const {
		uint LOC = UINT32_MAX;
		int LOCI = -1;
		for (size_t i = 0; i < QueueFamily.size(); i++) {
			if (((QueueFamily[i].SupportedOperations & aOp) == aOp) && (LOC > QueueFamily[i].OperationCount)) {
				LOC = QueueFamily[i].OperationCount;
				LOCI = i;
			}
		}
		return LOCI;
	}

	int device::qfc(int aQFI) const {
		return this->QueueFamily[aQFI].QueueCount;
	}

	VkInstance device::inst() {
		return this->ParentInstance;
	}

	VkPhysicalDevice device::handle() {
		return this->Handle;
	}

	uint device::operation_count(uint aOperation) const {
		uint OperationCount = 0;
		for (int i = 0; i < 32; i++) {
			if (aOperation & (1 << i)) {
				OperationCount++;
			}
		}
		return OperationCount;
	}

	// device::operation operator|(device::operation aLhs, device::operation aRhs) {
	// 	uint Combined = (uint)aLhs | (uint)aRhs;
	// 	return (device::operation)Combined;
	// }

	// device::operation operator&(device::operation aLhs, device::operation aRhs) {
	// 	uint Combined = (uint)aLhs & (uint)aRhs;
	// 	return (device::operation)Combined;
	// }

	// device::memory operator|(device::memory aLhs, device::memory aRhs) {
	// 	uint Combined = (uint)aLhs | (uint)aRhs;
	// 	return (device::memory)Combined;
	// }

	// device::memory operator&(device::memory aLhs, device::memory aRhs) {
	// 	uint Combined = (uint)aLhs & (uint)aRhs;
	// 	return (device::memory)Combined;
	// }

}