#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_ANIMATION_H
#define GEODESUKA_CORE_GRAPHICS_ANIMATION_H

#include <string>
#include <vector>
#include <map>

#include "../math.h"

// #include "mesh.h"
// #include "model.h"

namespace geodesuka::core::graphics {

	class animation {
	public:

		template <typename T>
		struct key {
			double		Time;
			T			Value;
		};

		struct node_anim {
			std::vector<key<math::vec3<float>>> 			PositionKey;
			std::vector<key<math::quaternion<float>>> 		RotationKey;
			std::vector<key<math::vec3<float>>> 			ScalingKey;
			node_anim();
			math::mat4<float> operator[](double aTime) const;
		};

		struct mesh_anim {

		};

		std::string 						Name;
		float 								Weight;
		float 								Duration;
		float 								TicksPerSecond;
		std::map<std::string, node_anim> 	NodeAnimMap;
		std::map<std::string, mesh_anim> 	MeshAnimMap;

		animation();

		node_anim operator[](std::string aNodeName);

	private:

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_ANIMATION_H
