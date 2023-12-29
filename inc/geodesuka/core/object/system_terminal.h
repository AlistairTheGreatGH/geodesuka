#pragma once
#ifndef GEODESUKA_CORE_OBJECT_SYSTEM_TERMINAL_H
#define GEODESUKA_CORE_OBJECT_SYSTEM_TERMINAL_H

#include "../util/string.h"
#include "../util/log.h"

#include "../gcl/context.h"

#include "../object_t.h"

/*
* This class may be really dumb and retarded,
* but I am importing some of my code from my
* mbed rc car code to handle terminal input.
* 
*/

namespace geodesuka::core::object {

	class system_terminal : public object_t {
	public:

		friend class geodesuka::engine;

		static const char* red;
		static const char* green;
		static const char* yellow;
		static const char* blue;
		static const char* magenta;
		static const char* cyan;
		static const char* white;
		static const char* reset;

		~system_terminal();

		// Returns true if input is gathered. Returns false if empty line.
		bool operator>>(util::string &aRhs);

		// Prints output to terminal.
		bool operator<<(util::string& aRhs);

		// Same as above, but good for string literals.
		bool operator<<(const char* aRhs);

		// Print Log
		void operator<<(util::log& aRhs);

	private:

		std::string UserName;
		std::string SystemName;
		std::string CurrentWorkingDirectory;

		// Only engine can create this. This is an object representation of the system
		// terminal at boot up. This will be used for debugging if system_terminal 
		// is enabled.
		system_terminal(engine* aEngine, const char* aName);

	};

}

#endif // !GEODESUKA_CORE_OBJECT_SYSTEM_TERMINAL_H
