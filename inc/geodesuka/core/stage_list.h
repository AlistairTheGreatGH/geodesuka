#pragma once
#ifndef GEODESUKA_CORE_STAGE_LIST_H
#define GEODESUKA_CORE_STAGE_LIST_H

#include "util/list.h"

#include "stage_t.h"

namespace geodesuka::core {

	typedef util::list<stage_t*> stage_list;

	//class stage_list : public util::list<stage_t*> {
	//public:
	//};

}

#endif // !GEODESUKA_CORE_STAGE_LIST_H
