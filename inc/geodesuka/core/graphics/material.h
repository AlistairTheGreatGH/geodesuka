#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MATERIAL_H
#define GEODESUKA_CORE_GRAPHICS_MATERIAL_H

#include <string>
#include <vector>

#include "../gcl/image.h"
#include "../gcl/shader.h"

namespace geodesuka::core::graphics {

	// A material is a description of the surface
	// of a mesh. Hence a Surface Material.
	class material {
	public:

		std::string Name;

		

		material();
		~material();

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MATERIAL_H
