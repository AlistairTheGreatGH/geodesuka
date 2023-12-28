#include <geodesuka/core/object/system_terminal.h>

#include <stdlib.h>
#include <string.h>

#include <iostream>

namespace geodesuka::core::object {

	system_terminal::system_terminal(engine* aEngine, const char* aName) : object_t(nullptr, nullptr, aName) {
		this->Engine = aEngine;
		// It doesn't matter what the Context is, it isn't used
		// here for rendering at all.
	}

	system_terminal::~system_terminal() {

	}

	bool system_terminal::operator>>(util::string& aRhs) {
		util::string String;
		this->Mutex.lock();
		while (true) {
			int CodePoint = fgetc(stdin);
			if (CodePoint == 0x0A) break;
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
		this->Mutex.lock();
		std::cout << aRhs;
		this->Mutex.unlock();
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