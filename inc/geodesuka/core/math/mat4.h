#pragma once
#ifndef GEODESUKA_CORE_MATH_MAT4_H
#define GEODESUKA_CORE_MATH_MAT4_H

#include "vec4.h"

namespace geodesuka::core::math {

	template <typename T>
	union mat4 {

		// Data
#ifdef ROW_MAJOR_MEMORY_LAYOUT
	// Row Major Memory Layout
		struct {
			T xx, xy, xz, xt;
			T yx, yy, yz, yt;
			T zx, zy, zz, zt;
			T tx, ty, tz, tt;
		};
#else
	// Column Major Memory Layout
		struct {
			T xx, yx, zx, tx;
			T xy, yy, zy, ty;
			T xz, yz, zz, tz;
			T xt, yt, zt, tt;
		};
#endif // !COL_MAJOR_MEMORY_LAYOUT
		//struct {
		//	vec4<T> x, y, z, t;
		//};
		//vec4<T> vptr[4];
		T aptr[4][4];
		T ptr[4 * 4];

		mat4() {};
		mat4(
			T XX, T XY, T XZ, T XT,
			T YX, T YY, T YZ, T YT,
			T ZX, T ZY, T ZZ, T ZT,
			T TX, T TY, T TZ, T TT
		) :
			xx(XX), xy(XY), xz(XZ), xt(XT),
			yx(YX), yy(YY), yz(YZ), yt(YT),
			zx(ZX), zy(ZY), zz(ZZ), zt(ZT),
			tx(TX), ty(TY), tz(TZ), tt(TT)
		{}
		mat4(
			const mat2<T>& A, const mat2<T>& B,
			const mat2<T>& C, const mat2<T>& D
		) :
			xx(A.xx), xy(A.xy), xz(B.xx), xt(B.xy),
			yx(A.yx), yy(A.yy), yz(B.yx), yt(B.yy),
			zx(C.xx), zy(C.xy), zz(D.xx), zt(D.xy),
			tx(C.yx), ty(C.yy), tz(D.yx), tt(D.yy)
		{}
		mat4(const vec4<T>& Col1, const vec4<T>& Col2, const vec4<T>& Col3, const vec4<T>& Col4) :
			xx(Col1.x), xy(Col2.x), xz(Col3.x), xt(Col4.x),
			yx(Col1.y), yy(Col2.y), yz(Col3.y), yt(Col4.y),
			zx(Col1.z), zy(Col2.z), zz(Col3.z), zt(Col4.z),
			tx(Col1.t), ty(Col2.t), tz(Col3.t), tt(Col4.t)
		{}

		// Access elements of matrix in standard notation.
		T operator()(int RowElement, int ColumnElement) const;
		T& operator()(int RowElement, int ColumnElement);

		mat4<T> operator-() const;
		mat4<T> operator+(const mat4<T>& Rhs) const;
		mat4<T> operator-(const mat4<T>& Rhs) const;
		mat4<T> operator*(const mat4<T>& Rhs) const;

		mat4<T>& operator+=(const mat4<T>& Rhs);
		mat4<T>& operator-=(const mat4<T>& Rhs);
		mat4<T>& operator*=(const mat4<T>& Rhs);

		// Multiply column vector
		vec4<T> operator*(const vec4<T>& Rhs) const;
		// Scalar Multiplication
		mat4<T> operator*(T Rhs) const;
		mat4<T> operator/(T Rhs) const;

		mat3<T> sub(int RowElement, int ColumnElement) const;
		void setr(const vec4<T>& Row1, const vec4<T>& Row2, const vec4<T>& Row3, const vec4<T>& Row4);
		void setc(const vec4<T>& Col1, const vec4<T>& Col2, const vec4<T>& Col3, const vec4<T>& Col4);

	};

	template <typename T>
	vec4<T> operator*(const vec4<T>& Lhs, const mat4<T>& Rhs);

	template <typename T>
	mat4<T> operator*(T Lhs, const mat4<T>& Rhs);

	template <typename T>
	T det(const mat4<T>& Arg);

	template <typename T>
	T tr(const mat4<T>& Arg);

	//template <typename T>
	//mat4<T> diag(const mat4<T>& Arg);

	template <typename T>
	mat4<T> transpose(const mat4<T>& Arg);

	//template <typename T>
	//mat4<T> inverse(const mat4<T>& Arg);

#ifdef MATH_ENABLE_IO
	std::ostream& operator<<(std::ostream& Out, const mat4<T>& Arg);
#endif // MATH_ENABLE_IO

	template <typename T> inline
		T mat4<T>::operator()(int RowElement, int ColumnElement) const {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		T& mat4<T>::operator()(int RowElement, int ColumnElement) {
#ifdef ROW_MAJOR_MEMORY_LAYOUT
		return this->aptr[RowElement - 1][ColumnElement - 1];
#else
		return this->aptr[ColumnElement - 1][RowElement - 1];
#endif // !ROW_MAJOR_MEMORY_LAYOUT
	}

	template <typename T> inline
		mat4<T> mat4<T>::operator-() const {
		mat4<T> temp;
		for (int i = 0; i < 4 * 4; i++) {
			temp.ptr[i] = -this->ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4<T> mat4<T>::operator+(const mat4<T>& Rhs) const {
		mat4<T> temp;
		for (int i = 0; i < 4 * 4; i++) {
			temp.ptr[i] = this->ptr[i] + Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4<T> mat4<T>::operator-(const mat4<T>& Rhs) const {
		mat4<T> temp;
		for (int i = 0; i < 4 * 4; i++) {
			temp.ptr[i] = this->ptr[i] - Rhs.ptr[i];
		}
		return temp;
	}

	template <typename T> inline
		mat4<T> mat4<T>::operator*(const mat4<T>& Rhs) const {
		// C = A * B ---> c_ij = a_ik * b_kj;
		mat4<T> temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 4; k++) {
					temp(i, j) += (*this)(i, k) * Rhs(k, j);
				}
			}
		}
		return temp;
	}

	template <typename T> inline
		mat4<T>& mat4<T>::operator+=(const mat4<T>& Rhs) {
		for (int i = 0; i < 4 * 4; i++) {
			this->ptr[i] += Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat4<T>& mat4<T>::operator-=(const mat4<T>& Rhs) {
		for (int i = 0; i < 4 * 4; i++) {
			this->ptr[i] -= Rhs.ptr[i];
		}
		return *this;
	}

	template <typename T> inline
		mat4<T>& mat4<T>::operator*=(const mat4<T>& Rhs) {
		*this = *this * Rhs;
		return *this;
	}

	template <typename T> inline
		vec4<T> mat4<T>::operator*(const vec4<T>& Rhs) const {
		vec4<T> temp;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp[i] += (*this)(i + 1, j + 1) * Rhs[j];
			}
		}
		return temp;
	}

	template <typename T> inline
		mat4<T> mat4<T>::operator*(T Rhs) const {
		mat4<T> temp;
		for (int i = 0; i < 4 * 4; i++) {
			temp.ptr[i] = this->ptr[i] * Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat4<T> mat4<T>::operator/(T Rhs) const {
		mat4<T> temp;
		for (int i = 0; i < 4 * 4; i++) {
			temp.ptr[i] = this->ptr[i] / Rhs;
		}
		return temp;
	}

	template <typename T> inline
		mat3<T> mat4<T>::sub(int RowElement, int ColumnElement) const {
		mat3<T> temp;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
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
		void mat4<T>::setr(const vec4<T>& Row1, const vec4<T>& Row2, const vec4<T>& Row3, const vec4<T>& Row4) {
		this->xx = Row1.x; this->xy = Row1.y; this->xz = Row1.z; this->xt = Row1.t;
		this->yx = Row2.x; this->yy = Row2.y; this->yz = Row2.z; this->yt = Row2.t;
		this->zx = Row3.x; this->zy = Row3.y; this->zz = Row3.z; this->zt = Row3.t;
		this->tx = Row4.x; this->ty = Row4.y; this->tz = Row4.z; this->tt = Row4.t;
	}

	template <typename T> inline
		void mat4<T>::setc(const vec4<T>& Col1, const vec4<T>& Col2, const vec4<T>& Col3, const vec4<T>& Col4) {
		this->xx = Col1.x; this->xy = Col2.x; this->xz = Col3.x; this->xt = Col4.x;
		this->yx = Col1.y; this->yy = Col2.y; this->yz = Col3.y; this->yt = Col4.y;
		this->zx = Col1.z; this->zy = Col2.z; this->zz = Col3.z; this->zt = Col4.z;
		this->tx = Col1.t; this->ty = Col2.t; this->tz = Col3.t; this->tt = Col4.t;
	}

	template <typename T> inline
		vec4<T> operator*(const vec4<T>& Lhs, const mat4<T>& Rhs) {
		vec4<T> temp;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp[i] += Lhs[j] * Rhs(j, i);
			}
		}
		return temp;
	}

	template <typename T> inline
		mat4<T> operator*(T Lhs, const mat4<T>& Rhs) {
		return Rhs * Lhs;
	}

	template <typename T> inline
		T det(const mat4<T>& Arg) {
		T Determinant = 0.0;
		int sign = 0;
		vec4<int> i;
		for (i.x = 0; i.x < 4; i.x++) {
			for (i.y = 0; i.y < 4; i.y++) {
				for (i.z = 0; i.z < 4; i.z++) {
					for (i.t = 0; i.t < 4; i.t++) {
						sign = -1;// levi_civita(4, i.x, i.y, i.z, i.t);
						if (sign == 0) continue;
						Determinant += ((T)sign) * (Arg(0, i.x) * Arg(1, i.y) * Arg(2, i.z) * Arg(3, i.t));
					}
				}
			}
		}
		return Determinant;
	}

	template <typename T> inline
		T tr(const mat4<T>& Arg) {
		return (Arg(0, 0) + Arg(1, 1) + Arg(2, 2) + Arg(3, 3));
	}

	template <typename T> inline
		mat4<T> transpose(const mat4<T>& arg) {
		mat4<T> temp;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				temp(j, i) = arg(i, j);
			}
		}
		return temp;
	}

#ifdef MATH_ENABLE_IO
	template <typename T> inline
		std::ostream& operator<<(std::ostream& Out, const mat4<T>& Arg) {
		for (int i = 1; i <= 4; i++) {
			Out << "| ";
			for (int j = 1; j <= 4; j++) {
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

#endif // !GEODESUKA_CORE_MATH_MAT4_H
