#pragma once
#ifndef GEODESUKA_CORE_MATH_QUATERNION_H
#define GEODESUKA_CORE_MATH_QUATERNION_H

#include "config.h"
#include "type.h"
#include "complex.h"


//tex:
// A quaternion<T> can be writtin in the mathematical form.
// $$ q = a + b\hat{i} + c \hat{j} + d \hat{k} = a + \vec{v} $$



//tex:
// ----- Exponentiation of Quaternions -----
// $$ e^{q} = e^{a} ( \cos{v} + \hat{v} \sin{v} ) $$
// Where:
// $$ v = \text{Magnitude of} \: \; \vec{v} \quad $$
// and,
// $$ \hat{v} = \frac{\vec{v}}{v} $$
// ----- Exponentiation of Quaternions -----
//



//tex:
// ----- Rotation of Quaternions -----
// $$ r^{\prime} = q r q^{-1} $$
// Where $ r $ is defined to be some arbitrary vector to be
// rotated in 3D space. The quaternion $ q $ contains the
// information of what vector $ r $ is to be rotated around.
// The quaternion $ q $ containing the direction vector info
// can be rewritten in its exponential form:
// $$ q = e^{a + \vec{v}} $$
// Its inverse is just simply
// $$ q^{-1} = e^{- a - \vec{v}} $$
// When substituting that in the rotation equation,
// $$ r^{\prime} = (e^{a + \vec{v}}) \cdot r \cdot (e^{- a - \vec{v}}) $$
// since $ e^{a} $ is simply just a scalar value, this commute with our
// quaternions and thus,
// $$ r^{\prime} = (e^{\vec{v}}) \cdot r \cdot (e^{-\vec{v}}) $$
// But since our element $ e^{\vec{v}} $ is simply just,
// $$ e^{\vec{v}} = ( \cos{v} + \hat{v} \sin{v} ) $$
// This tells us that the magnitude of $ \vec{v} $ ($v$), is the degree which
// the vector $ r $ is rotated while the unit vector $ \hat{v} = \frac{\vec{v}}{v} $
// is just simply the base unit vector in which $ r $ is rotated around.
// However, when we are saying that $ v $ designates by "how much" we want
// rotate the vector $ r $ around $ \vec{v} $, we want to know how many radians.
// We know that when the magnitude is zero, there is no rotation at all. For
// reasons I do not currently know, the magnitude corresponds to a rotation
// by the following ammount.
// $$ v = \frac{\theta \; [\text{radians}]}{2} $$
// Therefore the only information needed to calculate an arbitrary rotation, is 
// a vector $ \hat{v} $ to rotate around, and how many degrees $d$ one wishes to 
// rotate by. 
// $$ \vec{r}^{\prime} = e^{\frac{\theta}{2}\hat{v}} \; \vec{r} \; e^{-\frac{\theta}{2}\hat{v}} $$
// ----- Rotation of Quaternions -----

namespace geodesuka::core::math {

	template <typename T>
	union quaternion {

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
