#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MESH_H
#define GEODESUKA_CORE_GRAPHICS_MESH_H

#include "../physics/mesh.h"
#include "../gcl/context.h"
#include "../gcl/buffer.h"

namespace geodesuka::core::graphics {

	class mesh : public physics::mesh {
	public:

		gcl::context* 				Context;
		gcl::buffer 				VertexBuffer;
		std::vector<gcl::buffer> 	IndexBuffer;
		gcl::buffer 				BoneBuffer;

		//mesh(gcl::context* aContext, size_t aVertexCount, vertex* aVertex, size_t aIndexCount, ushort* aIndex);
		//mesh(gcl::context* aContext, size_t aVertexCount, vertex* aVertex, size_t aIndexCount, uint* aIndex);

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MESH_H
