#include <geodesuka/core/object/system_terminal.h>

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <filesystem>

namespace geodesuka::core::object {

	const char* system_terminal::red 		= "\033[0;31m";
	const char* system_terminal::green 		= "\033[0;32m";
	const char* system_terminal::yellow 	= "\033[0;33m";
	const char* system_terminal::blue 		= "\033[0;34m";
	const char* system_terminal::magenta 	= "\033[0;35m";
	const char* system_terminal::cyan 		= "\033[0;36m";
	const char* system_terminal::white 		= "\033[0;37m";
	const char* system_terminal::reset 		= "\033[0m";

	system_terminal::system_terminal(engine* aEngine, const char* aName) : object_t(nullptr, nullptr, aName) {
		this->Engine = aEngine;
		// It doesn't matter what the Context is, it isn't used
		// here for rendering at all.

		std::cout << red		<< "cock" << std::endl;
		std::cout << green		<< "cock" << std::endl;
		std::cout << yellow		<< "cock" << std::endl;
		std::cout << blue		<< "cock" << std::endl;
		std::cout << magenta 	<< "cock" << std::endl;
		std::cout << cyan		<< "cock" << std::endl;
		std::cout << white		<< "cock" << std::endl;
		std::cout << reset		<< "cock" << std::endl;
	}

	system_terminal::~system_terminal() {

	}

	bool system_terminal::operator>>(util::string& aRhs) {
		util::string String;
		this->Mutex.lock();
		while (true) {
			int CodePoint = fgetc(stdin);
			if (CodePoint == '\n') break;
			String += CodePoint;
		}
		this->Mutex.unlock();
		aRhs += String;
		return false;
	}

	bool system_terminal::operator<<(util::string& aRhs) {
		(*this) << aRhs.ptr();
		return false;
	}

	bool system_terminal::operator<<(const char* aRhs) {
		std::cout << aRhs;
		return false;
	}

	void system_terminal::operator<<(util::log& aRhs) {
		this->Mutex.lock();
		for (int i = 0; i < aRhs.count(); i++) {
			std::cout << util::log::message::type_to_string(aRhs[i].Type) << ": " << util::log::message::code_to_string(aRhs[i].Code);
			std::cout << " | " << "Phase: " << aRhs[i].Phase;
			std::cout << " | " << "API: " << util::log::message::api_to_string(aRhs[i].API);
			//std::cout << "|" << "ThreadID: " << aRhs[i].ThreadID;
			std::cout << "| " << "Object Type: " << aRhs[i].ObjectType;
			std::cout << " | " << "Object Name: " << aRhs[i].ObjectName;
			std::cout << " | " << "Content: " << aRhs[i].Content;
			std::cout << std::endl;
		}
		this->Mutex.unlock();	
	}

}