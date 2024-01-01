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

		// Extract Node Hierarchy.
		fill_and_traverse(this->Hierarchy, Scene->mRootNode);

		// Linearize Node Hierarchy
		this->LeafList = this->Hierarchy.linearize();
		
		this->Mesh			= std::vector<mesh*>(Scene->mNumMeshes);
		this->Material		= std::vector<material*>(Scene->mNumMaterials);
		this->Animation		= std::vector<animation*>(Scene->mNumAnimations);
		this->Texture		= std::vector<gcl::image*>(Scene->mNumTextures);
		this->Light			= std::vector<object::light*>(Scene->mNumLights);
		this->Camera		= std::vector<object::camera*>(Scene->mNumCameras);

		for (size_t i = 0; i < this->Mesh.size(); i++){
			std::vector<mesh::vertex> VertexData(Scene->mMeshes[i]->mNumVertices);
			for (size_t j = 0; j < VertexData.size(); j++) {
				VertexData[i].Position = math::vec3<float>(
					Scene->mMeshes[i]->mVertices[j].x,
					Scene->mMeshes[i]->mVertices[j].y,
					Scene->mMeshes[i]->mVertices[j].z
				);
				VertexData[i].Normal = math::vec3<float>(
					Scene->mMeshes[i]->mNormals[j].x,
					Scene->mMeshes[i]->mNormals[j].y,
					Scene->mMeshes[i]->mNormals[j].z
				);
				VertexData[i].Tangent = math::vec3<float>(
					Scene->mMeshes[i]->mTangents[j].x,
					Scene->mMeshes[i]->mTangents[j].y,
					Scene->mMeshes[i]->mTangents[j].z
				);
				VertexData[i].Bitangent = math::vec3<float>(
					Scene->mMeshes[i]->mBitangents[j].x,
					Scene->mMeshes[i]->mBitangents[j].y,
					Scene->mMeshes[i]->mBitangents[j].z
				);
			}
			Scene->mMeshes[i];
		}

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
