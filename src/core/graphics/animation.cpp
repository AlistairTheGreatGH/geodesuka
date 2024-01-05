#include <geodesuka/core/graphics/animation.h>

namespace geodesuka::core::graphics {


    animation::animation() {
        
    }

    math::mat4<float> animation::operator[](double aT) const {
        math::mat4<float> Transform = math::mat4<float>(
            1.0f, 0.0, 0.0f, 0.0f,
            0.0f, 1.0, 0.0f, 0.0f,
            0.0f, 0.0, 1.0f, 0.0f,
            0.0f, 0.0, 0.0f, 1.0f
        );

        // Use interpolation techniques to calculate the current transform.

        return Transform;
    }
    
}
