#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT2X4_H
#define GEODESUKA_CORE_MATH_MAT2X4_H

#include "config.h"

#include "vec2.h"
#include "vec4.h"

//struct mat2x4<T> {
//	T a11, a21, a31, a41;
//	T a12, a22, a32, a42;
//};

namespace geodesuka::core::math {

	template <typename T>
	union mat2x4 {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy, xz, xt;
			T yx, yy, yz, yt;
		};
		// Row Vectors
		//struct {
		//	vec4<T> x;
		//	vec4<T> y;
		//};
		//float3 vptr[2];
		T aptr[2][4];
#else
		// Column Major Memory Layout
		struct {
			T xx, yx;
			T xy, yy;
			T xz, yz;
			T xt, yt;
		};
		// Column Vectors
		//struct {
		//	vec2<T> x, y, z, t;
		//};
		//vec2<T> vptr[4];
		T aptr[4][2];
#endif // !COL_MAJOR_MEMORY_LAYOUT
		T ptr[2 * 4];

		mat2x4() {};
		mat2x4(
			T XX, T XY, T XZ, T XT,
			T YX, T YY, T YZ, T YT
		) :
			xx(XX), xy(XY), xz(XZ), xt(XT),
			yx(YX), yy(YY), yz(YZ), yt(YT)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		// Basic Addition and Subtraction
		mat2x4<T> operator-() const;
		mat2x4<T> operator+(const mat2x4<T>& Rhs) const;
		mat2x4<T> operator-(const mat2x4<T>& Rhs) const;

		mat2x4<T>& operator+=(const mat2x4<T>& Rhs);
		mat2x4<T>& operator-=(const mat2x4<T>& Rhs);

		// Multiply column vector
		vec2<T> operator*(const vec4<T>& Rhs) const;

		// Scalar Multiplication
		mat2x4<T> operator*(T Rhs) const;
		mat2x4<T> operator/(T Rhs) const;

		void setr(const vec4<T>& Row1, const vec4<T>& Row2);
		void setc(const vec2<T>& Col1, const vec2<T>& Col2, const vec2<T>& Col3, const vec2<T>& Col4);

	};

	template <typename T> inline
		T mat2x4<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat2x4<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat2x4<T> mat2x4<T>::operator-() const {
		mat2x4<T> temp;
		for (int i = 0; i < 2 * 4; i++) {
			temp.ptr[i] = this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2x4<T> mat2x4<T>::operator+(const mat2x4<T>& Rhs) const {
		mat2x4<T> temp;
		for (int i = 0; i < 2 * 4; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2x4<T> mat2x4<T>::operator-(const mat2x4<T>& Rhs) const {
		mat2x4<T> temp;
		for (int i = 0; i < 2 * 4; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2x4<T>& mat2x4<T>::operator+=(const mat2x4<T>& Rhs) {
		for (int i = 0; i < 2 * 4; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat2x4<T>& mat2x4<T>::operator-=(const mat2x4<T>& Rhs) {
		for (int i = 0; i < 2 * 4; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		vec2<T> mat2x4<T>::operator*(const vec4<T>& Rhs) const {
		vec2<T> temp;
		for (int i = 0; i < 2; i++) {
			temp.ptr[i] = 0.0;
			for (int j = 0; j < 4; j++) {
				temp.ptr[i] += (*this)(i + 1, j + 1) * Rhs.ptr[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat2x4<T> mat2x4<T>::operator*(T Rhs) const {
		mat2x4<T> temp;
		for (int i = 0; i < 2 * 4; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat2x4<T> mat2x4<T>::operator/(T Rhs) const {
		mat2x4<T> temp;
		for (int i = 0; i < 2 * 4; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat2x4<T>::setr(const vec4<T>& Row1, const vec4<T>& Row2) {
		this->xx = Row1.x; this->xy = Row1.y; this->xz = Row1.z; this->xt = Row1.t;
		this->yx = Row2.x; this->yy = Row2.y; this->yz = Row2.z; this->yt = Row2.t;
	}

	template <typename T> inline
		void mat2x4<T>::setc(const vec2<T>& Col1, const vec2<T>& Col2, const vec2<T>& Col3, const vec2<T>& Col4) {
		this->xx = Col1.x; this->xy = Col2.x; this->xz = Col3.x; this->xt = Col4.x;
		this->yx = Col1.y; this->yy = Col2.y; this->yz = Col3.y; this->yt = Col4.y;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat2x4<T>& Arg)
	{
		for (int i = 1; i <= 2; i++) {
			Out << "| ";
			for (int j = 1; j <= 4; j++) {
				Out << Arg(i, j) << " ";
			}
			Out << "|" << std::endl;
		}
		return Out;
	}
#endif // MATH_ENABLE_IO

}

#endif // !GEODESUKA_CORE_MATH_MAT2X4_H

