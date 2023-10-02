#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT2_H
#define GEODESUKA_CORE_MATH_MAT2_H

#include "config.h"

#include "vec2.h"

namespace geodesuka::core::math {

	template <typename T>
	union mat2 {

		// Data
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy;
			T yx, yy;
		};
#else
		// Column Major Memory Layout
		struct {
			T xx, yx;
			T xy, yy;
		};
#endif
		//struct {
		//	vec2<T> x;
		//	vec2<T> y;
		//};
		//vec2<T> vptr[2];
		T aptr[2][2];
		T ptr[2 * 2];


		mat2() {};
		mat2(
			T XX, T XY,
			T YX, T YY
		) :
			xx(XX), xy(XY),
			yx(YX), yy(YY)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		mat2<T> operator-() const;
		mat2<T> operator+(const mat2<T>& Rhs) const;
		mat2<T> operator-(const mat2<T>& Rhs) const;
		mat2<T> operator*(const mat2<T>& Rhs) const;

		mat2<T> operator+=(const mat2<T>& Rhs);
		mat2<T> operator-=(const mat2<T>& Rhs);
		mat2<T> operator*=(const mat2<T>& Rhs);

		vec2<T> operator*(const vec2<T>& Rhs) const;
		mat2<T> operator*(T Rhs) const;
		mat2<T> operator/(T Rhs) const;

		//T sub(integer RowElement, integer ColumnElement) const;
		void setr(const vec2<T>& Row1, const vec2<T>& Row2);
		void setc(const vec2<T>& Col1, const vec2<T>& Col2);

	};

	template <typename T> inline
		T mat2<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat2<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator-() const {
		mat2<T> temp;
		for (int i = 0; i < 2 * 2; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator+(const mat2<T>& Rhs) const {
		mat2<T> temp;
		for (int i = 0; i < 2 * 2; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator-(const mat2<T>& Rhs) const {
		mat2<T> temp;
		for (int i = 0; i < 2 * 2; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator*(const mat2<T>& Rhs) const {
		mat2<T> temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 2; k++) {
					temp(i, j) += (*this)(i, k) * Rhs(k, j);
				}
			}
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator+=(const mat2<T>& Rhs) {
		*this = *this + Rhs;
		return *this;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator-=(const mat2<T>& Rhs) {
		*this = *this - Rhs;
		return *this;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator*=(const mat2<T>& Rhs) {
		*this = *this * Rhs;
		return *this;
	}

	template <typename T> inline
		vec2<T> mat2<T>::operator*(const vec2<T>& Rhs) const {
		vec2<T> temp;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				temp[i] += (*this)(i, j) * Rhs[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator*(T Rhs) const {
		mat2<T> temp;
		for (int i = 0; i < 2 * 2; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat2<T>::operator/(T Rhs) const {
		mat2<T> temp;
		for (int i = 0; i < 2 * 2; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat2<T>::setr(const vec2<T>& Row1, const vec2<T>& Row2) {
		this->xx = Row1.x; this->xy = Row1.y;
		this->yx = Row2.x; this->yy = Row2.y;
	}

	template <typename T> inline
		void mat2<T>::setc(const vec2<T>& Col1, const vec2<T>& Col2) {
		this->xx = Col1.x; this->xy = Col2.x;
		this->yx = Col1.y; this->yy = Col2.y;
	}

	template <typename T> inline
		vec2<T> operator*(const vec2<T>& Lhs, const mat2<T>& Rhs) {
		vec2<T> temp;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				temp[i] += Rhs(j, i) * Lhs[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> operator*(T Lhs, const mat2<T>& Rhs) {
		return Rhs * Lhs;
	}

	template <typename T> inline
		T det(const mat2<T>& Arg) {
		return (Arg.xx * Arg.yy - Arg.yx * Arg.xy);
	}

	template <typename T> inline
		T tr(const mat2<T>& Arg) {
		return (Arg.xx + Arg.yy);
	}

	template <typename T> inline
		mat2<T> transpose(const mat2<T>& Arg) {
		mat2<T> temp;
		temp.xx = Arg.xx; temp.xy = Arg.yx;
		temp.yx = Arg.xy; temp.yy = Arg.yy;
		return temp;
	}

	template <typename T> inline
		mat2<T> inverse(const mat2<T>& Arg) {
		return (
			mat2<T>(
				Arg.yy, -Arg.xy,
				-Arg.yx, Arg.xx
			) / det(Arg)
			);
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat2<T>& Arg) {
		for (int i = 1; i <= 2; i++) {
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

#endif // !GEODESUKA_CORE_MATH_MAT2_H

