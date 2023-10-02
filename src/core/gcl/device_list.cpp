#include <geodesuka/core/gcl/device_list.h>

#include <geodesuka/core/util/list.h>

#include <geodesuka/core/gcl/device.h>

namespace geodesuka::core::gcl {

	device_list::device_list() {}

	device_list::device_list(device* aDevice) {
		this->Handle.push_back(aDevice);
	}

	device_list::device_list(size_t aDeviceCount, device** aDeviceList) {
		this->Handle.resize(aDeviceCount);
		for (size_t i = 0; i < aDeviceCount; i++) {
			this->Handle[i] = aDeviceList[i];
		}
	}

	device* device_list::get_device_type(VkPhysicalDeviceType aType) const {
		// Search for device that is discrete gpu.
		for (size_t i = 0; i < this->Handle.size(); i++) {
			if (this->Handle[i]->get_properties().deviceType == aType) {
				return this->Handle[i];
			}
		}
		return nullptr;
	}

}