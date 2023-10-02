#pragma once
#ifndef GEODESUKA_CORE_MATH_COMPLEX_H
#define GEODESUKA_CORE_MATH_COMPLEX_H

#include <cstddef>
#include <cmath>

#include "config.h"
#include "type.h"
#include "constants.h"

namespace geodesuka::core::math {

	template <typename T>
	union complex {
		struct {
			T a, b;
		};
		struct {
			T x, y;
		};
		T ptr[2];

		complex();
		complex(T aReal);
		complex(T aReal, T aImag);

		T& operator[](int aIndex);

		complex<T>& operator=(T aRhs);
		complex<T> operator-() const;
		complex<T> operator~() const;
		complex<T> operator+(const complex<T>& aRhs) const;
		complex<T> operator-(const complex<T>& aRhs) const;
		complex<T> operator*(const complex<T>& aRhs) const;
		complex<T> operator/(const complex<T>& aRhs) const;

		complex<T>& operator+=(const complex<T>& aRhs);
		complex<T>& operator-=(const complex<T>& aRhs);
		complex<T>& operator*=(const complex<T>& aRhs);
		complex<T>& operator/=(const complex<T>& aRhs);

		complex<T> operator+(T aRhs) const;
		complex<T> operator-(T aRhs) const;
		complex<T> operator*(T aRhs) const;
		complex<T> operator/(T aRhs) const;

		complex<T>& operator+=(T aRhs);
		complex<T>& operator-=(T aRhs);
		complex<T>& operator*=(T aRhs);
		complex<T>& operator/=(T aRhs);

	};

	template <typename T> 
	inline complex<T>::complex() {
		this->a = 0.0; this->b = 0.0;
	}

	template <typename T> 
	inline complex<T>::complex(T aReal) {
		this->a = aReal; this->b = 0.0;
	}

	template <typename T> 
	inline complex<T>::complex(T aReal, T aImag) {
		this->a = aReal; this->b = aImag;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator=(T aRhs) {
		this->a = aRhs; this->b = 0.0;
		return *this;
	}

	template <typename T> 
	inline T& complex<T>::operator[](int aIndex) {
		return ptr[aIndex];
	}

	template <typename T> 
	inline complex<T> complex<T>::operator-() const {
		return complex<T>(-this->a, -this->b);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator~() const {
		return complex<T>(this->a, -this->b);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator+(const complex<T>& aRhs) const {
		return complex<T>(this->a + aRhs.a, this->b + aRhs.b);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator-(const complex<T>& aRhs) const {
		return complex<T>(this->a - aRhs.a, this->b - aRhs.b);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator*(const complex<T>& aRhs) const {
		return complex<T>(this->a * aRhs.a - this->b * aRhs.b, this->a * aRhs.b + this->b * aRhs.a);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator/(const complex<T>& aRhs) const {
		T NewR = abs(*this) / abs(aRhs);
		T DeltaPhase = phase(*this) - phase(aRhs);
		return NewR * complex<T>(std::cos(DeltaPhase), std::sin(DeltaPhase));
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator+=(const complex<T>& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator-=(const complex<T>& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator*=(const complex<T>& aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator/=(const complex<T>& aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T> complex<T>::operator+(T aRhs) const {
		return complex<T>(this->a + aRhs, this->b);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator-(T aRhs) const {
		return complex<T>(this->a - aRhs, this->b);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator*(T aRhs) const {
		return complex<T>(this->a * aRhs, this->b * aRhs);
	}

	template <typename T> 
	inline complex<T> complex<T>::operator/(T aRhs) const {
		return complex<T>(this->a / aRhs, this->b / aRhs);
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator+=(T aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator-=(T aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator*=(T aRhs) {
		*this = *this * aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T>& complex<T>::operator/=(T aRhs) {
		*this = *this / aRhs;
		return *this;
	}

	template <typename T> 
	inline complex<T> operator+(T Lhs, const complex<T>& aRhs) {
		return complex<T>(Lhs + aRhs.a, aRhs.b);
	}

	template <typename T> 
	inline complex<T> operator-(T Lhs, const complex<T>& aRhs) {
		return complex<T>(Lhs - aRhs.a, -aRhs.b);
	}

	template <typename T> 
	inline complex<T> operator*(T Lhs, const complex<T>& aRhs) {
		return complex<T>(Lhs * aRhs.a, Lhs * aRhs.b);
	}

	template <typename T> 
	inline complex<T> operator/(T Lhs, const complex<T>& aRhs) {
		return (Lhs / abs2(aRhs)) * conj(aRhs);
	}

	template <typename T> 
	inline T re(const complex<T>& Arg) {
		return Arg.a;
	}

	template <typename T> 
	inline T im(const complex<T>& Arg) {
		return Arg.b;
	}

	template <typename T> 
	inline T abs2(const complex<T>& Arg) {
		return Arg.a * Arg.a + Arg.b * Arg.b;
	}

	template <typename T> 
	inline T abs(const complex<T>& Arg) {
		return std::sqrt(abs2(Arg));
	}

	template <typename T> 
	inline T phase(const complex<T>& Arg) {
		return std::atan2(Arg.b, Arg.a);
	}

	template <typename T> 
	inline complex<T> conj(const complex<T>& Arg) {
		return complex<T>(Arg.a, -Arg.b);
	}

	template <typename T> 
	inline complex<T> sqrt(const complex<T>& Arg) {
		T Mag = std::sqrt(abs(Arg));
		T Angle = 0.5 * phase(Arg);
		return Mag * complex<T>(std::cos(Angle), std::sin(Angle));
	}

	template <typename T> 
	inline complex<T> exp(const complex<T>& Arg) {
		return std::exp(Arg.a) * complex<T>(std::cos(Arg.b), std::sin(Arg.b));
	}

	template <typename T> 
	inline complex<T> ln(const complex<T>& Arg) {
		return complex<T>(ln(abs(Arg)), phase(Arg));
	}

	template <typename T> 
	inline complex<T> pow(const complex<T>& Base, const complex<T>& Exponent) {
		return exp(Exponent * (ln(abs(Base)) + complex<T>::i * phase(Base)));
	}

	template <typename T> 
	inline complex<T> sin(const complex<T>& Arg) {
		return complex<T>(std::sin(Arg.a) * std::cosh(Arg.b), std::cos(Arg.a) * std::sinh(Arg.b));
	}

	template <typename T> 
	inline complex<T> cos(const complex<T>& Arg) {
		return complex<T>(std::cos(Arg.a) * std::cosh(Arg.b), -std::sin(Arg.a) * std::sinh(Arg.b));
	}

	template <typename T> 
	inline complex<T> tan(const complex<T>& Arg) {
		return sin(Arg) / cos(Arg);
	}

	template <typename T> 
	inline complex<T> sinh(const complex<T>& Arg) {
		return complex<T>(std::sinh(Arg.a) * std::cos(Arg.b), std::cosh(Arg.a) * std::sin(Arg.b));
	}

	template <typename T> 
	inline complex<T> cosh(const complex<T>& Arg) {
		return complex<T>(std::cosh(Arg.a) * std::cos(Arg.b), std::sinh(Arg.a) * std::sin(Arg.b));
	}

	template <typename T> 
	inline complex<T> tanh(const complex<T>& Arg) {
		return (sinh(Arg) / cosh(Arg));
	}

	template <typename T> 
	inline complex<T> asin(const complex<T>& Arg) {
		return (-complex<T>::i * ln(complex<T>::i * Arg + sqrt(1.0 - pow(Arg, 2.0))));
	}

	template <typename T> 
	inline complex<T> acos(const complex<T>& Arg) {
		return (0.5 * constant::pi + complex<T>::i * ln(complex<T>::i * Arg + sqrt(1.0 - pow(Arg, 2.0))));
	}

	template <typename T> 
	inline complex<T> atan(const complex<T>& Arg) {
		return (0.5 * complex<T>::i * (ln(1.0 - complex<T>::i * Arg) - ln(1.0 + complex<T>::i * Arg)));
	}

	template <typename T> 
	inline complex<T> asinh(const complex<T>& Arg) {
		return ln(Arg + sqrt(pow(Arg, 2.0) + 1.0));
	}

	template <typename T> 
	inline complex<T> acosh(const complex<T>& Arg) {
		return ln(Arg + sqrt(Arg - 1.0) * sqrt(Arg + 1.0));
	}

	template <typename T> 
	inline complex<T> atanh(const complex<T>& Arg) {
		return (0.5 * (ln(1.0 + Arg) - ln(1.0 - Arg)));
	}

}

#endif // !GEODESUKA_CORE_MATH_COMPLEX_H
