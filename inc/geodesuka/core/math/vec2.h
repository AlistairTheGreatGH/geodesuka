#pragma once
#ifndef GEODESUKA_CORE_MATH_VEC2_H
#define GEODESUKA_CORE_MATH_VEC2_H

#include "config.h"

namespace geodesuka::core::math {

	template <typename T>
	union vec2 {

		// Data
		struct {
			T x, y;
		};
		struct {
			T r, g;
		};
		struct {
			T a, b;
		};
		T ptr[2];

		vec2() {};
		vec2(T aValue) : x(aValue), y(aValue) {}
		vec2(T aX, T aY) : x(aX), y(aY) {}

		T operator[](int aIndex) const;
		T& operator[](int aIndex);

		vec2<T> operator-() const;

		// Vector Operations
		vec2<T> operator+(const vec2<T>& aRhs) const;
		vec2<T> operator-(const vec2<T>& aRhs) const;
		T operator*(const vec2<T>& aRhs) const;
		T operator^(const vec2<T>& aRhs) const;

		vec2<T>& operator+=(const vec2<T>& aRhs);
		vec2<T>& operator-=(const vec2<T>& aRhs);

		// Scalar Operations
		vec2<T>& operator=(T aRhs);
		vec2<T> operator+(T aRhs) const;
		vec2<T> operator-(T aRhs) const;
		vec2<T> operator*(T aRhs) const;
		vec2<T> operator/(T aRhs) const;

		vec2<T>& operator+=(T aRhs);
		vec2<T>& operator-=(T aRhs);
		vec2<T>& operator*=(T aRhs);
		vec2<T>& operator/=(T aRhs);

	};

	template<typename T>
	inline T vec2<T>::operator[](int aIndex) const {
		return this->ptr[aIndex];
	}

	template<typename T>
	inline T& vec2<T>::operator[](int aIndex) {
		return this->ptr[aIndex];
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator-() const {
		return vec2<T>(-this->x, -this->y);
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator+(const vec2<T>& aRhs) const {
		return vec2<T>(this->x + aRhs.x, this->y + aRhs.y);
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator-(const vec2<T>& aRhs) const {
		return vec2<T>(this->x - aRhs.x, this->y - aRhs.y);
	}

	template<typename T>
	inline T vec2<T>::operator*(const vec2<T>& aRhs) const {
		return ((this->x * aRhs.x) + (this->y * aRhs.y));
	}

	template<typename T>
	inline T vec2<T>::operator^(const vec2<T>& aRhs) const {
		return ((this->x * aRhs.y) - (this->y * aRhs.x));
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator+=(const vec2<T>& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator-=(const vec2<T>& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator=(T aRhs) {
		this->x = aRhs; this->y = aRhs;
		return *this;
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator+(T aRhs) const {
		return vec2<T>(this->x + aRhs, this->y + aRhs);
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator-(T aRhs) const {
		return vec2<T>(this->x - aRhs, this->y - aRhs);
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator*(T aRhs) const {
		return vec2<T>(this->x * aRhs, this->y * aRhs);
	}

	template<typename T>
	inline vec2<T> vec2<T>::operator/(T aRhs) const {
		return vec2<T>(this->x / aRhs, this->y / aRhs);
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator+=(T aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator-=(T aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator*=(T aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template<typename T>
	inline vec2<T>& vec2<T>::operator/=(T aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T>
	vec2<T> operator+(T aLhs, const vec2<T>& aRhs) {
		vec2<T>(aLhs + aRhs.x, aLhs + aRhs.y);
	}

	template <typename T>
	vec2<T> operator-(T aLhs, const vec2<T>& aRhs) {
		vec2<T>(aLhs - aRhs.x, aLhs - aRhs.y);
	}

	template <typename T>
	vec2<T> operator*(T aLhs, const vec2<T>& aRhs) {
		vec2<T>(aLhs * aRhs.x, aLhs * aRhs.y);
	}

	template <typename T>
	vec2<T> operator/(T aLhs, const vec2<T>& aRhs) {
		vec2<T>(aLhs / aRhs.x, aLhs / aRhs.y);
	}

	template <typename T>
	T length(const vec2<T>& aArg) {
		return sqrt(aArg * aArg);
	}

	template <typename T>
	vec2<T> normalize(const vec2<T>& aArg) {
		return aArg / length(aArg);
	}

}

#endif // !GEODESUKA_CORE_MATH_VEC2_H
