#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT4X2_H
#define GEODESUKA_CORE_MATH_MAT4X2_H

#include "config.h"

#include "vec2.h"
#include "vec4.h"

namespace geodesuka::core::math {

	template <typename T>
	union mat4x2 {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy;
			T yx, yy;
			T zx, zy;
			T tx, ty;
		};
		// Row vectors
		//struct {
		//	vec2<T> x, y, z, t;
		//};
		//vec2<T> vptr[4];
		T aptr[4][2];
#else
		// Column Major Memory Layout
		struct {
			T xx, yx, zx, tx;
			T xy, yy, zy, ty;
		};
		// Column Vectors
		//struct {
		//	vec4<T> x, y;
		//};
		//vec4<T> vptr[2];
		T aptr[2][4];
#endif // !COL_MAJOR_MEMORY_LAYOUT
		T ptr[4 * 2];

		mat4x2() {};
		mat4x2(
			T XX, T XY,
			T YX, T YY,
			T ZX, T ZY,
			T TX, T TY
		) :
			xx(XX), xy(XY),
			yx(YX), yy(YY),
			zx(ZX), zy(ZY),
			tx(TX), ty(TY)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		// Basic Addition and Subtraction
		mat4x2<T> operator-() const;
		mat4x2<T> operator+(const mat4x2<T>& Rhs) const;
		mat4x2<T> operator-(const mat4x2<T>& Rhs) const;

		mat4x2<T>& operator+=(const mat4x2<T>& Rhs);
		mat4x2<T>& operator-=(const mat4x2<T>& Rhs);

		// Multiply column vector
		vec4<T> operator*(const vec2<T>& Rhs) const;

		// Scalar Multiplication
		mat4x2<T> operator*(T Rhs) const;
		mat4x2<T> operator/(T Rhs) const;

		// Set Rows/Columns using respective vectors
		void setr(const vec2<T>& Row1, const vec2<T>& Row2, const vec2<T>& Row3, const vec2<T>& Row4);
		void setc(const vec4<T>& Col1, const vec4<T>& Col2);

	};

	template <typename T> inline
		T mat4x2<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat4x2<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat4x2<T> mat4x2<T>::operator-() const {
		mat4x2<T> temp;
		for (int i = 0; i < 4 * 2; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4x2<T> mat4x2<T>::operator+(const mat4x2<T>& Rhs) const {
		mat4x2<T> temp;
		for (int i = 0; i < 4 * 2; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4x2<T> mat4x2<T>::operator-(const mat4x2<T>& Rhs) const {
		mat4x2<T> temp;
		for (int i = 0; i < 4 * 2; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4x2<T>& mat4x2<T>::operator+=(const mat4x2<T>& Rhs) {
		for (int i = 0; i < 4 * 2; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat4x2<T>& mat4x2<T>::operator-=(const mat4x2<T>& Rhs) {
		for (int i = 0; i < 4 * 2; i++) {
			this->ptr[i] -= Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat4x2<T> mat4x2<T>::operator*(T Rhs) const {
		mat4x2<T> temp;
		for (int i = 0; i < 4 * 2; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat4x2<T> mat4x2<T>::operator/(T Rhs) const {
		mat4x2<T> temp;
		for (int i = 0; i < 4 * 2; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat4x2<T>::setr(const vec2<T>& Row1, const vec2<T>& Row2, const vec2<T>& Row3, const vec2<T>& Row4) {
		this->xx = Row1.x; this->xy = Row1.y;
		this->yx = Row2.x; this->yy = Row2.y;
		this->zx = Row3.x; this->zy = Row3.y;
		this->tx = Row4.x; this->ty = Row4.y;
	}

	template <typename T> inline
		void mat4x2<T>::setc(const vec4<T>& Col1, const vec4<T>& Col2) {
		this->xx = Col1.x; this->xy = Col2.x;
		this->yx = Col1.y; this->yy = Col2.y;
		this->zx = Col1.z; this->zy = Col2.z;
		this->tx = Col1.t; this->ty = Col2.t;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat4x2<T>& Arg) {
		for (int i = 1; i <= 4; i++) {
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

#endif // !GEODESUKA_CORE_MATH_MAT4X2_H
