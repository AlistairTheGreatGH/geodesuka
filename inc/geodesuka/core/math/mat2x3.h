#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT2X3_H
#define GEODESUKA_CORE_MATH_MAT2X3_H

//T A[3][2] = { {0,1}, {2,3}, {4,5} };
//T A[2][3] = { {0,1,2}, {3,4,5} };

#include "config.h"

#include "vec2.h"
#include "vec3.h"

namespace geodesuka::core::math {

	template <typename T>
	union mat2x3 {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy, xz;
			T yx, yy, yz;
		};
		// Row Vectors
		//struct {
		//	vec3<T> x;
		//	vec3<T> y;
		//};
		//vec3<T> vptr[2];
		T aptr[2][3];
#else
		// Column Major Memory Layout
		struct {
			T xx, yx;
			T xy, yy;
			T xz, yz;
		};
		// Column Vectors
		//struct {
		//	vec2<T> x, y, z;
		//};
		//vec2<T> vptr[3];
		T aptr[3][2];
#endif
		T ptr[2 * 3];

		mat2x3() {};
		mat2x3(
			T XX, T XY, T XZ,
			T YX, T YY, T YZ
		) :
			xx(XX), xy(XY), xz(XZ),
			yx(YX), yy(YY), yz(YZ)
		{}
		//mat2x3<T>(const vec2<T>& Col1, const vec2<T>& Col2, const vec2<T>& Col3) :
		//	xx(Col1.x), xy(Col2.x), xz(Col3.x),
		//	yx(Col1.y), yy(Col2.y), yz(Col3.y)
		//{}

		mat2x3<T>& operator=(const mat2x3<T>& Rhs);

		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		// Basic Addition and Subtraction
		mat2x3<T> operator-() const;
		mat2x3<T> operator+(const mat2x3<T>& Rhs) const;
		mat2x3<T> operator-(const mat2x3<T>& Rhs) const;

		mat2x3<T>& operator+=(const mat2x3<T>& Rhs);
		mat2x3<T>& operator-=(const mat2x3<T>& Rhs);

		// Multiply column vector
		vec2<T> operator*(const vec3<T>& Rhs) const;

		// Scalar Multiplication/Division
		mat2x3<T> operator*(T Rhs) const;
		mat2x3<T> operator/(T Rhs) const;

		void setr(const vec3<T>& Row1, const vec3<T>& Row2);
		void setc(const vec2<T>& Col1, const vec2<T>& Col2, const vec2<T>& Col3);

	};

	template <typename T> inline
		T mat2x3<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat2x3<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat2x3<T> mat2x3<T>::operator-() const {
		mat2x3<T> temp;
		for (int i = 0; i < 2 * 3; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2x3<T> mat2x3<T>::operator+(const mat2x3<T>& Rhs) const {
		mat2x3<T> temp;
		for (int i = 0; i < 2 * 3; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2x3<T> mat2x3<T>::operator-(const mat2x3<T>& Rhs) const {
		mat2x3<T> temp;
		for (int i = 0; i < 2 * 3; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2x3<T>& mat2x3<T>::operator+=(const mat2x3<T>& Rhs) {
		for (int i = 0; i < 2 * 3; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat2x3<T>& mat2x3<T>::operator-=(const mat2x3<T>& Rhs) {
		for (int i = 0; i < 2 * 3; i++) {
			this->ptr[i] -= Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		vec2<T> mat2x3<T>::operator*(const vec3<T>& Rhs) const {
		vec2<T> temp;
		for (int i = 0; i < 2; i++) {
			temp[i] = 0.0;
			for (int j = 0; j < 3; j++) {
				temp[i] += (*this)(i, j) * Rhs[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat2x3<T> mat2x3<T>::operator*(T Rhs) const {
		mat2x3<T> temp;
		for (int i = 0; i < 2 * 3; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat2x3<T> mat2x3<T>::operator/(T Rhs) const {
		mat2x3<T> temp;
		for (int i = 0; i < 2 * 3; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat2x3<T>::setr(const vec3<T>& Row1, const vec3<T>& Row2) {
		this->xx = Row1.x; this->xy = Row1.y; this->xz = Row1.z;
		this->yx = Row2.x; this->yy = Row2.y; this->yz = Row2.z;
	}

	template <typename T> inline
		void mat2x3<T>::setc(const vec2<T>& Col1, const vec2<T>& Col2, const vec2<T>& Col3) {
		this->xx = Col1.x; this->xy = Col2.x; this->xz = Col3.x;
		this->yx = Col1.y; this->yy = Col2.y; this->yz = Col3.y;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat2x3<T>& Arg) {
		for (int i = 1; i <= 2; i++) {
			Out << "| ";
			for (int j = 1; j <= 3; j++) {
				Out << Arg(i, j) << " ";
			}
			Out << "|" << std::endl;
		}
		return Out;
	}
#endif // MATH_ENABLE_IO

	template <typename T>
	vec3<T> operator*(const vec2<T>& Lhs, const mat2x3<T>& Rhs);

	template <typename T>
	mat2x3<T> operator*(T Lhs, const mat2x3<T>& Rhs);

}

#endif // !GEODESUKA_CORE_MATH_MAT2X3_H
