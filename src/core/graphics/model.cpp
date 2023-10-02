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
		assert(!aMeshNode.resize_children(aAssimpNode->mNumChildren));
		for (int i = 0; i < aMeshNode.ChildCount; i++) {
			aMeshNode[i].Root	= aMeshNode.Root;
			aMeshNode[i].Parent = &aMeshNode;
			fill_and_traverse(aMeshNode[i], aAssimpNode->mChildren[i]);
		}

		aMeshNode.Name = aAssimpNode->mName.C_Str();
		assert(!aMeshNode.resize_indices(aAssimpNode->mNumMeshes));
		for (int i = 0; i < aMeshNode.MeshIndexCount; i++) {
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
		this->Root					= this;
		this->Parent				= nullptr;
		this->ChildCount			= 0;
		this->Child					= nullptr;
		this->Name					= "";
		this->Transformation		= math::mat4<float>(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		this->MeshIndexCount		= 0;
		this->MeshIndex				= NULL;
	}

	model::node::node(const node& aInput) {
		this->Root			= this;
		this->Parent		= nullptr;
		this->ChildCount	= 0;
		this->Child			= nullptr;
		assert(!this->resize_children(aInput.ChildCount));
		for (int i = 0; i < this->ChildCount; i++) {
			this->Child[i].Root		= this;
			this->Child[i].Parent	= this;
			this->Child[i] = aInput.Child[i];
		}
	}

	model::node::node(node&& aInput) noexcept {
		this->Root				= this;
		this->Parent			= nullptr;
		this->ChildCount		= aInput.ChildCount;
		this->Child				= aInput.Child;
		this->Name				= aInput.Name;
		this->MeshIndexCount	= aInput.MeshIndexCount;
		this->MeshIndex			= aInput.MeshIndex;
		this->Transformation	= aInput.Transformation;
		aInput.ChildCount		= 0;
		aInput.Child			= nullptr;
		aInput.Name				= "";
		aInput.MeshIndexCount	= 0;
		aInput.MeshIndex		= NULL;
		aInput.Transformation	= math::mat4<float>(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		for (int i = 0; i < this->ChildCount; i++) {
			this->Child[i].set_root(this);
			this->Child[i].Parent = this;
		}
	}

	model::node::~node() {
		this->clear();
	}

	model::node& model::node::operator[](int aIndex) {
		return this->Child[aIndex];
	}

	model::node& model::node::operator=(const node& aRhs) {
		if (this == &aRhs) return *this;
		// Allocate memory for this node.
		assert(!this->resize_children(aRhs.ChildCount));
		assert(!this->resize_indices(aRhs.MeshIndexCount));
		// Recursively Load Info.
		for (int i = 0; i < this->ChildCount; i++) {
			this->Child[i].Root		= this->Root;
			this->Child[i].Parent	= this;
			this->Child[i]			= aRhs.Child[i];
		}
		// Move over actual data.
		this->Name				= aRhs.Name;
		this->Transformation	= aRhs.Transformation;
		this->MeshIndexCount	= aRhs.MeshIndexCount;
		memcpy(this->MeshIndex, aRhs.MeshIndex, this->MeshIndexCount * sizeof(int));
		return *this;
	}

	model::node& model::node::operator=(node&& aRhs) noexcept {
		this->clear();
		this->ChildCount		= aRhs.ChildCount;
		this->Child				= aRhs.Child;
		this->Name				= aRhs.Name;
		this->MeshIndexCount	= aRhs.MeshIndexCount;
		this->MeshIndex			= aRhs.MeshIndex;
		this->Transformation	= aRhs.Transformation;
		aRhs.zero_out();
		return *this;
	}

	bool model::node::resize_children(int aNewCount) {
		if (this->ChildCount == aNewCount) return false;
		if (aNewCount > 0) {
			node* nptr = new node[aNewCount];
			if (nptr == nullptr) return true;
			delete[] this->Child;
			this->ChildCount	= aNewCount;
			this->Child			= nptr;
		}
		else {
			delete[] this->Child;
			this->ChildCount	= 0;
			this->Child			= nullptr;
		}
		return false;
	}

	bool model::node::resize_indices(int aNewCount) {
		if (this->MeshIndexCount == aNewCount) return false;
		if (aNewCount > 0) {
			void* nptr = NULL;
			size_t TypeSize = sizeof(int);
			if (this->MeshIndex == NULL) {
				nptr = malloc(aNewCount * TypeSize);
			}
			else {
				nptr = realloc(this->MeshIndex, aNewCount * TypeSize);
			}
			if (nptr == NULL) return true;
			this->MeshIndex = (int*)nptr;
			if (aNewCount > this->MeshIndexCount) {
				for (int i = this->MeshIndexCount; i < this->MeshIndexCount + aNewCount; i++) {
					this->MeshIndex[i] = -1;
				}
			}
			this->MeshIndexCount = aNewCount;
		}
		else {
			free(this->MeshIndex);
			this->MeshIndex = NULL;
			this->MeshIndexCount = 0;
		}
		return false;
	}

	int model::node::count() {
		int TotalCount = 1;
		for (int i = 0; i < this->ChildCount; i++) {
			TotalCount += this->Child[i].count();
		}
		return TotalCount;
	}

	int model::node::mesh_reference_count() {
		int MeshCount = 0;
		MeshCount += this->MeshIndexCount;
		for (int i = 0; i < this->ChildCount; i++) {
			MeshCount += this->Child[i].mesh_reference_count();
		}
		return MeshCount;
	}

	model::node model::node::linearize() {
		node Linear;
		assert(!Linear.resize_children(this->count()));
		for (int i = 0; i < Linear.ChildCount; i++) {
			Linear[i].Root		= &Linear;
			Linear[i].Parent	= &Linear;
		}
		int LinearOffset = 0;
		Linear.linearize(LinearOffset, *this);
		return Linear;
	}

	math::mat4<float> model::node::global_transform() {
		// TODO: Do not forget there are animations that alter this.
		if (this->Parent != nullptr) {
			return this->Parent->global_transform() * this->Transformation;
		}
		else {
			return this->Transformation;
		}
	}

	void model::node::clear() {
		delete[] this->Child;
		free(this->MeshIndex);
		this->zero_out();
	}

	void model::node::linearize(int& aOffset, node& aNode) {
		assert(!this->Child[aOffset].resize_indices(aNode.MeshIndexCount));
		this->Child[aOffset].Name				= aNode.Name;
		this->Child[aOffset].MeshIndexCount		= aNode.MeshIndexCount;
		memcpy(this->Child[aOffset].MeshIndex, aNode.MeshIndex, aNode.MeshIndexCount * sizeof(int));
		this->Child[aOffset].Transformation		= aNode.global_transform();
		aOffset += 1;
		for (int i = 0; i < aNode.ChildCount; i++) {
			this->linearize(aOffset, aNode[i]);
		}
	}

	void model::node::set_root(node* aRoot) {
		this->Root = aRoot;
		for (int i = 0; i < this->ChildCount; i++) {
			this->Child[i].set_root(aRoot);
		}
	}

	void model::node::zero_out() {
		//this->Root					= nullptr;
		//this->Parent				= nullptr;
		this->ChildCount			= 0;
		this->Child					= nullptr;
		this->Name					= "";
		this->Transformation		= math::mat4<float>(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		this->MeshIndexCount		= 0;
		this->MeshIndex				= NULL;
	}

	model::model() {

	}

	model::model(util::string& aFilePath) {

	}

	model::model(const char* aFilePath) {
		this->load_host(aFilePath);
	}

	model::model(gcl::context* aContext, util::string& aFilePath)
	{
	}

	model::model(gcl::context* aContext, const char* aFilePath) {
		this->load_host(aFilePath);
	}

	model::model(gcl::context* aContext, model* aModel)
	{
	}

	model::~model() {

	}

	// MeshInstanceCount * FaceCount
	size_t model::command_buffer_count() const {
		size_t CommandBufferCount = 0;
		// Iterate through nodes with meshes.
		for (size_t i = 0; i < this->MeshNode.size(); i++) {
			// Iterate through mesh instances
			for (int j = 0; j < this->MeshNode[i].MeshIndexCount; j++) {
				// Get Face Count;
				int Index = this->MeshNode[i].MeshIndex[j];
				//CommandBufferCount += this->Mesh[Index]->Face.size();
			}
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

	bool model::load_host(const char* aFilePath) {
		const aiScene *Scene = ModelImporter->ReadFile(aFilePath, 0);

		// Extract Node Hierarchy.
		fill_and_traverse(this->RootNode, Scene->mRootNode);

		// Linearize Node Hierarchy
		node Linear = this->RootNode.linearize();

		this->LinearNode.resize(Linear.ChildCount);
		for (size_t i = 0; i < this->LinearNode.size(); i++) {
			this->LinearNode[i] = Linear[i];
			if (Linear[i].MeshIndexCount > 0){	
				this->MeshNode.push_back(Linear[i]);
			}
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
		for (int i = 0; i < Linear.ChildCount; i++) {
			std::cout << Linear[i].Name.ptr() << std::endl;
		}

		std::cout << "Hell0 World!" << std::endl;

		ModelImporter->FreeScene();
		return false;
	}


}
