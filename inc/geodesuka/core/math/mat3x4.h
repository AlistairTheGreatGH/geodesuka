#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT3X4_H
#define GEODESUKA_CORE_MATH_MAT3X4_H

namespace geodesuka::core::math {

	template <typename T>
	union mat3x4 {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		struct {
			T xx, xy, xz, xt;
			T yx, yy, yz, yt;
			T zx, zy, zz, zt;
		};
		//struct {
		//	vec4<T> x, y, z;
		//};
		//vec4<T> vptr[3];
		T aptr[3][4];
#else
		// Column Major Memory Layout
		struct {
			T xx, yx, zx;
			T xy, yy, zy;
			T xz, yz, zz;
			T xt, yt, zt;
		};
		//struct {
		//	vec3<T> x, y, z, t;
		//};
		//vec3<T> vptr[4];
		T aptr[4][3];
#endif // !COL_MAJOR_MEMORY_LAYOUT
		T ptr[3 * 4];

		mat3x4() {};
		mat3x4(
			T XX, T XY, T XZ, T XT,
			T YX, T YY, T YZ, T YT,
			T ZX, T ZY, T ZZ, T ZT
		) :
			xx(XX), xy(XY), xz(XZ), xt(XT),
			yx(YX), yy(YY), yz(YZ), yt(YT),
			zx(ZX), zy(ZY), zz(ZZ), zt(ZT)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		// Basic Addition and Subtraction
		mat3x4<T> operator-() const;
		mat3x4<T> operator+(const mat3x4<T>& Rhs) const;
		mat3x4<T> operator-(const mat3x4<T>& Rhs) const;

		mat3x4<T>& operator+=(const mat3x4<T>& Rhs);
		mat3x4<T>& operator-=(const mat3x4<T>& Rhs);

		// Multiply column vector
		vec3<T> operator*(const vec4<T>& Rhs) const;

		// Scalar Multiplication
		mat3x4<T> operator*(T Rhs) const;
		mat3x4<T> operator/(T Rhs) const;

		// Set Rows/Columns using respective vectors
		void setr(const vec4<T>& Row1, const vec4<T>& Row2, const vec4<T>& Row3);
		void setc(const vec3<T>& Col1, const vec3<T>& Col2, const vec3<T>& Col3, const vec3<T>& Col4);

	};

#ifdef MATH_ENABLE_IO
	std::ostream& operator<<(std::ostream& Out, const mat3x4<T>& Arg);
#endif // MATH_ENABLE_IO

	template <typename T> inline
		T mat3x4<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat3x4<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		// Row Major Memory Layout
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		// Column Major Memory Layout
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat3x4<T> mat3x4<T>::operator-() const {
		mat3x4<T> temp;
		for (int i = 0; i < 3 * 4; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3x4<T> mat3x4<T>::operator+(const mat3x4<T>& Rhs) const {
		mat3x4<T> temp;
		for (int i = 0; i < 3 * 4; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3x4<T> mat3x4<T>::operator-(const mat3x4<T>& Rhs) const {
		mat3x4<T> temp;
		for (int i = 0; i < 3 * 4; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat3x4<T>& mat3x4<T>::operator+=(const mat3x4<T>& Rhs) {
		for (int i = 0; i < 3 * 4; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat3x4<T>& mat3x4<T>::operator-=(const mat3x4<T>& Rhs) {
		for (int i = 0; i < 3 * 4; i++) {
			this->ptr[i] -= Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat3x4<T> mat3x4<T>::operator*(T Rhs) const {
		mat3x4<T> temp;
		for (int i = 0; i < 3 * 4; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat3x4<T> mat3x4<T>::operator/(T Rhs) const {
		mat3x4<T> temp;
		for (int i = 0; i < 3 * 4; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		void mat3x4<T>::setr(const vec4<T>& Row1, const vec4<T>& Row2, const vec4<T>& Row3) {
		this->xx = Row1.x; this->xy = Row1.y; this->xz = Row1.z; this->xt = Row1.t;
		this->yx = Row2.x; this->yy = Row2.y; this->yz = Row2.z; this->yt = Row2.t;
		this->zx = Row3.x; this->zy = Row3.y; this->zz = Row3.z; this->zt = Row3.t;
	}

	template <typename T> inline
		void mat3x4<T>::setc(const vec3<T>& Col1, const vec3<T>& Col2, const vec3<T>& Col3, const vec3<T>& Col4) {
		this->xx = Col1.x; this->xy = Col2.x; this->xz = Col3.x; this->xt = Col4.x;
		this->yx = Col1.y; this->yy = Col2.y; this->yz = Col3.y; this->yt = Col4.y;
		this->zx = Col1.z; this->zy = Col2.z; this->zz = Col3.z; this->zt = Col4.z;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat3x4<T>& Arg)
	{
		for (int i = 1; i <= 3; i++) {
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

#endif // !GEODESUKA_CORE_MATH_MAT3X4_H
