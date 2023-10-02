#pragma once
#ifndef GEODESUKA_CORE_MATH_QUATERNION_H
#define GEODESUKA_CORE_MATH_QUATERNION_H

#include "config.h"
#include "type.h"
#include "complex.h"

//tex:
// A quaternion<T> can be writtin in the mathematical form.
// $$ q = a + b\hat{i} + c \hat{j} + d \hat{k} = a + \vec{v} $$
// A quaternion<T> exponential can be written as
// $$ e^{q} = e^{a} ( \cos{v} + \hat{v} \cos{v} )$$
// A unit quaternion<T> in the service of rotating a vector $\vec{r}$
// only needs to be defined in terms of another vector which it rotates
// around, and a quantity of of how much it rotates around.

//tex:
// The magnitude of the quaternion<T> specifies the amount of rotation,
// while the unit vector specifies which vector is it to be rotated around.
// $$ \vec{r}^{'} = e^{\frac{\theta}{2} \hat{v}} \; \vec{r} \; e^{-\frac{\theta}{2}\hat{v}}$$

namespace geodesuka::core::math {

	template <typename T>
	union quaternion {
		// Unit Versors
		// static const quaternion i;
		// static const quaternion j;
		// static const quaternion k;

		struct {
			T b, c, d, a;
		};
		struct {
			T x, y, z, t;
		};
		T ptr[4];

		quaternion();
		~quaternion();

		quaternion(T Val);
		quaternion(T VReal, T VImag);
		quaternion(const complex<T>& Val);
		quaternion(T ValA, T ValB, T ValC, T ValD);
		quaternion(const quaternion& Val);

		quaternion<T>& operator=(T aRhs);
		quaternion<T>& operator=(const complex<T>& aRhs);
		quaternion<T>& operator=(const quaternion<T>& aRhs);

		T& operator[](int Index);

		quaternion<T> operator-() const;
		quaternion<T> operator~() const;

		quaternion<T> operator+(const quaternion<T>& aRhs) const;
		quaternion<T> operator-(const quaternion<T>& aRhs) const;
		quaternion<T> operator*(const quaternion<T>& aRhs) const;
		quaternion<T> operator/(const quaternion<T>& aRhs) const;

		quaternion<T>& operator+=(const quaternion<T>& aRhs);
		quaternion<T>& operator-=(const quaternion<T>& aRhs);
		quaternion<T>& operator*=(const quaternion<T>& aRhs);
		quaternion<T>& operator/=(const quaternion<T>& aRhs);

		quaternion<T> operator+(const complex<T>& aRhs) const;
		quaternion<T> operator-(const complex<T>& aRhs) const;
		quaternion<T> operator*(const complex<T>& aRhs) const;
		quaternion<T> operator/(const complex<T>& aRhs) const;

		quaternion<T>& operator+=(const complex<T>& aRhs);
		quaternion<T>& operator-=(const complex<T>& aRhs);
		quaternion<T>& operator*=(const complex<T>& aRhs);
		quaternion<T>& operator/=(const complex<T>& aRhs);

		quaternion<T> operator+(T aRhs) const;
		quaternion<T> operator-(T aRhs) const;
		quaternion<T> operator*(T aRhs) const;
		quaternion<T> operator/(T aRhs) const;

		quaternion<T>& operator+=(T aRhs);
		quaternion<T>& operator-=(T aRhs);
		quaternion<T>& operator*=(T aRhs);
		quaternion<T>& operator/=(T aRhs);

	};

	// const quaternion<T> quaternion<T>::i = quaternion<T>(0, 1, 0, 0);
	// const quaternion<T> quaternion<T>::j = quaternion<T>(0, 0, 1, 0);
	// const quaternion<T> quaternion<T>::k = quaternion<T>(0, 0, 0, 1);

	template <typename T> 
	inline quaternion<T>::quaternion() {
		this->a = 0.0; this->b = 0.0; this->c = 0.0; this->d = 0.0;
	}

	template <typename T> 
	inline quaternion<T>::~quaternion() {
		this->a = 0.0; this->b = 0.0; this->c = 0.0; this->d = 0.0;
	}

	template <typename T> 
	inline quaternion<T>::quaternion(T Val) {
		this->a = Val; this->b = 0.0; this->c = 0.0; this->d = 0.0;
	}

	template <typename T> 
	inline quaternion<T>::quaternion(T VReal, T VImag) {
		this->a = VReal; this->b = VImag; this->c = 0.0; this->d = 0.0;
	}

	template <typename T> 
	inline quaternion<T>::quaternion(const complex<T>& Val) {
		this->a = Val.a; this->b = Val.b; this->c = 0.0; this->d = 0.0;
	}

	template <typename T> 
	inline quaternion<T>::quaternion(T ValA, T ValB, T ValC, T ValD) {
		this->a = ValA; this->b = ValB; this->c = ValC; this->d = ValD;
	}

	template <typename T> 
	inline quaternion<T>::quaternion(const quaternion<T>& Val) {
		this->a = Val.a; this->b = Val.b; this->c = Val.c; this->d = Val.d;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator=(T aRhs) {
		this->a = aRhs; this->b = 0.0; this->c = 0.0; this->d = 0.0;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator=(const complex<T>& aRhs) {
		this->a = aRhs.a; this->b = aRhs.b; this->c = 0.0; this->d = 0.0;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator=(const quaternion<T>& aRhs) {
		this->a = aRhs.a; this->b = aRhs.b; this->c = aRhs.c; this->d = aRhs.d;
		return *this;
	}

	template <typename T> 
	inline T& quaternion<T>::operator[](int Index) {
		return this->ptr[Index];
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator-() const {
		return quaternion<T>(-this->a, -this->b, -this->c, -this->d);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator~() const {
		return quaternion<T>(this->a, -this->b, -this->c, -this->d);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator+(const quaternion<T>& aRhs) const {
		return quaternion<T>(this->a + aRhs.a, this->b + aRhs.b, this->c + aRhs.c, this->d + aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator-(const quaternion<T>& aRhs) const {
		return quaternion<T>(this->a - aRhs.a, this->b - aRhs.b, this->c - aRhs.c, this->d - aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator*(const quaternion<T>& aRhs) const {
		return quaternion<T>(
			this->a * aRhs.a - this->b * aRhs.b - this->c * aRhs.c - this->d * aRhs.d,
			this->a * aRhs.b + this->b * aRhs.a + this->c * aRhs.d - this->d * aRhs.c,
			this->a * aRhs.c - this->b * aRhs.d + this->c * aRhs.a + this->d * aRhs.b,
			this->a * aRhs.d + this->b * aRhs.c - this->c * aRhs.b + this->d * aRhs.a
		);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator/(const quaternion<T>& aRhs) const {
		return ((*this) * (~aRhs)) / abs2(aRhs);
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator+=(const quaternion<T>& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator-=(const quaternion<T>& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator*=(const quaternion<T>& aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator/=(const quaternion<T>& aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator+(const complex<T>& aRhs) const {
		return (*this + quaternion<T>(aRhs));
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator-(const complex<T>& aRhs) const {
		return (*this - quaternion<T>(aRhs));
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator*(const complex<T>& aRhs) const {
		return (*this * quaternion<T>(aRhs));
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator/(const complex<T>& aRhs) const {
		return (*this / quaternion<T>(aRhs));
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator+=(const complex<T>& aRhs) {
		*this += quaternion<T>(aRhs);
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator-=(const complex<T>& aRhs) {
		*this -= quaternion<T>(aRhs);
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator*=(const complex<T>& aRhs) {
		*this *= quaternion<T>(aRhs);
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator/=(const complex<T>& aRhs) {
		*this /= quaternion<T>(aRhs);
		return *this;
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator+(T aRhs) const {
		return quaternion<T>(this->a + aRhs, this->b, this->c, this->d);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator-(T aRhs) const {
		return quaternion<T>(this->a - aRhs, this->b, this->c, this->d);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator*(T aRhs) const {
		return quaternion<T>(this->a * aRhs, this->b * aRhs, this->c * aRhs, this->d * aRhs);
	}

	template <typename T> 
	inline quaternion<T> quaternion<T>::operator/(T aRhs) const {
		return quaternion<T>(this->a / aRhs, this->b / aRhs, this->c / aRhs, this->d / aRhs);
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator+=(T aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator-=(T aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator*=(T aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T>& quaternion<T>::operator/=(T aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T> 
	inline quaternion<T> operator+(const complex<T>& aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs.a + aRhs.a, aLhs.b + aRhs.b, aRhs.c, aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> operator-(const complex<T>& aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs.a - aRhs.a, aLhs.b - aRhs.b, -aRhs.c, -aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> operator*(const complex<T>& aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs) * aRhs;
	}

	template <typename T> 
	inline quaternion<T> operator/(const complex<T>& aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs) / aRhs;
	}

	template <typename T> 
	inline quaternion<T> operator+(T aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs + aRhs.a, aRhs.b, aRhs.c, aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> operator-(T aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs - aRhs.a, -aRhs.b, -aRhs.c, -aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> operator*(T aLhs, const quaternion<T>& aRhs) {
		return quaternion<T>(aLhs * aRhs.a, aLhs * aRhs.b, aLhs * aRhs.c, aLhs * aRhs.d);
	}

	template <typename T> 
	inline quaternion<T> operator/(T aLhs, const quaternion<T>& aRhs) {
		return (aLhs / abs2(aRhs)) * (~aRhs);
	}

	template <typename T> 
	inline T abs2(const quaternion<T>& Arg) {
		return (Arg.a * Arg.a + Arg.b * Arg.b + Arg.c * Arg.c + Arg.d * Arg.d);
	}

	template <typename T> 
	inline T abs(const quaternion<T>& Arg) {
		return std::sqrt(abs2(Arg));
	}

	template <typename T> 
	inline quaternion<T> exp(const quaternion<T>& Arg) {
		quaternion<T> u = quaternion<T>(0.0, Arg.b, Arg.c, Arg.d);
		T uMag = abs(u);
		return (std::exp(Arg.a) * (std::cos(uMag) + u * (std::sin(uMag) / uMag)));
	}

	template <typename T> 
	inline quaternion<T> ln(const quaternion<T>& Arg) {
		quaternion<T> u = quaternion<T>(0.0, Arg.b, Arg.c, Arg.d);
		T Q = abs(Arg);
		T U = abs(u);
		return (ln(Q) + u * (std::acos(Arg.a / Q) / U));
	}

	template <typename T> 
	inline quaternion<T> pow(const quaternion<T>& Base, const quaternion<T>& Exponent) {
		return exp(ln(Base) * Exponent);
	}

#ifdef MATH_ENABLE_IO
	template <typename T> 
	inline std::ostream& operator<<(std::ostream& os, const quaternion<T>& q) {
		os << q.a << " + " << q.b << "i + " << q.c << "j + " << q.d << "k";
		return os;
	}
#endif // MATH_ENABLE_IO

}

#endif // !GEODESUKA_CORE_MATH_QUATERNION_H
