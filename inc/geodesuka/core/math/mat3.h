#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT3_H
#define GEODESUKA_CORE_MATH_MAT3_H

#include "config.h"

#include "vec3.h"

namespace geodesuka::core::math {

	template <typename T>
	union mat3 {

#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy, xz;
			T yx, yy, yz;
			T zx, zy, zz;
		};
#else
		// Column Major Memory Layout
		struct {
			T xx, yx, zx;
			T xy, yy, zy;
			T xz, yz, zz;
		};
#endif // !COL_MAJOR_MEMORY_LAYOUT
		//struct {
		//	vec3<T> x, y, z;
		//};
		//vec3<T> vptr[3];
		T aptr[3][3];
		T ptr[3 * 3];

		mat3() {};
		mat3(
			T XX, T XY, T XZ,
			T YX, T YY, T YZ,
			T ZX, T ZY, T ZZ
		) :
			xx(XX), xy(XY), xz(XZ),
			yx(YX), yy(YY), yz(YZ),
			zx(ZX), zy(ZY), zz(ZZ)
		{}
		mat3(const vec3<T>& Col1, const vec3<T>& Col2, const vec3<T>& Col3);

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		//T operator[](int2 Index) const;
		//T& operator[](int2 Index);

		mat3<T> operator-() const;
		mat3<T> operator+(const mat3<T>& Rhs) const;
		mat3<T> operator-(const mat3<T>& Rhs) const;
		mat3<T> operator*(const mat3<T>& Rhs) const;

		mat3<T>& operator+=(const mat3<T>& Rhs);
		mat3<T>& operator-=(const mat3<T>& Rhs);
		mat3<T>& operator*=(const mat3<T>& Rhs);

		vec3<T> operator*(const vec3<T>& Rhs) const;
		mat3<T> operator*(T Rhs) const;
		mat3<T> operator/(T Rhs) const;

		mat2<T> sub(int RowElement, int ColumnElement) const;
		void setr(const vec3<T>& Row1, const vec3<T>& Row2, const vec3<T>& Row3);
		void setc(const vec3<T>& Col1, const vec3<T>& Col2, const vec3<T>& Col3);

		friend vec3<T> operator*(const vec3<T>& Lhs, const mat3<T>& Rhs);
		friend mat3<T> operator*(T Lhs, const mat3<T>& Rhs);

	};

	template <typename T>
	T det(const mat3<T>& Arg);

	template <typename T>
	T tr(const mat3<T>& Arg);

	template <typename T>
	mat3<T> transpose(const mat3<T>& Arg);

	template <typename T>
	mat3<T> inverse(const mat3<T>& Arg);

#ifdef MATH_ENABLE_IO
	template <typename T>
	std::ostream& operator<<(std::ostream& Out, const mat3<T>& Arg);
#endif // MATH_ENABLE_IO

	// ------

	template <typename T> inline
		T mat3<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		return this->ptr[RowElement + ColumnElement * 3];
#else
		return this->ptr[RowElement * 3 + ColumnElement];
#endif // !COL_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat3<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		return this->ptr[RowElement + ColumnElement * 3];
#else
		return this->ptr[RowElement * 3 + ColumnElement];
#endif // !COL_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat3<T> mat3<T>::operator-() const {
		mat3<T> temp;
		for (int i = 0; i < 3 * 3; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> mat3<T>::operator+(const mat3<T>& Rhs) const {
		mat3<T> temp;
		for (int i = 0; i < 3 * 3; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> mat3<T>::operator-(const mat3<T>& Rhs) const {
		mat3<T> temp;
		for (int i = 0; i < 3 * 3; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> mat3<T>::operator*(const mat3<T>& Rhs) const {
		mat3<T> temp;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					temp(i, j) += (*this)(i, k) * Rhs(k, j);
				}
			}
		}
		return temp;
	}

	template <typename T> inline
		mat3<T>& mat3<T>::operator+=(const mat3<T>& Rhs) {
		*this = *this + Rhs;
		return *this;
	}

	template <typename T> inline
		mat3<T>& mat3<T>::operator-=(const mat3<T>& Rhs) {
		*this = *this - Rhs;
		return *this;
	}

	template <typename T> inline
		mat3<T>& mat3<T>::operator*=(const mat3<T>& Rhs) {
		*this = *this * Rhs;
		return *this;
	}

	template <typename T> inline
		vec3<T> mat3<T>::operator*(const vec3<T>& Rhs) const {
		vec3<T> temp;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				temp[i] += (*this)(i, j) * Rhs[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> mat3<T>::operator*(T Rhs) const {
		mat3<T> temp;
		for (int i = 0; i < 3 * 3; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> mat3<T>::operator/(T Rhs) const {
		mat3<T> temp;
		for (int i = 0; i < 3 * 3; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat2<T> mat3<T>::sub(int RowElement, int ColumnElement) const {
		mat2<T> temp;
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 2; j++) {
				if ((i < RowElement) && (j < ColumnElement)) {
					temp(i, j) = (*this)(i, j);
				}
				else if (i < RowElement) {
					temp(i, j) = (*this)(i, j + 1);
				}
				else if (j < ColumnElement) {
					temp(i, j) = (*this)(i + 1, j);
				}
				else {
					temp(i, j) = (*this)(i + 1, j + 1);
				}
			}
		}
		return temp;
	}

	template <typename T> inline
		void mat3<T>::setr(const vec3<T>& Row1, const vec3<T>& Row2, const vec3<T>& Row3) {
		this->xx = Row1.x; this->xy = Row1.y; this->xz = Row1.z;
		this->yx = Row2.x; this->yy = Row2.y; this->yz = Row2.z;
		this->zx = Row3.x; this->zy = Row3.y; this->zz = Row3.z;
	}

	template <typename T> inline
		void mat3<T>::setc(const vec3<T>& Col1, const vec3<T>& Col2, const vec3<T>& Col3) {
		this->xx = Col1.x; this->xy = Col2.x; this->xz = Col3.x;
		this->yx = Col1.y; this->yy = Col2.y; this->yz = Col3.y;
		this->zx = Col1.z; this->zy = Col2.z; this->zz = Col3.z;
	}

	template <typename T> inline
		vec3<T> operator*(const vec3<T>& Lhs, const mat3<T>& Rhs) {
		vec3<T> temp;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				temp[i] += Lhs[j] * Rhs(j, i);
			}
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> operator*(T Lhs, const mat3<T>& Rhs) {
		return Rhs * Lhs;
	}

	template <typename T> inline
		T det(const mat3<T>& Arg) {
		T Determinant = 0.0;
		int sign = 0;
		vec3<int> i;
		for (i.x = 0; i.x < 3; i.x++) {
			for (i.y = 0; i.y < 3; i.y++) {
				for (i.z = 0; i.z < 3; i.z++) {
					sign = -1;//levi_civita(3, i.x, i.y, i.z);
					if (sign == 0) continue;
					Determinant += ((T)sign) * (Arg(0, i.x) * Arg(1, i.y) * Arg(2, i.z));
				}
			}
		}
		return Determinant;
	}

	template <typename T> inline
		T tr(const mat3<T>& Arg) {
		return (Arg(0, 0) + Arg(1, 1) + Arg(2, 2));
	}

	template <typename T> inline
		mat3<T> transpose(const mat3<T>& Arg) {
		mat3<T> temp;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				temp(j, i) = Arg(i, j);
			}
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> inverse(const mat3<T>& Arg) {
		return (0.5 * (pow((tr(Arg)), 2.0) - tr(Arg * Arg)) * mat3<T>::I - Arg * tr(Arg) + Arg * Arg);
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat3<T>& Arg) {
		for (int i = 1; i <= 3; i++) {
			Out << "| ";
			for (int j = 1; j <= 3; j++) {
				Out << Arg(i, j) << " ";
			}
			Out << "|" << std::endl;
		}
		return Out;
	}
#endif // MATH_ENABLE_IO
}

#endif // !GEODESUKA_CORE_MATH_MAT3_H
