#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT4X3_H
#define GEODESUKA_CORE_MATH_MAT4X3_H

#include "config.h"

#include "vec3.h"
#include "vec4.h"

//struct mat4x3<T> {
//	T xx, yx, zx;
//	T xy, yy, zy;
//	T xz, yz, zz;
//	T xt, yt, zt;
//};

namespace geodesuka::core::math {

	template <typename T>
	union mat4x3 {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy, xz;
			T yx, yy, yz;
			T zx, zy, zz;
			T tx, ty, tz;
		};
		// Row vectors
		//struct {
		//	vec3<T> x, y, z, t;
		//};
		//vec3<T> vptr[4];
		T aptr[4][3];
#else
		// Column Major Memory Layout
		struct {
			T xx, yx, zx, tx;
			T xy, yy, zy, ty;
			T xz, yz, zz, tz;
		};
		// Column Vectors
		//struct {
		//	vec4<T> x, y, z;
		//};
		//vec4<T> vptr[3];
		T aptr[3][4];
#endif // !COL_MAJOR_MEMORY_LAYOUT
		T ptr[4 * 3];

		mat4x3() {};
		mat4x3(
			T XX, T XY, T XZ,
			T YX, T YY, T YZ,
			T ZX, T ZY, T ZZ,
			T TX, T TY, T TZ
		) :
			xx(XX), xy(XY), xz(XZ),
			yx(YX), yy(YY), yz(YZ),
			zx(ZX), zy(ZY), zz(ZZ),
			tx(TX), ty(TY), tz(TZ)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		// Basic Addition and Subtraction
		mat4x3<T> operator-() const;
		mat4x3<T> operator+(const mat4x3<T>& Rhs) const;
		mat4x3<T> operator-(const mat4x3<T>& Rhs) const;

		mat4x3<T>& operator+=(const mat4x3<T>& Rhs);
		mat4x3<T>& operator-=(const mat4x3<T>& Rhs);

		// Multiply column vector
		vec4<T> operator*(const vec3<T>& Rhs) const;

		// Scalar Multiplication
		mat4x3<T> operator*(T Rhs) const;
		mat4x3<T> operator/(T Rhs) const;

		// Set Rows/Columns using respective vectors
		void setr(const vec3<T>& Row1, const vec3<T>& Row2, const vec3<T>& Row3, const vec3<T>& Row4);
		void setc(const vec4<T>& Col1, const vec4<T>& Col2, const vec4<T>& Col3);

	};

	template <typename T> inline
		T mat4x3<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat4x3<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat4x3<T> mat4x3<T>::operator-() const {
		mat4x3<T> temp;
		for (int i = 0; i < 4 * 3; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4x3<T> mat4x3<T>::operator+(const mat4x3<T>& Rhs) const {
		mat4x3<T> temp;
		for (int i = 0; i < 4 * 3; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4x3<T> mat4x3<T>::operator-(const mat4x3<T>& Rhs) const {
		mat4x3<T> temp;
		for (int i = 0; i < 4 * 3; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4x3<T>& mat4x3<T>::operator+=(const mat4x3<T>& Rhs) {
		for (int i = 0; i < 4 * 3; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat4x3<T>& mat4x3<T>::operator-=(const mat4x3<T>& Rhs) {
		for (int i = 0; i < 4 * 3; i++) {
			this->ptr[i] -= Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		vec4<T> mat4x3<T>::operator*(const vec3<T>& Rhs) const {
		return vec4<T>();
	}

	template <typename T> inline
		mat4x3<T> mat4x3<T>::operator*(T Rhs) const {
		mat4x3<T> temp;
		for (int i = 0; i < 4 * 3; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat4x3<T> mat4x3<T>::operator/(T Rhs) const {
		mat4x3<T> temp;
		for (int i = 0; i < 4 * 3; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat4x3<T>::setr(const vec3<T>& Row1, const vec3<T>& Row2, const vec3<T>& Row3, const vec3<T>& Row4) {
		this->xx = Row1.x; this->xy = Row1.y; this->xz = Row1.z;
		this->yx = Row2.x; this->yy = Row2.y; this->yz = Row2.z;
		this->zx = Row3.x; this->zy = Row3.y; this->zz = Row3.z;
		this->tx = Row4.x; this->ty = Row4.y; this->tz = Row4.z;
	}

	template <typename T> inline
		void mat4x3<T>::setc(const vec4<T>& Col1, const vec4<T>& Col2, const vec4<T>& Col3) {
		this->xx = Col1.x; this->xy = Col2.x; this->xz = Col3.x;
		this->yx = Col1.y; this->yy = Col2.y; this->yz = Col3.y;
		this->zx = Col1.z; this->zy = Col2.z; this->zz = Col3.z;
		this->tx = Col1.t; this->ty = Col2.t; this->tz = Col3.t;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat4x3<T>& Arg) {
		for (int i = 1; i <= 4; i++) {
			Out << "| ";
			for (int j = 1; j <= 3; j++) {
				if (Arg(i, j) >= 0) {
					Out << " ";
				}
				Out << Arg(i, j) << " ";
			}
			Out << "|" << std::endl;
		}
		return Out;
	}
#endif // MATH_ENABLE_IO 

}

#endif // !GEODESUKA_CORE_MATH_MAT4X3_H

