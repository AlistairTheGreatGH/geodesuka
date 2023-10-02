#pragma once
#ifndef GEODESUKA_CORE_GCL_DEVICE_LIST_H
#define GEODESUKA_CORE_GCL_DEVICE_LIST_H

#include "../util/list.h"

#include "device.h"

namespace geodesuka::core::gcl {

	class device_list : public util::list<device*> {
	public:

		device_list();
		device_list(device* aDevice);
		device_list(size_t aDeviceCount, device** aDeviceList);

		// Search for support options.
		device* get_device_type(VkPhysicalDeviceType aType) const;

	};

}

#endif // !GEODESUKA_CORE_GCL_DEVICE_LIST_H
