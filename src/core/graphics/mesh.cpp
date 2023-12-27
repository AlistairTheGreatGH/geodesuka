#include <geodesuka/core/physics/mesh.h>
#include <geodesuka/core/graphics/mesh.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace geodesuka::core::physics {

	// mesh::vertex::vertex() {
	// 	this->Position					= math::vec3<float>(0.0f, 0.0f, 0.0f);
	// 	this->Normal					= math::vec3<float>(0.0f, 0.0f, 0.0f);
	// 	this->Tangent					= math::vec3<float>(0.0f, 0.0f, 0.0f);
	// 	this->Bitangent					= math::vec3<float>(0.0f, 0.0f, 0.0f);
	// 	this->BoneID					= math::vec4<uint>(0u, 0u, 0u, 0u);
	// 	this->BoneWeight				= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);
	// 	this->TextureCoordinate			= math::vec2<float>(0.0f, 0.0f);
	// 	this->Color						= math::vec4<float>(0.0f, 0.0f, 0.0f, 0.0f);
	// }

	// //void mesh::generate(primitive aPrimitive) {
	// //	switch (aPrimitive) {
	// //	case TRIANGLE:
	// //	{
	// //		// vertex VertexData[3];
	// //		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
	// //		// VertexData[0].Position = math::vec3<float>(0.433f, -0.25f, 0.0f);
	// //		// VertexData[1].Position = math::vec3<float>(-0.433f, -0.25f, 0.0f);
	// //		// VertexData[2].Position = math::vec3<float>(0.0f, 0.5f, 0.0f);
	// //		// for (size_t i = 0; i < this->VertexCount; i++) {
	// //		// 	VertexData[i].Normal = math::vec3<float>(0.0f, 0.0f, 1.0f);
	// //		// }

	// //		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
	// //		// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
	// //		// assert(this->Vertex);
	// //		// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));
	// //	}
	// //		break;
	// //	case QUAD:
	// //	{
	// //		// vertex VertexData[4];
	// //		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);
	// //		// VertexData[0].Position = math::vec3<float>(-0.5f, -0.5f, 0.0f); // Bottom Left
	// //		// VertexData[1].Position = math::vec3<float>(0.5f, -0.5f, 0.0f); // Bottom Right
	// //		// VertexData[2].Position = math::vec3<float>(0.5f, 0.5f, 0.0f); // Top Right
	// //		// VertexData[3].Position = math::vec3<float>(-0.5f, 0.5f, 0.0f); // Top Left
	// //		// for (size_t i = 0; i < this->VertexCount; i++) {
	// //		// 	VertexData[i].Normal = math::vec3<float>(0.0f, 0.0f, 1.0f);
	// //		// }

	// //		// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
	// //		// assert(this->Vertex);
	// //		// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));

	// //		// Setup indices for two triangle rendering

	// //	}
	// //		break;
	// //	case GIZMO:
	// //	{
	// //		// vertex VertexData[4];
	// //		// this->VertexCount = sizeof(VertexData) / sizeof(vertex);

	// //		// // Gizmo Positions.
	// //		// VertexData[0].Position = math::vec3<float>(0.0f, 0.0f, 0.0f); // Origin
	// //		// VertexData[1].Position = math::vec3<float>(1.0f, 0.0f, 0.0f); // +X Axis
	// //		// VertexData[2].Position = math::vec3<float>(0.0f, 1.0f, 0.0f); // +Y Axis
	// //		// VertexData[3].Position = math::vec3<float>(0.0f, 0.0f, 1.0f); // +Z Axiz

	// //		// // Gizmo Coloring.
	// //		// VertexData[0].Color = math::vec4<float>(1.0f, 1.0f, 1.0f, 1.0f); // White
	// //		// VertexData[1].Color = math::vec4<float>(1.0f, 0.0f, 0.0f, 1.0f); // Red
	// //		// VertexData[2].Color = math::vec4<float>(0.0f, 1.0f, 0.0f, 1.0f); // Green
	// //		// VertexData[3].Color = math::vec4<float>(0.0f, 0.0f, 1.0f, 1.0f); // Blue

	// //		// this->Vertex = (vertex*)malloc(this->VertexCount * sizeof(vertex));
	// //		// assert(this->Vertex);
	// //		// memcpy(this->Vertex, VertexData, this->VertexCount * sizeof(vertex));

	// //		// this->FaceCount = 1;
	// //		// this->Face = (face*)malloc(this->FaceCount * sizeof(face));
	// //		// assert(this->Face);

	// //		// // {0,1}, {0,2}, {0,3}
	// //		// short IndexData[6] = {
	// //		// 	0, 1,
	// //		// 	0, 2,
	// //		// 	0, 3
	// //		// };

	// //		// Face[0].Topology	= VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	// //		// Face[0].IndexCount	= sizeof(IndexData) / sizeof(short);
	// //		// Face[0].IndexType	= gcl::type::id::SHORT;
	// //		// Face[0].IndexData	= malloc(Face[0].IndexCount * sizeof(short));
	// //		// assert(Face[0].IndexData);
	// //		// memcpy(Face[0].IndexData, IndexData, Face[0].IndexCount * sizeof(short));

	// //	}
	// //		break;
	// //	case CUBE:
	// //	{
	// //		vertex VertexData[8];




	// //	}
	// //		break;
	// //	case TETRAHEDRON:
	// //		break;
	// //	case PYRAMID:
	// //		break;
	// //	default:
	// //		break;
	// //	}
	// //}

	// mesh::mesh() {

	// }

	// mesh::mesh(size_t aVertexCount, vertex* aVertex, size_t aIndexCount, ushort* aIndex) {
	// 	this->Vertex.resize(aVertexCount);
	// 	memcpy(this->Vertex.data(), aVertex, aVertexCount * sizeof(vertex));
	// 	this->Face.resize(1);
	// 	this->Face[0].IndexType = VK_INDEX_TYPE_UINT16;
	// 	this->Face[0].H16.resize(aIndexCount);
	// 	memcpy(this->Face[0].H16.data(), aIndex, aIndexCount * sizeof(ushort));
	// }

	// mesh::mesh(size_t aVertexCount, vertex* aVertex, size_t aIndexCount, uint* aIndex) {
	// 	this->Vertex.resize(aVertexCount);
	// 	memcpy(this->Vertex.data(), aVertex, aVertexCount * sizeof(vertex));
	// 	this->Face.resize(1);
	// 	this->Face[0].IndexType = VK_INDEX_TYPE_UINT32;
	// 	this->Face[0].H32.resize(aIndexCount);
	// 	memcpy(this->Face[0].H32.data(), aIndex, aIndexCount * sizeof(uint));
	// }

	// mesh::vertex mesh::operator[](size_t aIndex) const {
	// 	return this->Vertex[aIndex];
	// }

	// mesh::vertex& mesh::operator[](size_t aIndex) {
	// 	return this->Vertex[aIndex];
	// }

}

namespace geodesuka::core::graphics {


}
