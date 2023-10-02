#pragma once
#ifndef GEODESUKA_CORE_MATH_VEC4_H
#define GEODESUKA_CORE_MATH_VEC4_H

#include "config.h"

namespace geodesuka::core::math {

	template <typename T>
	union vec4 {

		// Data
		struct {
			T x, y, z, t;
		};
		struct {
			T r, g, b, a;
		};
		T ptr[4];

		vec4() {};
		vec4(T aValue) : x(aValue), y(aValue) {}
		vec4(T aX, T aY, T aZ, T aT) : x(aX), y(aY), z(aZ), t(aT) {}

		T operator[](int aIndex) const;
		T& operator[](int aIndex);

		vec4<T> operator-() const;

		// Vector Operations
		vec4<T> operator+(const vec4<T>& aRhs) const;
		vec4<T> operator-(const vec4<T>& aRhs) const;
		T operator*(const vec4<T>& aRhs) const;

		vec4<T>& operator+=(const vec4<T>& aRhs);
		vec4<T>& operator-=(const vec4<T>& aRhs);

		// Scalar Operations
		vec4<T>& operator=(T aRhs);
		vec4<T> operator+(T aRhs) const;
		vec4<T> operator-(T aRhs) const;
		vec4<T> operator*(T aRhs) const;
		vec4<T> operator/(T aRhs) const;

		vec4<T>& operator+=(T aRhs);
		vec4<T>& operator-=(T aRhs);
		vec4<T>& operator*=(T aRhs);
		vec4<T>& operator/=(T aRhs);

	};

	template<typename T>
	inline T vec4<T>::operator[](int aIndex) const {
		return this->ptr[aIndex];
	}

	template<typename T>
	inline T& vec4<T>::operator[](int aIndex) {
		return this->ptr[aIndex];
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator-() const {
		return vec4<T>(-this->x, -this->y, -this->z, -this->t);
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator+(const vec4<T>& aRhs) const {
		return vec4<T>(this->x + aRhs.x, this->y + aRhs.y, this->z + aRhs.z, this->t + aRhs.t);
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator-(const vec4<T>& aRhs) const {
		return vec4<T>(this->x - aRhs.x, this->y - aRhs.y, this->z - aRhs.z, this->t - aRhs.t);
	}

	template<typename T>
	inline T vec4<T>::operator*(const vec4<T>& aRhs) const {
		return ((this->x * aRhs.x) + (this->y * aRhs.y) + (this->z * aRhs.z) + (this->t * aRhs.t));
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator+=(const vec4<T>& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator-=(const vec4<T>& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator=(T aRhs) {
		this->x = aRhs; this->y = aRhs; this->z = aRhs; this->t = aRhs;
		return *this;
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator+(T aRhs) const {
		return vec4<T>(this->x + aRhs, this->y + aRhs, this->z + aRhs, this->t + aRhs);
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator-(T aRhs) const {
		return vec4<T>(this->x - aRhs, this->y - aRhs, this->z - aRhs, this->t - aRhs);
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator*(T aRhs) const {
		return vec4<T>(this->x * aRhs, this->y * aRhs, this->z * aRhs, this->t * aRhs);
	}

	template<typename T>
	inline vec4<T> vec4<T>::operator/(T aRhs) const {
		return vec4<T>(this->x / aRhs, this->y / aRhs, this->z / aRhs, this->t / aRhs);
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator+=(T aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator-=(T aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator*=(T aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template<typename T>
	inline vec4<T>& vec4<T>::operator/=(T aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T>
	vec4<T> operator+(T aLhs, const vec4<T>& aRhs) {
		vec4<T>(aLhs + aRhs.x, aLhs + aRhs.y, aLhs + aRhs.z, aLhs + aRhs.t);
	}

	template <typename T>
	vec4<T> operator-(T aLhs, const vec4<T>& aRhs) {
		vec4<T>(aLhs - aRhs.x, aLhs - aRhs.y, aLhs - aRhs.z, aLhs - aRhs.t);
	}

	template <typename T>
	vec4<T> operator*(T aLhs, const vec4<T>& aRhs) {
		vec4<T>(aLhs * aRhs.x, aLhs * aRhs.y, aLhs * aRhs.z, aLhs * aRhs.t);
	}

	template <typename T>
	vec4<T> operator/(T aLhs, const vec4<T>& aRhs) {
		vec4<T>(aLhs / aRhs.x, aLhs / aRhs.y, aLhs / aRhs.z, aLhs / aRhs.t);
	}

	template <typename T>
	T length(const vec4<T>& aArg) {
		return sqrt(aArg * aArg);
	}

	template <typename T>
	vec4<T> normalize(const vec4<T>& aArg) {
		return aArg / length(aArg);
	}

}

#endif // !GEODESUKA_CORE_MATH_VEC4_H
