#pragma once
#ifndef GEODESUKA_CORE_LOGIC_THREAD_LIST_H
#define GEODESUKA_CORE_LOGIC_THREAD_LIST_H

#include <thread>

namespace geodesuka::core::logic {

	class thread_list {
	public:



	private:

		size_t Count;
		std::thread::id* Handle;

	};

}

#endif // !GEODESUKA_CORE_LOGIC_THREAD_LIST_H
