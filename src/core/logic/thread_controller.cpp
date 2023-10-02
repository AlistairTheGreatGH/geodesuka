#include <geodesuka/core/logic/thread_controller.h>

#include <geodesuka/core/logic/timer.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <thread>
#include <mutex>

#define GEODESUKA_CORE_LOGIC_THREAD_SLEEP_TIME 0.001

namespace geodesuka::core::logic {


	thread_controller::thread_controller() {}

	thread_controller::~thread_controller() {}

	bool thread_controller::cycle(double aTimeStep) {
		bool IsTerminated = this->is_terminated();

		// If thread is not registered, register it.
		if (!this->exists())
			this->add_thread();

		// Lock thread if requested.
		this->suspension_loop();

		// Enforce Time Step on Loop.
		this->enforce_time_step(aTimeStep);

		// If thread exists and is terminated, 
		if ((this->exists()) && (IsTerminated))
			this->remove_thread();

		return !IsTerminated;
	}

	void thread_controller::suspend() {
		this->suspend(std::this_thread::get_id());
	}

	void thread_controller::suspend(std::thread::id aID) {
		std::thread::id ID[1] = { aID };
		this->suspend(ID, 1);
	}

	void thread_controller::suspend(std::thread::id* aID, size_t aCount) {
		bool AllThreadsTrapped = false;
		while (!AllThreadsTrapped) {
			AllThreadsTrapped = true;
			this->Mutex.lock();

			// Flag all threads to suspend.
			for (size_t i = 0; i < aCount; i++) {
				// Set thread suspension to true.
				for (size_t j = 0; j < this->ThreadData.size(); j++) {
					if (aID[i] == this->ThreadData[j].ID) {
						// Set suspension to true.
						this->ThreadData[j].Suspend = true;
					}
				}
			}

			// Check if all threads are trapped.
			for (size_t i = 0; i < aCount; i++) {
				// Skip, do not wait for current thread to be suspended.
				if (std::this_thread::get_id() == aID[i]) continue;
				// Check if non-current threads have been trapped.
				for (size_t j = 0; j < this->ThreadData.size(); j++) {
					if (aID[i] == this->ThreadData[j].ID) {
						AllThreadsTrapped &= this->ThreadData[j].Trapped;
					}
				}
			}

			this->Mutex.unlock();
			// Hand off back to other threads and wait for update.
			wait_for(0.001);
		}
	}

	void thread_controller::suspend_all() {
		std::vector<std::thread::id> SuspendList;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) continue;
			SuspendList.push_back(this->ThreadData[i].ID);
		}
		this->Mutex.unlock();
		this->suspend(SuspendList.data(), SuspendList.size());
	}

	void thread_controller::resume(std::thread::id aID) {
		std::thread::id ID[1] = { aID };
		this->resume(ID, 1);
	}

	void thread_controller::resume(std::thread::id* aID, size_t aCount) {
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			for (size_t j = 0; j < aCount; j++) {
				if (this->ThreadData[i].ID == aID[j]) {
					this->ThreadData[i].Suspend = false;
				}
			}
		}
		this->Mutex.unlock();
	}

	void thread_controller::resume_all() {
		std::vector<std::thread::id> ResumeList;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			//if (this->ThreadData[i].ID == std::this_thread::get_id()) continue;
			ResumeList.push_back(this->ThreadData[i].ID);
		}
		this->Mutex.unlock();
		this->resume(ResumeList.data(), ResumeList.size());
	}

	void thread_controller::terminate() {
		this->terminate(std::this_thread::get_id());
	}

	void thread_controller::terminate(std::thread::id aID) {
		std::thread::id ID[1] = {aID};
		this->terminate(ID, 1);
	}

	void thread_controller::terminate(std::thread::id* aID, size_t aCount) {
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			for (size_t j = 0; j < aCount; j++) {
				if (this->ThreadData[i].ID == aID[j]) {
					this->ThreadData[i].Terminate = true;
				}
			}
		}
		this->Mutex.unlock();
	}

	void thread_controller::terminate_all() {
		std::vector<std::thread::id> TerminateList;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			TerminateList.push_back(this->ThreadData[i].ID);
		}
		this->Mutex.unlock();
		this->terminate(TerminateList.data(), TerminateList.size());
	}

	bool thread_controller::exists() {
		bool TF = false;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				TF = true;
				break;
			}
		}
		this->Mutex.unlock();
		return TF;
	}

	thread_controller::data::data() {
		this->Suspend = false;
		this->Trapped = false;
		this->Terminate = false;
		//this->SleepTime = 0.001;
		this->t1 = 0.0; this->t2 = 0.0;
		this->wt = 0.0; this->ht = 0.0;
		this->dt = 0.0;
		this->ts = 0.0;
	}

	thread_controller::data::data(std::thread::id aID) {
		this->ID = aID;
		this->Suspend = false;
		this->Trapped = false;
		this->Terminate = false;
		//this->SleepTime = 0.001;
		this->t1 = 0.0; this->t2 = 0.0;
		this->wt = 0.0; this->ht = 0.0;
		this->dt = 0.0;
		this->ts = 0.0;
	}

	void thread_controller::add_thread() {
		this->Mutex.lock();
		this->ThreadData.push_back(data(std::this_thread::get_id()));
		this->Mutex.unlock();
	}

	void thread_controller::remove_thread() {
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				this->ThreadData.erase(i + this->ThreadData.begin());
			}
		}
		this->Mutex.unlock();
	}

	bool thread_controller::is_suspended() {
		bool ReturnValue = false;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				ReturnValue = this->ThreadData[i].Suspend;
			}
		}
		this->Mutex.unlock();
		return ReturnValue;
	}

	void thread_controller::set_trapped_state(bool aTF) {
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				this->ThreadData[i].Trapped = aTF;
			}
		}
		this->Mutex.unlock();
	}

	bool thread_controller::is_terminated() {
		bool ReturnValue = false;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				ReturnValue = this->ThreadData[i].Terminate;
			}
		}
		this->Mutex.unlock();
		return ReturnValue;
	}

	void thread_controller::suspension_loop() {
		if (this->is_suspended()) {
			this->set_trapped_state(true);
			while (this->is_suspended()) {
				wait_for(GEODESUKA_CORE_LOGIC_THREAD_SLEEP_TIME);
			}
			this->set_trapped_state(false);
		}
	}

	void thread_controller::enforce_time_step(double aTimeStep) {
		// Get per thread data.
		data td;
		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				td = this->ThreadData[i];
				break;
			}
		}
		this->Mutex.unlock();


		// Set new timestep.
		td.ts = aTimeStep;

		// End of Time Cycle
		td.t2 = get_time();

		// Wait if finished early.
		td.wt = td.t2 - td.t1;
		if (td.wt < td.ts) {
			td.ht = td.ts - td.wt;
			wait_for(td.ht);
		}
		else {
			td.ht = 0.0;
		}
		td.dt = td.wt + td.ht;

		// Start of new cycle
		td.t1 = get_time();

		this->Mutex.lock();
		for (size_t i = 0; i < this->ThreadData.size(); i++) {
			if (this->ThreadData[i].ID == std::this_thread::get_id()) {
				this->ThreadData[i].t1 = td.t1;
				this->ThreadData[i].t2 = td.t2;
				this->ThreadData[i].wt = td.wt;
				this->ThreadData[i].ht = td.ht;
				this->ThreadData[i].dt = td.dt;
				this->ThreadData[i].ts = td.ts;
				break;
			}
		}
		this->Mutex.unlock();
	}

}
