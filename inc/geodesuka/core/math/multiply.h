#pragma once
#ifndef GEODESUKA_CORE_MATH_MATRIX_MULTIPLY_H
#define GEODESUKA_CORE_MATH_MATRIX_MULTIPLY_H

#include "mat2.h"
#include "mat2x3.h"
#include "mat2x4.h"
#include "mat3x2.h"
#include "mat3.h"
#include "mat3x4.h"
#include "mat4x2.h"
#include "mat4x3.h"
#include "mat4.h"

//tex:
// Matrix Multiplication
// $$C = AB$$
// $$ c_{ij} = \sum_{k} a_{ik} b_{kj} $$

namespace geodesuka::core::math {

	// //mat2<T> operator*(mat2<T> aLhs, mat2<T> aRhs);
	// mat2x3<T> operator*(mat2<T> aLhs, mat2x3<T> aRhs);
	// mat2x4<T> operator*(mat2<T> aLhs, mat2x4<T> aRhs);

	// mat2<T> operator*(mat2x3<T> aLhs, mat3x2<T> aRhs);
	// mat2x3<T> operator*(mat2x3<T> aLhs, mat3<T> aRhs);
	// mat2x4<T> operator*(mat2x3<T> aLhs, mat3x4<T> aRhs);

	// mat2<T> operator*(mat2x4<T> aLhs, mat4x2<T> aRhs);
	// mat2x3<T> operator*(mat2x4<T> aLhs, mat4x3<T> aRhs);
	// mat2x4<T> operator*(mat2x4<T> aLhs, mat4<T> aRhs);

	// mat3x2<T> operator*(mat3x2<T> aLhs, mat2<T> aRhs);
	// mat3<T> operator*(mat3x2<T> aLhs, mat2x3<T> aRhs);
	// mat3x4<T> operator*(mat3x2<T> aLhs, mat2x4<T> aRhs);

	// mat3x2<T> operator*(mat3<T> aLhs, mat3x2<T> aRhs);
	// //mat3<T> operator*(mat3<T> aLhs, mat3<T> aRhs);
	// mat3x4<T> operator*(mat3<T> aLhs, mat3x4<T> aRhs);

	// mat3x2<T> operator*(mat3x4<T> aLhs, mat4x2<T> aRhs);
	// mat3<T> operator*(mat3x4<T> aLhs, mat4x3<T> aRhs);
	// mat3x4<T> operator*(mat3x4<T> aLhs, mat4<T> aRhs);

	// mat4x2<T> operator*(mat4x2<T> aLhs, mat2<T> aRhs);
	// mat4x3<T> operator*(mat4x2<T> aLhs, mat2x3<T> aRhs);
	// mat4<T> operator*(mat4x2<T> aLhs, mat2x4<T> aRhs);

	// mat4x2<T> operator*(mat4x3<T> aLhs, mat3x2<T> aRhs);
	// mat4x3<T> operator*(mat4x3<T> aLhs, mat3<T> aRhs);
	// mat4<T> operator*(mat4x3<T> aLhs, mat3x4<T> aRhs);

	// mat4x2<T> operator*(mat4<T> aLhs, mat4x2<T> aRhs);
	// mat4x3<T> operator*(mat4<T> aLhs, mat4x3<T> aRhs);
	// //mat4<T> operator*(mat4<T> aLhs, mat4<T> aRhs);

	template <typename T> inline
		mat2x3<T> operator*(mat2<T> aLhs, mat2x3<T> aRhs) {
		mat2x3<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2x4<T> operator*(mat2<T> aLhs, mat2x4<T> aRhs) {
		mat2x4<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2<T> operator*(mat2x3<T> aLhs, mat3x2<T> aRhs) {
		mat2<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2x3<T> operator*(mat2x3<T> aLhs, mat3<T> aRhs) {
		mat2x3<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2x4<T> operator*(mat2x3<T> aLhs, mat3x4<T> aRhs) {
		mat2x4<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2<T> operator*(mat2x4<T> aLhs, mat4x2<T> aRhs) {
		mat2<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2x3<T> operator*(mat2x4<T> aLhs, mat4x3<T> aRhs) {
		mat2x3<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat2x4<T> operator*(mat2x4<T> aLhs, mat4<T> aRhs) {
		mat2x4<T> Temp;
		for (int i = 1; i <= 2; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3x2<T> operator*(mat3x2<T> aLhs, mat2<T> aRhs) {
		mat3x2<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3<T> operator*(mat3x2<T> aLhs, mat2x3<T> aRhs) {
		mat3<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3x4<T> operator*(mat3x2<T> aLhs, mat2x4<T> aRhs) {
		mat3x4<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3x2<T> operator*(mat3<T> aLhs, mat3x2<T> aRhs) {
		mat3x2<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3x4<T> operator*(mat3<T> aLhs, mat3x4<T> aRhs) {
		mat3x4<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3x2<T> operator*(mat3x4<T> aLhs, mat4x2<T> aRhs) {
		mat3x2<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3<T> operator*(mat3x4<T> aLhs, mat4x3<T> aRhs) {
		mat3<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat3x4<T> operator*(mat3x4<T> aLhs, mat4<T> aRhs) {
		mat3x4<T> Temp;
		for (int i = 1; i <= 3; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4x2<T> operator*(mat4x2<T> aLhs, mat2<T> aRhs) {
		mat4x2<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4x3<T> operator*(mat4x2<T> aLhs, mat2x3<T> aRhs) {
		mat4x3<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4<T> operator*(mat4x2<T> aLhs, mat2x4<T> aRhs) {
		mat4<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 2; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4x2<T> operator*(mat4x3<T> aLhs, mat3x2<T> aRhs) {
		mat4x2<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4x3<T> operator*(mat4x3<T> aLhs, mat3<T> aRhs) {
		mat4x3<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4<T> operator*(mat4x3<T> aLhs, mat3x4<T> aRhs) {
		mat4<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 4; j++) {
				for (int k = 1; k <= 3; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4x2<T> operator*(mat4<T> aLhs, mat4x2<T> aRhs) {
		mat4x2<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 2; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

	template <typename T> inline
		mat4x3<T> operator*(mat4<T> aLhs, mat4x3<T> aRhs) {
		mat4x3<T> Temp;
		for (int i = 1; i <= 4; i++) {
			for (int j = 1; j <= 3; j++) {
				for (int k = 1; k <= 4; k++) {
					Temp(i, j) += aLhs(i, k) * aRhs(k, j);
				}
			}
		}
		return Temp;
	}

}

#endif // !GEODESUKA_CORE_MATH_MATRIX_MULTIPLY_H
