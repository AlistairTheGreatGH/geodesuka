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
			struct weight {
				math::vec4<uint>				BoneID;
				math::vec4<float>				BoneWeight;
			};
			math::vec3<float> 				Position;
			math::vec3<float> 				Normal;
			math::vec3<float> 				Tangent;
			math::vec3<float> 				Bitangent;
			math::vec3<float> 				TextureCoordinate;
			math::vec4<float> 				Color;
			vertex();
		};

		struct index {
			VkIndexType 					Type;
			std::vector<ushort>				Data16;
			std::vector<uint>				Data32;
		};

		struct bone {
			struct weight {
				uint							ID;
				float							Weight;
			};
			std::string						Name;
			std::vector<weight>				Vertex;
			math::mat4<float> 				Transform;
			math::mat4<float>				Offset;
		};

		struct instance {
			// Host Memory Objects
			int 							Index;
			math::mat4<float> 				Transform;
			std::vector<vertex::weight> 	Vertex;
			std::vector<bone>				Bone;

			// Device Memory Objects
			gcl::context* 					Context;
			gcl::buffer 					VertexWeightBuffer;
			gcl::buffer 					BoneBuffer;

			instance();
			instance(int aMeshIndex, uint aVertexCount, const std::vector<bone>& aBoneData);

			void update(double DeltaTime);
		};

		// Metadata.
		std::string 					Name;
		gcl::context* 					Context;
		uint 							MaterialIndex;
		float 							BoundingRadius;

		// Host Memory Objects
		std::vector<vertex> 			Vertex;
		index 							Index;

		// Device Memory Objects
		gcl::buffer 					VertexBuffer;
		gcl::buffer 					IndexBuffer;

		mesh();
		mesh(gcl::context* aContext, const std::vector<vertex>& aVertexData, const index& aIndexData);
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
