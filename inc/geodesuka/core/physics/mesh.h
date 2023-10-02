#pragma once
#ifndef GEODESUKA_CORE_PHYSICS_MESH_H
#define GEODESUKA_CORE_PHYSICS_MESH_H

#include <vector>

#include "../math.h"
#include "../util/string.h"
#include "../gcl/config.h"
#include "../gcl/variable.h"

namespace geodesuka::core::physics {

    class mesh {
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
			math::vec3<float> 	Position;
			math::vec3<float> 	Normal;
			math::vec3<float> 	Tangent;
			math::vec3<float> 	Bitangent;
			// Mesh Texturing & Coloring.
			math::vec2<float> 	TextureCoordinate; // Texture data should be associated with index data, not vertex data...
			math::vec4<float> 	Color;
			// Mesh Animation.
			math::vec4<uint> 	BoneID;
			math::vec4<float> 	BoneWeight;
			vertex();
		};

		struct face {
			VkIndexType 			IndexType;
			std::vector<ushort>		H16;
			std::vector<uint>		H32;
		};

		struct bone {
			struct vertex_weight {
				uint					ID;
				float					Weight;
			};
			util::string			Name;
			uint					VertexCount;
			vertex_weight*			Vertex;
		};

		float 					BoundingRadius;
		std::vector<vertex> 	Vertex;
		std::vector<face> 		Face;
		std::vector<bone> 		Bone;

		mesh();
		mesh(size_t aVertexCount, vertex* aVertex, size_t aIndexCount, ushort* aIndex);
		mesh(size_t aVertexCount, vertex* aVertex, size_t aIndexCount, uint* aIndex);

		vertex operator[](size_t aIndex) const;
		vertex& operator[](size_t aIndex);

		//
		void recenter();

	};

}

#endif // !GEODESUKA_CORE_PHYSICS_MESH_H