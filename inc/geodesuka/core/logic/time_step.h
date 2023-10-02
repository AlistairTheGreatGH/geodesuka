#pragma once
#ifndef GEODESUKA_CORE_LOGIC_TIME_STEP_H
#define GEODESUKA_CORE_LOGIC_TIME_STEP_H


namespace geodesuka::core::logic {

	class time_step {
	public:

		time_step(double aTimeStep);

		// Set the time step which to fix at.
		void set(double aTimeStep);

		// Start before loop workload.
		void start();

		// Use to calculate remaining time and fix timestep.
		double stop();

		bool cycle(double aTimeStep);

		double delta_time() const;

	private:

		double t1, t2;
		double wt, ht;
		double dt;
		double ts;

	};

}

#endif // !GEODESUKA_CORE_LOGIC_TIME_STEP_H
