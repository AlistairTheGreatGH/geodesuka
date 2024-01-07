#include <geodesuka/core/physics/mesh.h>
#include <geodesuka/core/graphics/mesh.h>

#include <vector>
#include <algorithm>

namespace geodesuka::core::physics {

}

namespace geodesuka::core::graphics {

	using namespace gcl;

	mesh::vertex::vertex() {
		this->Position					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Normal					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Tangent					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Bitangent					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		// this->BoneID					= math::vec4<uint>(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
		// this->BoneWeight				= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);
		this->TextureCoordinate			= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Color						= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);
	}

	mesh::instance::instance() {
		this->Index 	= -1;
		this->Context 	= nullptr;
	}

	mesh::instance::instance(int aMeshIndex, uint aVertexCount, const std::vector<bone>& aBoneData) : instance() {
		this->Index 		= aMeshIndex;
		this->Vertex 		= std::vector<vertex::weight>(aVertexCount);
		this->Bone 			= aBoneData;
		// Generate the corresponding vertex buffer which will supply the mesh
		// instance the needed bone animation data.
		for (size_t i = 0; i < Vertex.size(); i++) {
			Vertex[i].BoneID 		= math::vec4<uint>(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX);
			Vertex[i].BoneWeight 	= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);

			// Group Bone Weights By Vertex Index.
			std::vector<bone::weight> VertexBoneWeight;
			for (size_t j = 0; j < Bone.size(); j++) {
				for (size_t k = 0; k < Bone[j].Vertex.size(); k++) {
					// If the vertex index matches the bone vertex index, then then copy over.
					if (i == Bone[j].Vertex[k].ID) {
						// Uses insert sort to keep the weights sorted from largest to smallest.
						// {  0,  1   2,  3, 4 }
						// { 69, 25, 21, 10, 4 } <- 15
						// Insert at index 3
						// { 69, 25, 21, 15, 10, 4 }
						for (size_t a = 0; a < VertexBoneWeight.size(); a++) {
							if (VertexBoneWeight[a].Weight < Bone[j].Vertex[k].Weight) {
								VertexBoneWeight.insert(VertexBoneWeight.begin() + a, Bone[j].Vertex[k]);
								break;
							}
						}
					}
				}
			}

			// Will take the first and largest elements.
			for (size_t j = 0; j < std::min((size_t)4, VertexBoneWeight.size()); j++) {
				Vertex[i].BoneID[j] 		= VertexBoneWeight[j].ID;
				Vertex[i].BoneWeight[j] 	= VertexBoneWeight[j].Weight;
			}
		}
	}

	void mesh::instance::update(double DeltaTime) {
		// The goal here is to update the Bone Buffer in the vertex shader.
		std::vector<math::mat4<float>> TransformData(1 + 2*Bone.size());

		// This is the Mesh Instance Transform. This transform is applied to mesh space vertices
		// directly is no bone structure is altering the vertices. It takes the mesh space vertices
		// and transforms them to root model space. This is directly applied to the vertices.
		TransformData[0] = this->Transform;

		// This is the current transform data for each bone modified by the current animations structure.
		for (size_t i = 0; i < Bone.size(); i++) {
			TransformData[i + 1] = Bone[i].Transform;
		}

		// This is the offset matrix data that transforms vertices from mesh space to bone space.
		// Needed to transform vertices from bone space to mesh space for animated bones to animate
		// the mesh.
		for (size_t i = 0; i < Bone.size(); i++) {
			TransformData[i + Bone.size() + 1] = Bone[i].Offset;
		}

		// Send data to the GPU Uniform Buffer.
		this->BoneBuffer.write(0, TransformData.data(), 0, TransformData.size() * sizeof(math::mat4<float>));
	}

	mesh::mesh() {
		this->BoundingRadius = 0.0f;
	}

	mesh::mesh(gcl::context* aContext, const std::vector<vertex>& aVertexData, const index& aIndexData) : mesh() {
		this->Vertex 	= aVertexData;
		this->Index 	= aIndexData;
		this->generate_device_representation(aContext);
	}

	mesh::~mesh() {}

	mesh::vertex mesh::operator[](size_t aIndex) const {
		return this->Vertex[aIndex];
	}

	mesh::vertex& mesh::operator[](size_t aIndex) {
		return this->Vertex[aIndex];
	}

	void mesh::generate_device_representation(gcl::context* aContext) {
		this->Context = aContext;
		if (Context != nullptr) {
			// Vertex Buffer Creation Info
			gcl::buffer::create_info VBCI(
				device::memory::DEVICE_LOCAL,
				buffer::usage::VERTEX | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST
			);
			// Index buffer Create Info
			gcl::buffer::create_info IBCI(
				device::memory::DEVICE_LOCAL,
				buffer::usage::INDEX | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST
			);
			// Index buffer Create Info
			// gcl::buffer::create_info BBCI(
			// 	device::memory::HOST_VISIBLE | device::memory::DEVICE_LOCAL,
			// 	buffer::usage::INDEX | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST
			// );
			VertexBuffer = buffer(Context, VBCI, Vertex.size() * sizeof(vertex), (void*)Vertex.data());
			// wtf this is legal?
			// IndexBuffer = std::vector<gcl::buffer>(1, buffer(Context, IBCI, aIndexData.size() * sizeof(ushort), (void*)aIndexData.data()));
			switch (Index.Type) {
			case VK_INDEX_TYPE_UINT16:
				IndexBuffer = buffer(Context, IBCI, Index.Data16.size() * sizeof(ushort), (void*)Index.Data16.data());
				break;
			case VK_INDEX_TYPE_UINT32:
				IndexBuffer = buffer(Context, IBCI, Index.Data32.size() * sizeof(uint), (void*)Index.Data32.data());
				break;
			default:
				break;
			}
		}
	}

}
