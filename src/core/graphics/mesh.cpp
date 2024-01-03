#include <geodesuka/core/physics/mesh.h>
#include <geodesuka/core/graphics/mesh.h>

namespace geodesuka::core::physics {

}

namespace geodesuka::core::graphics {

	using namespace gcl;

	mesh::vertex::vertex() {
		this->Position					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Normal					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Tangent					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Bitangent					= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->BoneID					= math::vec4<uint>(0u, 0u, 0u, 0u);
		this->BoneWeight				= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);
		this->TextureCoordinate			= math::vec3<float>(0.0f, 0.0f, 0.0f);
		this->Color						= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);
	}

	mesh::mesh() {
		this->BoundingRadius = 0.0f;
	}

	mesh::mesh(gcl::context* aContext, const std::vector<vertex>& aVertexData, const index& aIndexData, const std::vector<bone>& aBoneData) : mesh() {
		this->Vertex 	= aVertexData;
		this->Index 	= aIndexData;
		this->Bone 		= aBoneData;
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
			gcl::buffer::create_info BBCI(
				device::memory::HOST_VISIBLE | device::memory::DEVICE_LOCAL,
				buffer::usage::INDEX | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST
			);
			VertexBuffer = buffer(Context, VBCI, Vertex.size() * sizeof(vertex), (void*)Vertex.data());
			// wtf this is legal?
			// IndexBuffer = std::vector<gcl::buffer>(1, buffer(Context, IBCI, aIndexData.size() * sizeof(ushort), (void*)aIndexData.data()));
			// IndexBuffer = std::vector<buffer>(Face.size());
			// for (size_t i = 0; i < Face.size(); i++) {
			// 	switch(Face[i].IndexType) {
			// 	case VK_INDEX_TYPE_UINT16:
			// 		IndexBuffer[i] = buffer(Context, IBCI, Face[i].H16.size() * sizeof(ushort), (void*)Face[i].H16.data());
			// 		break;
			// 	case VK_INDEX_TYPE_UINT32:
			// 	IndexBuffer[i] = buffer(Context, IBCI, Face[i].H32.size() * sizeof(uint), (void*)Face[i].H32.data());
			// 		break;
			// 	}
			// }
			// TODO: Generate Bone Buffer Data. (Must be a linearized array!)
			BoneBuffer = buffer(Context, BBCI, Bone.size() * sizeof(bone), (void*)Bone.data());
		}
	}

}
