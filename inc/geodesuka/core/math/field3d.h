#pragma once
#ifndef GEODESUKA_CORE_MATH_FIELD3D_H
#define GEODESUKA_CORE_MATH_FIELD3D_H

// ------------------------------ field.h ------------------------------ //
/*
* field.h is a class which exists over a selected space with
*/

#include "field.h"

namespace geodesuka::core::math {

	template <typename T>
	class field3d : public field<vec3<size_t>, vec3<float>, T> {
	public:

		//field3d();
		//field3d(vec3<float> aLowerBound, vec3<float> aUpperBound, vec3<size_t> aElementCount, field::direction aDirection);
		//field3d(vec3<float> aLowerBound, vec3<float> aUpperBound, vec3<size_t> aElementCount, T aFieldValue);

		T operator[](size_t aIndex) const;
		T& operator[](size_t aIndex);

		T operator[](vec3<size_t> aIndex) const;
		T& operator[](vec3<size_t> aIndex);

		T operator[](vec3<float> aR) const;
		T& operator[](vec3<float> aR);

		field3d<T> operator&(const field3d<T>& aRhs) const;
		field3d<T> operator|(const field3d<T>& aRhs) const;

	private:

	};


}

#endif // !GEODESUKA_CORE_MATH_FIELD3D_H
