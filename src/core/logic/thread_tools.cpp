#include <geodesuka/core/logic/thread_tools.h>

#include <vector>

#include <geodesuka/core/math/type.h>

namespace geodesuka::core::logic {

	std::vector<workload> calculate_workloads(size_t aElementCount, size_t aThreadCount) {
		std::vector<workload> Workload(aThreadCount);

		uint WorkloadSize = aElementCount / aThreadCount;
		uint Remainder = aElementCount % aThreadCount;

		// Generate Workload Sizes for each thread.
		for (size_t i = 0; i < aThreadCount; i++) {
			Workload[i].Size = WorkloadSize;
			if (i < Remainder) {
				Workload[i].Size += 1;
			}
		}

		// Generate Offsets From Thread Workload Sizes.
		Workload[0].Offset = 0;
		for (size_t i = 0; i < (aThreadCount - 1); i++) {
			Workload[i + 1].Offset = Workload[i].Size + Workload[i].Offset;
		}

		return Workload;
	}

}
