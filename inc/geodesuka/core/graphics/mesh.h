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
			math::vec3<float> 			Position;
			math::vec3<float> 			Normal;
			math::vec3<float> 			Tangent;
			math::vec3<float> 			Bitangent;
			math::vec4<uint> 			BoneID;
			math::vec4<float> 			BoneWeight;
			math::vec3<float> 			TextureCoordinate;
			math::vec4<float> 			Color;
			vertex();
		};

		struct index {
			VkIndexType 				Type;
			std::vector<ushort>			Data16;
			std::vector<uint>			Data32;
		};

		struct bone {
			struct vertex_weight {
				uint						ID;
				float						Weight;
			};
			std::string					Name;
			std::vector<vertex_weight>	Vertex;
			math::mat4<float>			Offset;
		};

		// Metadata.
		std::string 				Name;
		gcl::context* 				Context;
		uint 						MaterialIndex;
		float 						BoundingRadius;
		// Host Memory Objects
		std::vector<vertex> 		Vertex;
		index 						Index;
		std::vector<bone> 			Bone;
		// Device Memory Objects
		gcl::buffer 				VertexBuffer;
		gcl::buffer 				IndexBuffer;
		gcl::buffer 				BoneBuffer;

		mesh();
		mesh(gcl::context* aContext, const std::vector<vertex>& aVertexData, const index& aIndexData, const std::vector<bone>& aBoneData);
		~mesh();

		vertex operator[](size_t aIndex) const;
		vertex& operator[](size_t aIndex);

	private:

		void clear();
		void zero_out();
		void generate_device_representation(gcl::context* aContext);

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MESH_H
