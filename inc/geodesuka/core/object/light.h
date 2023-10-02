#pragma once
#ifndef GEODESUKA_CORE_OBJECT_LIGHT_H
#define GEODESUKA_CORE_OBJECT_LIGHT_H

#include "../object_t.h"

namespace geodesuka::core::object {

	class light : public object_t {
	public:

		enum type {
			AMBIENT,
			DIRECTIONAL,
			SPOT,
			POINT,
		};

		struct color {
			math::vec3<float> Ambient;
			math::vec3<float> Diffuse;
			math::vec3<float> Specular;
		};

		math::vec3<float> Direction;
		color Color;
		float a0, a1, a2;

	};

}

#endif // !GEODESUKA_CORE_OBJECT_LIGHT_H
