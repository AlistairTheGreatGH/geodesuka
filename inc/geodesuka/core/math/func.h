#pragma once
#ifndef GEODESUKA_CORE_MATH_FUNC_H
#define GEODESUKA_CORE_MATH_FUNC_H

// Numbers
#include "type.h"
#include "constants.h"
#include "complex.h"
#include "quaternion.h"

// Vector
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

// Matrix
#include "mat2.h"
#include "mat2x3.h"
#include "mat2x4.h"
#include "mat3x2.h"
#include "mat3.h"
#include "mat3x4.h"
#include "mat4x2.h"
#include "mat4x3.h"
#include "mat4.h"
#include "matrix.h"
#include "multiply.h"

// Field
#include "field.h"
#include "field1d.h"
#include "field2d.h"
#include "field3d.h"
#include "field4d.h"

// ---- Geodesuka Core Math Function Library -----
// These are assistance functions for the math library.
//

namespace geodesuka::core::math {

    template <typename T>
    inline mat4<T> perspective(T FOV, T AspectRatio, T Near, T Far) {
        //tex:
        // Aspect Ratio: $$a$$
        // Field of View (Radians): $$\theta$$
        // Near Point: $$n$$
        // Far Point: $$f$$
        // $$ x_{n} = \frac{1}{\tan{\frac{\theta}{2}}} \frac{x_{e}}{z_{e}}$$
        // $$ y_{n} = \frac{a}{\tan{\frac{\theta}{2}}} \frac{y_{e}}{z_{e}}$$
        // $$ z_{n} = \frac{1}{z_{e}} \bigg(-\frac{f+n}{f-n} z_{e} + \frac{2fn}{f-n} \bigg)$$ 
        // The $z$ term is why the perspective matrix must be a mat4<float> type 
        // and not just a float3x3. The set of equations above describe
        // the transform from what the perspective of the camera
        // to the screen space of the context.
        // 
        // The matrix then takes the form of 
        // $$ P =
        // \begin{bmatrix}
        // \frac{1}{\tan{\frac{\theta}{2}}} & 0 & 0 & 0 \\
    	// 0 & \frac{a}{\tan{\frac{\theta}{2}}} & 0 & 0 \\
    	// 0 & 0 & - \frac{f + n}{f - n} & \frac{2fn}{f - n} \\
    	// 0 & 0 & 1 & 0 \\
    	// \end{bmatrix}
        // $$

        T tn = std::tan(FOV / 2.0);
        return mat4<T>(
            (1.0 / tn),     0.0,                    0.0,                                    0.0,
            0.0,            (AspectRatio / tn),     0.0,                                    0.0,
            0.0,            0.0,                    (-((Far + Near) / (Far - Near))),       ((2.0 * Far * Near) / ((double)Far - (double)Near)),
            0.0,            0.0,                    1.0,                                    0.0
        );
    }

	// Uses a quaternion intended for rotation and casts it into a rotation matrix.
	template <typename T>
	inline mat4<T> rotation(quaternion<T> aQuaternion) {
		//tex:
		// In quaternion notation, a rotation is of the form
		// $$ \vec{r}^{'} = q\vec{r}q^{-1} $$
		// Where 
		// $ q = e^{\phi} $
		// and $\phi$ is
		// $$ \phi = \frac{\theta}{2} \hat{u} $$
		// $\theta$ is the angle of rotation, and $\hat{u}$ is the vector
		// which the object is rotated around.
		// $$ s = \frac{1}{|q|^{2}} $$
		// The matrix below is to be used in the following way $\vec{r}^{'} = R \vec{r}$
		// and is equivalent to $ \vec{r}^{'} = q \vec{r} q^{-1} $.
		// $$ R = 
		// \begin{bmatrix}
		// 1 - s(c^{2} + d^{2}) & 2s(bc - da) & 2s(bd + ca) \\ 
		// 2s(bc + da) & 1 - 2s(b^{2} + d^{2}) & 2s(cd - ba) \\
    	// 2s(bd - ca) & 2s(cd + ba) & 1 - 2s(b^{2} + c^{2})
		// \end{bmatrix}    
		// $$
		// Citation: http://www.faqs.org/faqs/graphics/algorithms-faq/

		quaternion<T> q = aQuaternion;
		T s = 1.0 / abs2(q);
		return mat4<T>(
			1.0 - 2.0 * s * (q.c * q.c + q.d * q.d), 2.0 * s * (q.b * q.c - q.d * q.a), 2.0 * s * (q.b * q.d + q.c * q.a), 0.0,
			2.0 * s * (q.b * q.c + q.d * q.a), 1.0 - 2.0 * s * (q.b * q.b + q.d * q.d), 2.0 * s * (q.c * q.d - q.b * q.a), 0.0,
			2.0 * s * (q.b * q.d - q.c * q.a), 2.0 * s * (q.c * q.d + q.b * q.a), 1.0 - 2.0 * s * (q.b * q.b + q.c * q.c), 0.0,
			0.0, 0.0, 0.0, 1.0
		);
	}

    template <typename T>
	inline mat4<T> rotation(T aAngle, vec3<T> aAxis) {
        vec3<T> V 	        = normalize(aAxis);
		T Phi 		        = std::exp(aAngle / 2.0);
		quaternion<T> q     = Phi * quaternion<T>(0.0, V.x, V.y, V.z);
		return rotation(q);
	}

}

#endif // !GEODESUKA_CORE_MATH_FUNC_H