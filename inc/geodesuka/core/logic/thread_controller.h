#pragma once
#ifndef GEODESUKA_CORE_LOGIC_THREAD_CONTROLLER_H
#define GEODESUKA_CORE_LOGIC_THREAD_CONTROLLER_H

// thread_controller exists as a one size fits
// all thread contoller, for each thread to 
// impart control on neighboring threads by ID.
//

#include <vector>
#include <atomic>
#include <mutex>
#include <thread>
#include "thread_list.h"

namespace geodesuka::core::logic {

	class thread_controller {
	public:

		thread_controller();
		~thread_controller();

		// How to use.
		// while(ThreadController.cycle(0.01)) {
		//		// Work done here.
		// }
		bool cycle(double aTimeStep);

		void suspend();
		void suspend(std::thread::id aID);
		void suspend(std::thread::id* aID, size_t aCount);
		void suspend_all();

		void resume(std::thread::id aID);
		void resume(std::thread::id* aID, size_t aCount);
		void resume_all();

		void terminate();
		void terminate(std::thread::id aID);
		void terminate(std::thread::id* aID, size_t aCount);
		void terminate_all();

		bool exists();

	private:

		struct data {
			// ID of the thread in question.
			std::thread::id ID;

			// Is it suspended, trapped, or need to be terminated?
			bool Suspend;
			bool Trapped;
			bool Terminate;
			//double SleepTime;

			// Time Control for the thread in question.
			double t1, t2;
			double wt, ht;
			double dt;
			double ts;

			data();
			data(std::thread::id aID);
		};

		std::mutex Mutex;
		std::vector<data> ThreadData;

		void add_thread();
		void remove_thread();

		bool is_suspended();
		void set_trapped_state(bool aTF);
		bool is_terminated();

		void suspension_loop();
		void enforce_time_step(double aTimeStep);

	};

}

#endif // !GEODESUKA_CORE_LOGIC_THREAD_CONTROLLER_H
