#pragma once
#ifndef GEODESUKA_CORE_MATH_VEC3_H
#define GEODESUKA_CORE_MATH_VEC3_H

#include "config.h"

namespace geodesuka::core::math {

	template <typename T>
	union vec3 {

		// Data
		struct {
			T x, y, z;
		};
		struct {
			T r, g, b;
		};
		T ptr[3];

		vec3() {};
		vec3(T aValue) : x(aValue), y(aValue), z(aValue) {}
		vec3(T aX, T aY, T aZ) : x(aX), y(aY), z(aZ) {}

		T operator[](int aIndex) const;
		T& operator[](int aIndex);

		vec3<T> operator-() const;

		// Vector Operations
		vec3<T> operator+(const vec3<T>& aRhs) const;
		vec3<T> operator-(const vec3<T>& aRhs) const;
		T operator*(const vec3<T>& aRhs) const;
		T operator^(const vec3<T>& aRhs) const;

		vec3<T>& operator+=(const vec3<T>& aRhs);
		vec3<T>& operator-=(const vec3<T>& aRhs);

		// Scalar Operations
		vec3<T>& operator=(T aRhs);
		vec3<T> operator+(T aRhs) const;
		vec3<T> operator-(T aRhs) const;
		vec3<T> operator*(T aRhs) const;
		vec3<T> operator/(T aRhs) const;

		vec3<T>& operator+=(T aRhs);
		vec3<T>& operator-=(T aRhs);
		vec3<T>& operator*=(T aRhs);
		vec3<T>& operator/=(T aRhs);

	};

	template<typename T>
	inline T vec3<T>::operator[](int aIndex) const {
		return this->ptr[aIndex];
	}

	template<typename T>
	inline T& vec3<T>::operator[](int aIndex) {
		return this->ptr[aIndex];
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator-() const {
		return vec3<T>(-this->x, -this->y, -this->z);
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator+(const vec3<T>& aRhs) const {
		return vec3<T>(this->x + aRhs.x, this->y + aRhs.y, this->z + aRhs.z);
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator-(const vec3<T>& aRhs) const {
		return vec3<T>(this->x - aRhs.x, this->y - aRhs.y, this->z - aRhs.z);
	}

	template<typename T>
	inline T vec3<T>::operator*(const vec3<T>& aRhs) const {
		return ((this->x * aRhs.x) + (this->y * aRhs.y) + (this->z * aRhs.z));
	}

	template<typename T>
	inline T vec3<T>::operator^(const vec3<T>& aRhs) const {
		return vec3<T>(
			this->y * aRhs.z - this->z * aRhs.y,
			this->z * aRhs.x - this->x * aRhs.z,
			this->x * aRhs.y - this->y * aRhs.x
		);
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator+=(const vec3<T>& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator-=(const vec3<T>& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator=(T aRhs) {
		this->x = aRhs; this->y = aRhs; this->z = aRhs;
		return *this;
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator+(T aRhs) const {
		return vec3<T>(this->x + aRhs, this->y + aRhs, this->z + aRhs);
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator-(T aRhs) const {
		return vec3<T>(this->x - aRhs, this->y - aRhs, this->z - aRhs);
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator*(T aRhs) const {
		return vec3<T>(this->x * aRhs, this->y * aRhs, this->z * aRhs);
	}

	template<typename T>
	inline vec3<T> vec3<T>::operator/(T aRhs) const {
		return vec3<T>(this->x / aRhs, this->y / aRhs, this->z / aRhs);
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator+=(T aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator-=(T aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator*=(T aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template<typename T>
	inline vec3<T>& vec3<T>::operator/=(T aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T>
	vec3<T> operator+(T aLhs, const vec3<T>& aRhs) {
		return vec3<T>(aLhs + aRhs.x, aLhs + aRhs.y, aLhs + aRhs.z);
	}

	template <typename T>
	vec3<T> operator-(T aLhs, const vec3<T>& aRhs) {
		return vec3<T>(aLhs - aRhs.x, aLhs - aRhs.y, aLhs - aRhs.z);
	}

	template <typename T>
	vec3<T> operator*(T aLhs, const vec3<T>& aRhs) {
		return vec3<T>(aLhs * aRhs.x, aLhs * aRhs.y, aLhs * aRhs.z);
	}

	template <typename T>
	vec3<T> operator/(T aLhs, const vec3<T>& aRhs) {
		return vec3<T>(aLhs / aRhs.x, aLhs / aRhs.y, aLhs / aRhs.z);
	}

	template <typename T>
	T length(const vec3<T>& aArg) {
		return sqrt(aArg * aArg);
	}

	template <typename T>
	vec3<T> normalize(const vec3<T>& aArg) {
		return aArg / length(aArg);
	}

}

#endif // !GEODESUKA_CORE_MATH_VEC3_H
