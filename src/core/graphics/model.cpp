#include <geodesuka/core/graphics/model.h>

#include <assert.h>

#include <iostream>

// Model Loading
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static Assimp::Importer* ModelImporter = nullptr;

/*
				  N[0]
				   |
	 ---------------------------------
	 |               |               |
   N[1]             N[2]             N[3]
			  ---------        -----------
			  |       |        |    |    |
			 N[4]    N[5]     N[6] N[7] N[8]
*/

//tex:
// When I am denoting raw vertices in mesh space for the 3d model, I will
// denote them as $ \vec{v} $ so they are not confused with bone space vertices
// $ \vec{v}^{bs} $. This is to eliminate ambiguity in mathematical symbols which
// many don't care for when describing something. The being said, in the node hierarchy
// it is neccessary to map mesh space vertices $ \vec{v} \rightarrow \vec{v}^{bs} $
// so that the node hierarchy and its associated animations can then be applied to 
// the mesh. 

//tex:
// When mapping mesh space vertices to bone space vertices, the offset matrix 
// $ A^{offset} $ provided in the bone structure associated with the mesh instance 
// maps them with the following relationship.
// $$ \vec{v}^{bs} = A^{offset} \vec{v} $$

//tex:
// $$ S = \{ B_{1}, B_{2}, \dots B_{n} \} \quad \quad \forall \; i \in \{ 1, 2, \dots n \} $$
// $$ B = \{ A^{bone}, A^{offset}, w \} $$
// When it comes to each mesh instance, they carry bone structures which 
// designating the vertices they are affecting along with a weight $ w_{i} $ indicating
// how much a bone affects the vertex in question. The thing is for convience
// in the per-vertex shader, only four vertex weights with the largest
// weights are passed into the vertex shader, but to maintain mathematical
// generality, the list can be longer. 
// $$ \vec{v}_{\text{model space}} =  \bigg( \sum_{i} A_{i}^{bone} \cdot A_{i}^{offset} w_{i} \bigg) \vec{v} $$
// If there is no bone structure applied to the mesh instance, then the global transform where the mesh
// instance exists will suffice.
// $$ \vec{v}_{\text{model space}} = A^{\text{mesh instance}} \vec{v} $$

//tex:
// How the entire node hierarchy works is that at every node in the 
// tree there exists a transformation matrix which informs how to transform
// on object in the node's space to its parent's node space. The Root node's
// space is the space of the entire model. The cumulative transform from each
// node is thus the map from the node's local space to the model space of the
// object.


namespace geodesuka::core::graphics {

	static void fill_and_traverse(const aiScene* aScene, model::node& aModelNode, aiNode* aAssimpNode) {
		// Copy over traversal data
		aModelNode.Child.resize(aAssimpNode->mNumChildren);
		for (size_t i = 0; i < aModelNode.Child.size(); i++) {
			aModelNode[i].Root 	= aModelNode.Root;
			aModelNode[i].Parent = &aModelNode;
			fill_and_traverse(aScene, aModelNode[i], aAssimpNode->mChildren[i]);
		}

		// Copy over naming and transformation node meta data.
		aModelNode.Name 		= aAssimpNode->mName.C_Str();
		aModelNode.Transformation = math::mat4<float>(
			aAssimpNode->mTransformation.a1, aAssimpNode->mTransformation.a2, aAssimpNode->mTransformation.a3, aAssimpNode->mTransformation.a4,
			aAssimpNode->mTransformation.b1, aAssimpNode->mTransformation.b2, aAssimpNode->mTransformation.b3, aAssimpNode->mTransformation.b4,
			aAssimpNode->mTransformation.c1, aAssimpNode->mTransformation.c2, aAssimpNode->mTransformation.c3, aAssimpNode->mTransformation.c4,
			aAssimpNode->mTransformation.d1, aAssimpNode->mTransformation.d2, aAssimpNode->mTransformation.d3, aAssimpNode->mTransformation.d4
		);

		// ASSIMP RANT: This is fucking stupid, what's the point
		// of mesh instancing if the fucking bone animation is 
		// tied to the mesh object itself instead of its instance?
		//
		// For future development, the bone association which
		// is the information needed to animate a mesh should
		// be tied to its instance and not the actual mesh object
		// itself. Instancing only makes sense if you want to
		// reuse an already existing mesh multiple times, and thus
		// you instance it. It then logically follows that the 
		// bone animation data should be tied to its instance
		// rather than the mesh object itself. Tying the bone 
		// structure to the mesh object quite literally defeats
		// the point of mesh instancing, at least for animated
		// meshes. Rant Over

		// This works by not only copying over the mesh instancing index, but
		// also copies over the vertex weight data which informs how to deform
		// the mesh instance. Used for animations.
		aModelNode.MeshInstance.resize(aAssimpNode->mNumMeshes);
		for (int i = 0; i < aAssimpNode->mNumMeshes; i++) {
			int MeshIndex 						= aAssimpNode->mMeshes[i];
			const aiMesh* Mesh 					= aScene->mMeshes[MeshIndex];
			std::vector<mesh::bone> BoneData(Mesh->mNumBones);
			for (size_t j = 0; j < BoneData.size(); j++) {
				aiBone* Bone = Mesh->mBones[j];
				// Get the name of the bone.
				BoneData[j].Name 	= Bone->mName.C_Str();
				// Copy over vertex affecting weights per bone.
				BoneData[j].Vertex = std::vector<mesh::bone::weight>(Bone->mNumWeights);
				for (size_t k = 0; k < BoneData[j].Vertex.size(); k++) {
					BoneData[j].Vertex[k].ID 		= Bone->mWeights[k].mVertexId;
					BoneData[j].Vertex[k].Weight 	= Bone->mWeights[k].mWeight;
				}
				// Copy over bind pose matrix. This converts vertices from mesh space to bone space.
				BoneData[j].Offset = math::mat4<float>(
					Bone->mOffsetMatrix.a1, Bone->mOffsetMatrix.a2, Bone->mOffsetMatrix.a3, Bone->mOffsetMatrix.a4,
					Bone->mOffsetMatrix.b1, Bone->mOffsetMatrix.b2, Bone->mOffsetMatrix.b3, Bone->mOffsetMatrix.b4,
					Bone->mOffsetMatrix.c1, Bone->mOffsetMatrix.c2, Bone->mOffsetMatrix.c3, Bone->mOffsetMatrix.c4,
					Bone->mOffsetMatrix.d1, Bone->mOffsetMatrix.d2, Bone->mOffsetMatrix.d3, Bone->mOffsetMatrix.d4
				);
			}
			// Load Mesh Instance Data
			aModelNode.MeshInstance[i] = mesh::instance(MeshIndex, Mesh->mNumVertices, BoneData);
		}
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
		this->Transformation		= aRhs.Transformation;
		this->MeshInstance 			= aRhs.MeshInstance;
		this->Animation 			= aRhs.Animation;
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

	model::node& model::node::operator[](const char* aName) {
		for (node& Chd : Child) {
			if (Chd.Name == aName) {
				return Chd;
			}
			else {
				return Chd[aName];
			}
		}
		throw std::runtime_error("Node Not Found");
	}

	void model::node::update(double aTime) {
		// Work done here will start at the root
		// of the node hierarchy.

		// Cycle Through Animation;
		for (node& Chd : Child) {
			Chd.update(aTime);
		}

		// Work done here implies that the leafs
		// will be updated first.

		// For each mesh instance, and for each bone, update the 
		// bone transformations according to their respective
		// animation object.
		for (mesh::instance& MI : MeshInstance) {
			// This is only used to tranform mesh instance vertices without bone animation.
			MI.Transform = this->global_transform(aTime);
			for (mesh::bone& B : MI.Bone) {
				// Update Bone Transformations from Bone Hierarchies
				B.Transform = (*this->Root)[B.Name.c_str()].global_transform(aTime);
			}
			// Update Bone Buffer Date GPU side.
			MI.update(aTime);
		}
	}

	size_t model::node::node_count() const {
		size_t TotalCount = 1;
		for (const node& Chd : Child) {
			TotalCount += Chd.node_count();
		}
		return TotalCount;
	}

	size_t model::node::instance_count() const {
		size_t MeshCount = 0;
		for (const node& Chd : Child) {
			MeshCount += Chd.MeshInstance.size();
			MeshCount += Chd.instance_count();
		}
		return MeshCount;
	}

	math::mat4<float> model::node::global_transform(double aTime) {
		// This calculates the global transform of the node which it is
		// being called from. If there are no animations associated with 
		// the node hierarchy, the bind pose transformations will be used
		// instead. If there are animations associated with the node hierarchy,
		// Then the animation transformations will be used in a weighted average.


		//tex:
		// It is the reponsibility of the model class to insure that the sum of the contribution
		// factors (weights) is equal to 1.
		// $$ 1 = w^{b} + \sum_{\forall A \in Anim} w_{i} $$
		// $$ T = T^{base} \cdot w^{base} + \sum_{\forall i \in A} T_{i}^{A} \cdot w_{i}^{A} $$ 
		//

		math::mat4<float> NodeTransform = this->Transformation * this->Weight;
		// Checks if there are animations at the Root Node.
		if (this->Root->Animation.size() > 0) {
			// If there are, iterate through them, get their transforms and
			// their contribution factors (weights).
			for (animation& Anim : this->Root->Animation) {
				// NodeTransform += AnimationTransform * Contribution Factor
				NodeTransform += Anim[this->Name][aTime]*Anim.Weight;
			}
		}

		// Recursively apply parent transformations.
		if (this->Parent != nullptr) {
			return this->Parent->global_transform(aTime) * NodeTransform;
		}
		else {
			return NodeTransform;
		}
	}

	std::vector<mesh::instance*> model::node::gather_mesh_instances() {
		std::vector<mesh::instance*> MIL(this->MeshInstance.size());
		for (size_t i = 0; i < MIL.size(); i++) {
			MIL[i] = &this->MeshInstance[i];
		}
		for (model::node& Chd : this->Child) {
			std::vector<mesh::instance*> CMIL = Chd.gather_mesh_instances();
			MIL.insert(MIL.end(), CMIL.begin(), CMIL.end());
		}
		return MIL;
	}

	void model::node::set_root(node* aRoot) {
		this->Root = aRoot;
		for (node& Chd : Child) {
			Chd.set_root(aRoot);
		}
	}

	void model::node::clear() {
		this->Child.clear();
		this->MeshInstance.clear();
		this->zero_out();
	}

	void model::node::zero_out() {
		this->Root				= this;
		this->Parent			= nullptr;
		this->Name				= "";
		this->Weight 			= 1.0f;
		this->Transformation 	= math::mat4<float>(
			1.0f, 	0.0f, 	0.0f, 	0.0f,
			0.0f,	1.0f, 	0.0f, 	0.0f,
			0.0f,	0.0f, 	1.0f, 	0.0f,
			0.0f,	0.0f, 	0.0f, 	1.0f
		);
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

		// Get Name of Model.
		this->Name = Scene->mName.C_Str();

		// Extract Scene Hiearchy
		fill_and_traverse(Scene, this->Hierarchy, Scene->mRootNode);

		// Extract Scene Animations. Animations will be tied to the node structure
		// since animation info is what animates the node structure. The reason why
		// elements are extracted as stack variables is simply for readability.
		this->Hierarchy.Animation = std::vector<animation>(Scene->mNumAnimations);
		for (size_t i = 0; i < this->Hierarchy.Animation.size(); i++) {
			aiAnimation* RA 	= Scene->mAnimations[i];
			animation& LA 		= this->Hierarchy.Animation[i];
			LA.Name					= RA->mName.C_Str();
			LA.Weight				= 0.0f; // Animation Disabled until specified otherwise.
			LA.Duration				= RA->mDuration;
			LA.TicksPerSecond		= RA->mTicksPerSecond;
			// Extract Node Animation Animation Structure.
			for (uint j = 0; j < RA->mNumChannels; j++) {
				// Get Node Animation Data Structure
				aiNodeAnim* RNA 										= RA->mChannels[j];
				std::string NodeName 									= RNA->mNodeName.C_Str();
				this->Hierarchy.Animation[i].NodeAnimMap[NodeName] 		= animation::node_anim();
				animation::node_anim& LNA 								= this->Hierarchy.Animation[i].NodeAnimMap[NodeName];

				// Initialize Vectors For Key Data
				LNA.PositionKey = std::vector<animation::key<math::vec3<float>>>(RNA->mNumPositionKeys);	
				LNA.RotationKey = std::vector<animation::key<math::quaternion<float>>>(RNA->mNumRotationKeys);
				LNA.ScalingKey = std::vector<animation::key<math::vec3<float>>>(RNA->mNumScalingKeys);

				// Get Position Keys
				for (uint k = 0; k < RNA->mNumPositionKeys; k++) {
					LNA.PositionKey[k].Time = RNA->mPositionKeys[k].mTime;
					LNA.PositionKey[k].Value = math::vec3<float>(
						RNA->mPositionKeys[k].mValue.x,
						RNA->mPositionKeys[k].mValue.y,
						RNA->mPositionKeys[k].mValue.z
					);
				}

				// Get Rotation Keys
				for (uint k = 0; k < RNA->mNumRotationKeys; k++) {
					LNA.RotationKey[k].Time = RNA->mRotationKeys[k].mTime;
					LNA.RotationKey[k].Value = math::quaternion<float>(
						RNA->mRotationKeys[k].mValue.w,
						RNA->mRotationKeys[k].mValue.x,
						RNA->mRotationKeys[k].mValue.y,
						RNA->mRotationKeys[k].mValue.z
					);
				}

				// Finally Get Scaling Keys
				for (uint k = 0; k < RNA->mNumScalingKeys; k++) {
					LNA.ScalingKey[k].Time = RNA->mScalingKeys[k].mTime;
					LNA.ScalingKey[k].Value = math::vec3<float>(
						RNA->mScalingKeys[k].mValue.x,
						RNA->mScalingKeys[k].mValue.y,
						RNA->mScalingKeys[k].mValue.z
					);
				}
			}
		}

		this->Mesh			= std::vector<mesh>(Scene->mNumMeshes);
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

			// Create Mesh Object
			this->Mesh[i] = mesh(nullptr, VertexData, IndexData);
			this->Mesh[i].Name = Scene->mMeshes[i]->mName.C_Str();
			this->Mesh[i].MaterialIndex = Scene->mMeshes[i]->mMaterialIndex;
		}

		// What is the point of a material? Material describes the material
		// of a surface. A material has qualities to it that affects how it
		// is seen.
		this->Material = std::vector<material>(Scene->mNumMaterials);
		for (size_t i = 0; i < Scene->mNumMaterials; i++) {
			aiMaterial *Mat = Scene->mMaterials[i];
			for (size_t j = 0; j < Mat->mNumProperties; j++) {
				aiMaterialProperty* Prop = Mat->mProperties[j];
				std::string Key = Prop->mKey.C_Str();
				if (Key == AI_MATKEY_NAME) {

				}

			}
			
			//	std::cout << "mKey = " << Prop->mKey.C_Str() << ",\t";
			//	std::cout << "mSemantic = " << Prop->mSemantic << ",\t";
			//	std::cout << "mIndex = " << Prop->mIndex << ",\t";
			//	std::cout << "mDataLength = " << Prop->mDataLength << ",\t";
			//	std::cout << "mType = " << Prop->mType << std::endl;
			//}
		}


		this->Texture = std::vector<gcl::image>(Scene->mNumTextures);

		ModelImporter->FreeScene();
	}


	model::model(gcl::context* aContext, const char* aFilePath) : model(aFilePath) {

	}

	model::model(gcl::context* aContext, model* aModel) {

	}

	model::~model() {

	}

	void model::update(double aDeltaTime) {
		Time += aDeltaTime;
		// Choose animation here.
		this->Hierarchy.update(aDeltaTime);
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
