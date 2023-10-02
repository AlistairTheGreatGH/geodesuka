#include <geodesuka/core/logic/time_step.h>

#include <geodesuka/core/logic/timer.h>

namespace geodesuka::core::logic {

	time_step::time_step(double aTimeStep) {
		this->t1 = 0.0; this->t2 = 0.0;
		this->wt = 0.0; this->ht = 0.0;
		this->dt = 0.0;
		this->ts = aTimeStep;
	}

	void time_step::set(double aTimeStep) {
		this->ts = aTimeStep;
	}

	void time_step::start() {
		// Get First Time
		this->t1 = get_time();
	}

	double time_step::stop() {
		// Get second time.
		this->t2 = get_time();
		// Stalls thread if work is completed earlier than timestep.
		wt = t2 - t1;
		if (wt < ts) {
			ht = ts - wt;
			wait_for(ht);
		}
		else {
			ht = 0.0;
		}
		dt = wt + ht;
		return dt;
	}

	bool time_step::cycle(double aTimeStep) {
		// Get second time.
		this->ts = aTimeStep;
		this->t2 = get_time();
		// Stalls thread if work is completed earlier than timestep.
		wt = t2 - t1;
		if (wt < ts) {
			ht = ts - wt;
			wait_for(ht);
		}
		else {
			ht = 0.0;
		}
		dt = wt + ht;
		this->t1 = get_time();
		return true;
	}

	double time_step::delta_time() const {
		return this->ts;
	}

}