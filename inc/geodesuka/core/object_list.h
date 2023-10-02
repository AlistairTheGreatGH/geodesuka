#pragma once
#ifndef GEODESUKA_CORE_OBJECT_LIST_H
#define GEODESUKA_CORE_OBJECT_LIST_H

#include <vector>

#include "util/list.h"

#include "object_t.h"

namespace geodesuka::core {

	class object_list : public util::list<object_t*> {
	public:

		object_list();
		object_list(int aObjectCount);
		object_list(object_t* aObject);
		object_list(int aObjectCount, object_t** aObjectList);

		// Creates a list of objects owned by that stage.
		object_list gather_by(stage_t* aStage) const;

		// Finds an object by a name in the object set.
		object_t* find_by_name(const char* aName) const;

	};

}

#endif // !GEODESUKA_CORE_OBJECT_LIST_H
