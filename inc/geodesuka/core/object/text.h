#pragma once
#ifndef GEODESUKA_CORE_OBJECT_TEXT_H
#define GEODESUKA_CORE_OBJECT_TEXT_H

#include "../util/string.h"

#include "../object_t.h"

namespace geodesuka::core::object {

	class text : public util::string, public object_t {
	public:

		// Is both a child of text.h and object_t Used for
		// direct text rendering to 
	};

}

#endif // !GEODESUKA_CORE_OBJECT_VTEXT_H
