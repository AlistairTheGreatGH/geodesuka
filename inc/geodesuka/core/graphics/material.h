#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MATERIAL_H
#define GEODESUKA_CORE_GRAPHICS_MATERIAL_H

#include <vector>

#include "../gcl/image.h"
#include "../gcl/shader.h"

namespace geodesuka::core::graphics {

	class material {
	public:

		std::vector<gcl::image*> Texture;
		gcl::shader* VertexShader;
		gcl::shader* PixelShader;

		material();
		~material();

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MATERIAL_H
