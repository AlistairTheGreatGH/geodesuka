#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT3X2_H
#define GEODESUKA_CORE_MATH_MAT3X2_H

#include "config.h"

//struct mat3x2<T> {
//	T a11, a21;
//	T a12, a22;
//	T a13, a23;
//};

namespace geodesuka::core::math {

	template <typename T>
	union mat3x2 {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy;
			T yx, yy;
			T zx, zy;
		};
		// Row Vectors
		//struct {
		//	vec2<T> x;
		//	vec2<T> y;
		//	vec2<T> z;
		//};
		//vec3<T> vptr[2];
		T aptr[3][2];
#else
		// Column Major Memory Layout
		struct {
			T xx, yx, zx;
			T xy, yy, zy;
		};
		// Column Vectors
		//struct {
		//	vec3<T> x, y;
		//};
		//vec2<T> vptr[3];
		T aptr[3][2];
#endif // !COL_MAJOR_MEMORY_LAYOUT
		T ptr[2 * 3];

		mat3x2() {};
		mat3x2(
			T XX, T XY,
			T YX, T YY,
			T ZX, T ZY
		) :
			xx(XX), xy(XY),
			yx(YX), yy(YY),
			zx(ZX), zy(ZY)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		// Basic Addition and Subtraction
		mat3x2<T> operator-() const;
		mat3x2<T> operator+(const mat3x2<T>& Rhs) const;
		mat3x2<T> operator-(const mat3x2<T>& Rhs) const;

		mat3x2<T>& operator+=(const mat3x2<T>& Rhs);
		mat3x2<T>& operator-=(const mat3x2<T>& Rhs);

		// Multiply column vector
		vec3<T> operator*(const vec2<T>& Rhs) const;

		// Scalar Multiplication
		mat3x2<T> operator*(T Rhs) const;
		mat3x2<T> operator/(T Rhs) const;

		void setr(const vec2<T>& Row1, const vec2<T>& Row2, const vec2<T>& Row3);
		void setc(const vec3<T>& Col1, const vec3<T>& Col2);

	};

#ifdef MATH_ENABLE_IO
	std::ostream& operator<<(std::ostream& Out, const mat3x2<T>& Arg);
#endif // MATH_ENABLE_IO

	template <typename T> inline
		T mat3x2<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat3x2<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat3x2<T> mat3x2<T>::operator-() const {
		mat3x2<T> temp;
		for (int i = 0; i < 3 * 2; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3x2<T> mat3x2<T>::operator+(const mat3x2<T>& Rhs) const {
		mat3x2<T> temp;
		for (int i = 0; i < 3 * 2; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3x2<T> mat3x2<T>::operator-(const mat3x2<T>& Rhs) const {
		mat3x2<T> temp;
		for (int i = 0; i < 3 * 2; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3x2<T>& mat3x2<T>::operator+=(const mat3x2<T>& Rhs) {
		for (int i = 0; i < 3 * 2; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat3x2<T>& mat3x2<T>::operator-=(const mat3x2<T>& Rhs) {
		for (int i = 0; i < 3 * 2; i++) {
			this->ptr[i] -= Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		vec3<T> mat3x2<T>::operator*(const vec2<T>& Rhs) const {
		vec3<T> temp;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				temp[i] += (*this)(i, j) * Rhs[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat3x2<T> mat3x2<T>::operator*(T Rhs) const {
		mat3x2<T> temp;
		for (int i = 0; i < 3 * 2; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat3x2<T> mat3x2<T>::operator/(T Rhs) const {
		mat3x2<T> temp;
		for (int i = 0; i < 3 * 2; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat3x2<T>::setr(const vec2<T>& Row1, const vec2<T>& Row2, const vec2<T>& Row3) {
		this->xx = Row1.x; this->xy = Row1.y;
		this->yx = Row2.x; this->yy = Row2.y;
		this->zx = Row3.x; this->zy = Row3.y;
	}

	template <typename T> inline
		void mat3x2<T>::setc(const vec3<T>& Col1, const vec3<T>& Col2) {
		this->xx = Col1.x; this->xy = Col2.x;
		this->yx = Col1.y; this->yy = Col2.y;
		this->zx = Col1.z; this->zy = Col2.z;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat3x2<T>& Arg) {
		for (int i = 1; i <= 3; i++) {
			Out << "| ";
			for (int j = 1; j <= 2; j++) {
				Out << Arg(i, j) << " ";
			}
			Out << "|" << std::endl;
		}
		return Out;
	}
#endif // MATH_ENABLE_IO


}

#endif // !GEODESUKA_CORE_MATH_MAT3X2_H

