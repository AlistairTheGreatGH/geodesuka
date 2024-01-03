#include <geodesuka/core/graphics/model.h>

#include <assert.h>

#include <iostream>

// Model Loading
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Assimp::Importer* ModelImporter = nullptr;

namespace geodesuka::core::graphics {

	static void fill_and_traverse(model::node& aMeshNode, aiNode* aAssimpNode) {
		aMeshNode.Child.resize(aAssimpNode->mNumChildren);
		for (size_t i = 0; i < aMeshNode.Child.size(); i++) {
			aMeshNode[i].Root = aMeshNode.Root;
			aMeshNode[i].Parent = &aMeshNode;
			fill_and_traverse(aMeshNode[i], aAssimpNode->mChildren[i]);
		}
		aMeshNode.Name = aAssimpNode->mName.C_Str();
		aMeshNode.MeshIndex.resize(aAssimpNode->mNumMeshes);
		for (size_t i = 0; i < aMeshNode.MeshIndex.size(); i++) {
			aMeshNode.MeshIndex[i] = aAssimpNode->mMeshes[i];
		}
		aAssimpNode->mTransformation.a1;
		aMeshNode.Transformation = math::mat4<float>(
			aAssimpNode->mTransformation.a1, aAssimpNode->mTransformation.a2, aAssimpNode->mTransformation.a3, aAssimpNode->mTransformation.a4,
			aAssimpNode->mTransformation.b1, aAssimpNode->mTransformation.b2, aAssimpNode->mTransformation.b3, aAssimpNode->mTransformation.b4,
			aAssimpNode->mTransformation.c1, aAssimpNode->mTransformation.c2, aAssimpNode->mTransformation.c3, aAssimpNode->mTransformation.c4,
			aAssimpNode->mTransformation.d1, aAssimpNode->mTransformation.d2, aAssimpNode->mTransformation.d3, aAssimpNode->mTransformation.d4
		);
	}

	static void traverse(const aiScene* aScene, aiNode* aNode) {
		static int TreeDepth = -1;
		TreeDepth += 1;
		bool isBone = false;
		for (int i = 0; i < aScene->mNumMeshes; i++) {
			for (int j = 0; j < aScene->mMeshes[i]->mNumBones; j++) {
				util::string A = aScene->mMeshes[i]->mBones[j]->mName.C_Str();
				util::string B = aNode->mName.C_Str();
				if (A == B) {
					isBone = true;
				}
			}
		}
		for (int i = 0; i < TreeDepth; i++) {
			std::cout << "  ";
		}
		std::cout << "Depth: " << TreeDepth << ", isBone: " << isBone << ", Name: " << aNode->mName.C_Str() << std::endl;
		for (int i = 0; i < aNode->mNumMeshes; i++) {
			for (int j = 0; j < TreeDepth; j++) {
				std::cout << " ";
			}
			std::cout << "Mesh Name: " << aScene->mMeshes[aNode->mMeshes[i]]->mName.C_Str() << std::endl;
		}
		for (int i = 0; i < aNode->mNumChildren; i++) {
			traverse(aScene, aNode->mChildren[i]);
		}
		TreeDepth -= 1;
	}

	model::node::node() {
		this->zero_out();
	}

	model::node::node(const node& aInput) : node() {
		this->Root 		= this;
		this->Parent 	= nullptr;
		*this 			= aInput;
	}

	model::node::node(node&& aInput) noexcept : node() {
		*this = aInput;
	}

	model::node::~node() {
		this->clear();
	}

	model::node& model::node::operator=(const node& aRhs) {
		if (this == &aRhs) return *this;
		this->Name					= aRhs.Name;
		this->MeshIndex				= aRhs.MeshIndex;
		this->Transformation		= aRhs.Transformation;
		this->Context				= aRhs.Context;
		this->UniformBuffer			= aRhs.UniformBuffer;
		this->Child.resize(aRhs.Child.size());
		for (size_t i = 0; i < aRhs.Child.size(); i++) {
			this->Child[i].Root 		= this->Root;
			this->Child[i].Parent 		= this;
			this->Child[i] 				= aRhs.Child[i];
		}
		return *this;
	}

	model::node& model::node::operator=(node&& aRhs) noexcept {
		*this = aRhs;
		return *this;
	}

	model::node& model::node::operator[](int aIndex) {
		return this->Child[aIndex];
	}

	int model::node::count() const {
		int TotalCount = 1;
		for (const node& Chd : Child) {
			TotalCount += Chd.count();
		}
		return TotalCount;
	}

	int model::node::mesh_reference_count() const {
		int MeshCount = 0;
		MeshCount += this->MeshIndex.size();
		for (const node& Chd : Child) {
			MeshCount += Chd.mesh_reference_count();
		}
		return MeshCount;
	}

	model::node model::node::linearize() const {
		node Linear;
		Linear.Child.resize(this->count());
		for (node& Chd : Linear.Child) {
			Chd.Root	= &Linear;
			Chd.Parent	= &Linear;
		}
		int LinearOffset = 0;
		Linear.linearize(LinearOffset, *this);
		return Linear;
	}

	math::mat4<float> model::node::global_transform() const {
		// TODO: Do not forget there are animations that alter this.
		if (this->Parent != nullptr) {
			return this->Parent->global_transform() * this->Transformation;
		}
		else {
			return this->Transformation;
		}
	}

	void model::node::linearize(int& aOffset, const node& aNode) {
		this->Child[aOffset].MeshIndex 			= aNode.MeshIndex;
		this->Child[aOffset].Name				= aNode.Name;
		this->Child[aOffset].Transformation		= aNode.global_transform();
		aOffset += 1;
		for (const node& Chd : aNode.Child) {
			this->linearize(aOffset, Chd);
		}
	}

	void model::node::set_root(node* aRoot) {
		this->Root = aRoot;
		for (node& Chd : Child) {
			Chd.set_root(aRoot);
		}
	}

	void model::node::clear() {
		this->Child.clear();
		this->Name = "";
		this->MeshIndex.clear();
		this->UniformBuffer = gcl::buffer();
		this->zero_out();
	}

	void model::node::zero_out() {
		Root			= this;
		Parent			= nullptr;
		Transformation = math::mat4<float>(
			1.0f, 	0.0f, 	0.0f, 	0.0f,
			0.0f,	1.0f, 	0.0f, 	0.0f,
			0.0f,	0.0f, 	1.0f, 	0.0f,
			0.0f,	0.0f, 	0.0f, 	1.0f
		);
		Context 		= nullptr;
	}

	model::model() {

	}

	model::model(const char* aFilePath) {
		if (aFilePath == NULL) return;
		const aiScene *Scene = ModelImporter->ReadFile(aFilePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

		for (int i = 0; i < Scene->mNumMeshes; i++) {
			std::cout << "Mesh Name: " << Scene->mMeshes[i]->mName.C_Str() << std::endl;
		}
		std::cout << "--------------- Node Hierarchy --------------------" << std::endl;
		traverse(Scene, Scene->mRootNode);
		std::cout << "--------------- Mesh & Bone --------------------" << std::endl;
		for (int i = 0; i < Scene->mNumMeshes; i++) {
			std::cout << "Mesh Name: " << Scene->mMeshes[i]->mName.C_Str() << std::endl;
			for (int j = 0; j < Scene->mMeshes[i]->mNumBones; j++) {
				std::cout << "\tBone Name: " << Scene->mMeshes[i]->mBones[j]->mName.C_Str() << std::endl;
			}
		}
		for (const node& Chd : this->LeafList.Child) {
			std::cout << Chd.Name << std::endl;
		}

		// Extract Scene Hiearchy
		fill_and_traverse(this->Hierarchy, Scene->mRootNode);

		// Linearize Node Hierarchy
		this->LeafList = this->Hierarchy.linearize();
		
		this->Mesh			= std::vector<mesh>(Scene->mNumMeshes);
		this->Material		= std::vector<material>(Scene->mNumMaterials);
		this->Animation		= std::vector<animation>(Scene->mNumAnimations);
		this->Texture		= std::vector<gcl::image>(Scene->mNumTextures);

		for (size_t i = 0; i < this->Mesh.size(); i++) {
			// Load Raw Vertex Data (Gross, optimize later)
			std::vector<mesh::vertex> VertexData(Scene->mMeshes[i]->mNumVertices);
			for (size_t j = 0; j < VertexData.size(); j++) {
				if (Scene->mMeshes[i]->HasPositions()) {
					VertexData[j].Position = math::vec3<float>(
						Scene->mMeshes[i]->mVertices[j].x,
						Scene->mMeshes[i]->mVertices[j].y,
						Scene->mMeshes[i]->mVertices[j].z
					);
				}
				if (Scene->mMeshes[i]->HasNormals()) {
					VertexData[j].Normal = math::vec3<float>(
						Scene->mMeshes[i]->mNormals[j].x,
						Scene->mMeshes[i]->mNormals[j].y,
						Scene->mMeshes[i]->mNormals[j].z
					);
				}
				if (Scene->mMeshes[i]->HasTangentsAndBitangents()) {
					VertexData[j].Tangent = math::vec3<float>(
						Scene->mMeshes[i]->mTangents[j].x,
						Scene->mMeshes[i]->mTangents[j].y,
						Scene->mMeshes[i]->mTangents[j].z
					);
					VertexData[j].Bitangent = math::vec3<float>(
						Scene->mMeshes[i]->mBitangents[j].x,
						Scene->mMeshes[i]->mBitangents[j].y,
						Scene->mMeshes[i]->mBitangents[j].z
					);
				}

				// -------------------- Texturing & Coloring -------------------- //

				// TODO: Support multiple textures
				// Take only the first element of the Texture Coordinate Array.
				for (int k = 0; k < 1 /* AI_MAX_NUMBER_OF_TEXTURECOORDS */; k++) {
					if (Scene->mMeshes[i]->HasTextureCoords(k)) {
						VertexData[j].TextureCoordinate = math::vec3<float>(
							Scene->mMeshes[i]->mTextureCoords[k][j].x,
							Scene->mMeshes[i]->mTextureCoords[k][j].y,
							Scene->mMeshes[i]->mTextureCoords[k][j].z
						);
					}
				}
				// Take an average of all the Colors associated with Vertex.
				for (int k = 0; k < 1 /*AI_MAX_NUMBER_OF_COLOR_SETS*/; k++) {
					if (Scene->mMeshes[i]->HasVertexColors(k)) {
						VertexData[j].Color += math::vec4<float>(
							Scene->mMeshes[i]->mColors[k][j].r,
							Scene->mMeshes[i]->mColors[k][j].g,
							Scene->mMeshes[i]->mColors[k][j].b,
							Scene->mMeshes[i]->mColors[k][j].a
						);
					}
				}
				// VertexData[j].Color /= AI_MAX_NUMBER_OF_COLOR_SETS;
			}

			// Load Index Data (Used for primitive rendering)
			mesh::index IndexData;
			if (Scene->mMeshes[i]->mNumVertices <= (1 << 16)) {
				IndexData.Type = VK_INDEX_TYPE_UINT16;
				IndexData.Data16 = std::vector<ushort>(Scene->mMeshes[i]->mNumFaces * 3);
			} else {
				IndexData.Type = VK_INDEX_TYPE_UINT32;
				IndexData.Data32 = std::vector<uint>(Scene->mMeshes[i]->mNumFaces * 3);
			}
			for (size_t j = 0; j < Scene->mMeshes[i]->mNumFaces; j++) {
				switch(IndexData.Type) {
				case VK_INDEX_TYPE_UINT16:
					IndexData.Data16[3*j + 0] = (ushort)Scene->mMeshes[i]->mFaces[j].mIndices[0];
					IndexData.Data16[3*j + 1] = (ushort)Scene->mMeshes[i]->mFaces[j].mIndices[1];
					IndexData.Data16[3*j + 2] = (ushort)Scene->mMeshes[i]->mFaces[j].mIndices[2];
					break;
				case VK_INDEX_TYPE_UINT32:
					IndexData.Data16[3*j + 0] = (uint)Scene->mMeshes[i]->mFaces[j].mIndices[0];
					IndexData.Data16[3*j + 1] = (uint)Scene->mMeshes[i]->mFaces[j].mIndices[1];
					IndexData.Data16[3*j + 2] = (uint)Scene->mMeshes[i]->mFaces[j].mIndices[2];
					break;
				default:
					break;
				}
			}

			// Load Bone Data
			std::vector<mesh::bone> BoneData(Scene->mMeshes[i]->mNumBones);
			for (size_t j = 0; j < BoneData.size(); j++) {
				aiBone* Bone			= Scene->mMeshes[i]->mBones[j];
				BoneData[j].Name		= Bone->mName.C_Str();
				BoneData[j].Vertex		= std::vector<mesh::bone::vertex_weight>(Bone->mNumWeights);
				for (size_t k = 0; k < BoneData[j].Vertex.size(); k++) {
					BoneData[j].Vertex[k].ID 		= Bone->mWeights[k].mVertexId;
					BoneData[j].Vertex[k].Weight 	= Bone->mWeights[k].mWeight;
				}
			}

			// Create Mesh Object
			this->Mesh[i] = mesh(nullptr, VertexData, IndexData, BoneData);
			this->Mesh[i].Name = Scene->mMeshes[i]->mName.C_Str();
			this->Mesh[i].MaterialIndex = Scene->mMeshes[i]->mMaterialIndex;
		}

		ModelImporter->FreeScene();
	}


	model::model(gcl::context* aContext, const char* aFilePath) : model(aFilePath) {

	}

	model::model(gcl::context* aContext, model* aModel) {

	}

	model::~model() {

	}

	// MeshInstanceCount * FaceCount
	size_t model::command_buffer_count() const {
		size_t CommandBufferCount = 0;
		for (const node& Chd : this->Hierarchy.Child) {
			CommandBufferCount += Chd.mesh_reference_count();
		}
		return CommandBufferCount;
	}

	bool model::initialize() {
		ModelImporter = new Assimp::Importer();
		return (ModelImporter != nullptr);
	}

	void model::terminate() {
		delete ModelImporter;
		ModelImporter = nullptr;
	}

}
