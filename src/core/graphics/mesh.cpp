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

	//void mesh::generate(primitive aPrimitive) {
	//	switch (aPrimitive) {
	//	case TRIANGLE:
	//	{
	//		// vertex VertexData[3];
	//		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
	//		// VertexData[0].Position = math::vec3<float>(0.433f, -0.25f, 0.0f);
	//		// VertexData[1].Position = math::vec3<float>(-0.433f, -0.25f, 0.0f);
	//		// VertexData[2].Position = math::vec3<float>(0.0f, 0.5f, 0.0f);
	//		// for (size_t i = 0; i < this->VertexCount; i++) {
	//		// 	VertexData[i].Normal = math::vec3<float>(0.0f, 0.0f, 1.0f);
	//		// }

	//		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
	//		// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
	//		// assert(this->Vertex);
	//		// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));
	//	}
	//		break;
	//	case QUAD:
	//	{
	//		// vertex VertexData[4];
	//		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
	//		// VertexData[0].Position = math::vec3<float>(-0.5f, -0.5f, 0.0f); // Bottom Left
	//		// VertexData[1].Position = math::vec3<float>(0.5f, -0.5f, 0.0f); // Bottom Right
	//		// VertexData[2].Position = math::vec3<float>(0.5f, 0.5f, 0.0f); // Top Right
	//		// VertexData[3].Position = math::vec3<float>(-0.5f, 0.5f, 0.0f); // Top Left
	//		// for (size_t i = 0; i < this->VertexCount; i++) {
	//		// 	VertexData[i].Normal = math::vec3<float>(0.0f, 0.0f, 1.0f);
	//		// }

	//		// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
	//		// assert(this->Vertex);
	//		// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));

	//		// Setup indices for two triangle rendering

	//	}
	//		break;
	//	case GIZMO:
	//	{
	//		// vertex VertexData[4];
	//		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);

	//		// // Gizmo Positions.
	//		// VertexData[0].Position = math::vec3<float>(0.0f, 0.0f, 0.0f); // Origin
	//		// VertexData[1].Position = math::vec3<float>(1.0f, 0.0f, 0.0f); // +X Axis
	//		// VertexData[2].Position = math::vec3<float>(0.0f, 1.0f, 0.0f); // +Y Axis
	//		// VertexData[3].Position = math::vec3<float>(0.0f, 0.0f, 1.0f); // +Z Axiz

	//		// // Gizmo Coloring.
	//		// VertexData[0].Color = math::vec4<float>(1.0f, 1.0f, 1.0f, 1.0f); // White
	//		// VertexData[1].Color = math::vec4<float>(1.0f, 0.0f, 0.0f, 1.0f); // Red
	//		// VertexData[2].Color = math::vec4<float>(0.0f, 1.0f, 0.0f, 1.0f); // Green
	//		// VertexData[3].Color = math::vec4<float>(0.0f, 0.0f, 1.0f, 1.0f); // Blue

	//		// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
	//		// assert(this->Vertex);
	//		// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));

	//		// this->FaceCount = 1;
	//		// this->Face = (face*)malloc(this->FaceCount * sizeof(face));
	//		// assert(this->Face);

	//		// // {0,1}, {0,2}, {0,3}
	//		// short IndexData[6] = {
	//		// 	0, 1,
	//		// 	0, 2,
	//		// 	0, 3
	//		// };

	//		// Face[0].Topology	= VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	//		// Face[0].IndexCount	= sizeof(IndexData) / sizeof(short);
	//		// Face[0].IndexType	= gcl::type::id::SHORT;
	//		// Face[0].IndexData	= malloc(Face[0].IndexCount * sizeof(short));
	//		// assert(Face[0].IndexData);
	//		// memcpy(Face[0].IndexData, IndexData, Face[0].IndexCount * sizeof(short));

	//	}
	//		break;
	//	case CUBE:
	//	{
	//		vertex VertexData[8];




	//	}
	//		break;
	//	case TETRAHEDRON:
	//		break;
	//	case PYRAMID:
	//		break;
	//	default:
	//		break;
	//	}
	//}

	mesh::mesh() {

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
