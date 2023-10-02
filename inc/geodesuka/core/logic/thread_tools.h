#pragma once
#ifndef GEODESUKA_CORE_LOGIC_THREAD_TOOLS_H
#define GEODESUKA_CORE_LOGIC_THREAD_TOOLS_H

#include <cstddef>

#include <vector>

namespace geodesuka::core::logic {

	struct workload {
		size_t Offset;
		size_t Size;
	};

	// Calculates Per Thread Workload, evenly distributing work
	std::vector<workload> calculate_workloads(size_t aElementCount, size_t aThreadCount);

}

#endif // !GEODESUKA_CORE_LOGIC_THREAD_TOOLS_H
