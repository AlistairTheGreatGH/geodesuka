#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MESH_H
#define GEODESUKA_CORE_GRAPHICS_MESH_H

#include "../physics/mesh.h"
#include "../gcl/context.h"
#include "../gcl/buffer.h"

namespace geodesuka::core::graphics {

	class mesh : public physics::mesh {
	public:

		enum primitive {
			TRIANGLE	,
			QUAD		,
			GIZMO		,
			TETRAHEDRON	,
			PYRAMID		, 
			CUBE		,
		};

		struct vertex {
			math::vec3<float> 		Position;
			math::vec3<float> 		Normal;
			math::vec3<float> 		Tangent;
			math::vec3<float> 		Bitangent;
			math::vec4<uint> 		BoneID;
			math::vec4<float> 		BoneWeight;
			math::vec3<float> 		TextureCoordinate[8];
			math::vec4<float> 		Color[8];
			vertex();
		};

		struct uv {
			// math::vec2<float> 		TextureCoordinate;
			// math::vec4<float> 		Color;
		};

		struct face {
			VkIndexType 				IndexType;
			std::vector<ushort>			H16;
			std::vector<uint>			H32;
		};

		struct bone {
			struct vertex_weight {
				uint						ID;
				float						Weight;
			};
			util::string				Name;
			std::vector<vertex_weight>	Vertex;
		};

		// Host Memory Data.
		float 						BoundingRadius;
		std::vector<vertex> 		Vertex;
		std::vector<face> 			Face;
		std::vector<bone> 			Bone;
		gcl::buffer 				VertexBuffer;
		std::vector<gcl::buffer> 	IndexBuffer;
		gcl::buffer 				BoneBuffer;

		mesh();
		mesh(const std::vector<vertex>& aVertexData, const std::vector<ushort>& aIndexData);
		mesh(const std::vector<vertex>& aVertexData, const std::vector<uint>& aIndexData);
		~mesh();

		vertex operator[](size_t aIndex) const;
		vertex& operator[](size_t aIndex);

		//
		void recenter();

	private:

		void clear();
		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MESH_H
