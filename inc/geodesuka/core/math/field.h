#pragma once
#ifndef GEODESUKA_CORE_MATH_FIELD_H
#define GEODESUKA_CORE_MATH_FIELD_H

// ------------------------------ field.h ------------------------------ //
/*
* field.h is a class which exists over a selected space with
*/


#include <vector>

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace geodesuka::core::math {

	// T1 = 1, 2, 3, 4
	template <typename T1, typename T2, typename T3>
	class field {
	public:

		enum direction {
			X,
			Y,
			Z,
			T
		};

		// Element Count, Bounding Region, and Field Data
		T1 ElementCount;
		T2 LowerBound, UpperBound;
		std::vector<T3> Data;

	private:

	};

}

#endif // !GEODESUKA_CORE_MATH_FIELD_H
