#include <geodesuka/core/gcl/pipeline.h>

//#include <cassert>
#include <assert.h>

#include <iostream>

//#include <glslang/Include/arrays.h>
#include <glslang/Include/BaseTypes.h>
#include <glslang/Include/Common.h>
#include <glslang/Include/ConstantUnion.h>
#include <glslang/Include/intermediate.h>
#include <glslang/Include/PoolAlloc.h>
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Include/SpirvIntrinsics.h>
#include <glslang/Include/Types.h>

#include <glslang/MachineIndependent/localintermediate.h>

#include <glslang/Public/ShaderLang.h>

// Converts shader source into SPIRV.
#include <glslang/SPIRV/GlslangToSpv.h>
//#include <glslang/SPIRV/Logger.h>
//#include <glslang/SPIRV/spirv.hpp>
//#include <glslang/SPIRV/spvIR.h>
//#include <glslang/SPIRV/SPVRemapper.h>
//#include <glslang/SPIRV/SpvTools.h>

// Included for compiling
#include "ResourceLimits.h"

#include <geodesuka/core/gcl/variable.h>

namespace geodesuka::core::gcl {

	static variable convert_to_variable(const glslang::TType* aType, const char* aName) {
		variable Variable;
		glslang::TBasicType BasicType = aType->getBasicType();
		if ((BasicType == glslang::TBasicType::EbtBlock) || (BasicType == glslang::TBasicType::EbtStruct)) {
			const glslang::TTypeList* TypeList = aType->getStruct();
			int ElementCount = TypeList->size();
			variable* VariableList = new variable[TypeList->size()];
			for (int i = 0; i < ElementCount; i++) {
				glslang::TType* MemberType = (*TypeList)[i].type;
				VariableList[i] = convert_to_variable(MemberType, MemberType->getFieldName().c_str());
			}
			// TODO: Figure out how to get name space
			Variable = variable(type(type::id::STRUCT, aType->getTypeName().c_str(), ElementCount, VariableList), aName);
			delete[] VariableList;
		}
		else if (BasicType == glslang::TBasicType::EbtSampler) {
			type::id ID;
			char NumString[64];
			util::string TypeName;

			switch (aType->getSampler().getBasicType()) {
			default:
				ID = type::id::UNKNOWN;
				break;
			case glslang::TBasicType::EbtFloat:
				ID = type::id::FLOAT;
				break;
			case glslang::TBasicType::EbtDouble:
				ID = type::id::DOUBLE;
				break;
				//case glslang::TBasicType::EbtFloat16:
				//	ID = type::id::HALF;
				//	break;
			case glslang::TBasicType::EbtInt8:
				ID = type::id::CHAR;
				break;
			case glslang::TBasicType::EbtUint8:
				ID = type::id::UCHAR;
				break;
			case glslang::TBasicType::EbtInt16:
				ID = type::id::SHORT;
				break;
			case glslang::TBasicType::EbtUint16:
				ID = type::id::USHORT;
				break;
			case glslang::TBasicType::EbtInt:
				ID = type::id::INT;
				break;
			case glslang::TBasicType::EbtUint:
				ID = type::id::UINT;
				break;
				//case glslang::TBasicType::EbtInt64:
				//	break;
				//case glslang::TBasicType::EbtUint64:
				//	break;
				//case glslang::TBasicType::EbtBool:
				//	break;
			}

			TypeName = type::name_of(ID);

			memset(NumString, 0x00, sizeof(NumString));
			if (aType->getSampler().vectorSize > 1) {
				sprintf(NumString, "%d", aType->getSampler().vectorSize);
				TypeName += NumString;
			}

			ID = type::id_of_string(TypeName.ptr());

			Variable = variable(ID, aName);
		}
		else {
			util::string TypeName = "";
			char NumString[64];
			type::id TypeID = type::id::UNKNOWN;

			memset(NumString, 0x00, sizeof(NumString));
			TypeName = aType->getBasicString();
			if (aType->isVector()) {
				sprintf(NumString, "%d", aType->getVectorSize());
				TypeName += NumString;
			}
			if (aType->isMatrix()) {
				sprintf(NumString, "%dx%d", aType->getMatrixRows(), aType->getMatrixCols());
				TypeName += NumString;
			}

			int ArrayDimCount = 0;
			int* ArrayDimSize = NULL;
			if (aType->isArray()) {
				const glslang::TArraySizes* lmao = aType->getArraySizes();
				ArrayDimCount = lmao->getNumDims();
				ArrayDimSize = (int*)malloc(ArrayDimCount * sizeof(int));
				for (int j = 0; j < lmao->getNumDims(); j++) {
					ArrayDimSize[j] = lmao->getDimSize(j);
				}
			}

			TypeID = type::id_of_string(TypeName.ptr());

			Variable = variable(TypeID, aName, ArrayDimCount, ArrayDimSize);
		}
		return Variable;
	}

	pipeline::rasterizer::rasterizer() {
		this->zero_out();
	}

	pipeline::rasterizer::rasterizer(uint32_t aShaderCount, shader** aShaderList) {
		// Zero out memory.
		this->zero_out();
		// Create Object.
		this->create(aShaderCount, aShaderList);
	}

	pipeline::rasterizer::rasterizer(const rasterizer& aInput) {
		this->zero_out();
		*this = aInput;
	}

	pipeline::rasterizer::~rasterizer() {
		this->clear();
	}

	pipeline::rasterizer& pipeline::rasterizer::operator=(const rasterizer& aRhs) {
		if (this == &aRhs) return *this;

		this->clear();

		std::vector<shader*> ShaderList;
		for (int i = 0; i < GCL_RASTERIZER_STAGE_COUNT; i++) {
			if (aRhs.Shader[i] != nullptr) {
				ShaderList.push_back(aRhs.Shader[i]);
			}
		}

		this->create(ShaderList.size(), ShaderList.data());

		return *this;
	}

	void pipeline::rasterizer::bind(uint32_t aBindingReference, size_t aVertexStride, uint32_t aLocation, size_t aVertexOffset) {
		this->VertexBufferBindingDescription[aBindingReference].binding		= aBindingReference;
		this->VertexBufferBindingDescription[aBindingReference].stride		= aVertexStride;
		this->VertexAttributeDescription[aLocation].location				= aLocation;
		this->VertexAttributeDescription[aLocation].binding					= aBindingReference;
		this->VertexAttributeDescription[aLocation].offset					= aVertexOffset;
	}

	std::vector<VkDescriptorPoolSize> pipeline::rasterizer::descriptor_pool_size() {
		std::vector<VkDescriptorPoolSize> PoolSize;
		for (uint32_t i = 0; i < this->DescriptorSetBindingCount; i++) {
			for (size_t j = 0; j < this->DescriptorSetBinding[i].size(); j++) {
				// Check if Descriptor Type Exists in Pool
				bool AlreadyExistsInSet = false;
				for (size_t k = 0; k < PoolSize.size(); k++) {
					AlreadyExistsInSet |= (PoolSize[k].type == DescriptorSetBinding[i][j].descriptorType);
				}

				if (AlreadyExistsInSet) {
					// Already Exists in set, add count.
					for (size_t k = 0; k < PoolSize.size(); k++) {
						if (PoolSize[k].type == DescriptorSetBinding[i][j].descriptorType) {
							PoolSize[k].descriptorCount += DescriptorSetBinding[i][j].descriptorCount;
						}
					}
				} else {
					// Does not exist in set, add new type.
					VkDescriptorPoolSize NewPoolSize;
					NewPoolSize.descriptorCount 	= DescriptorSetBinding[i][j].descriptorCount;
					NewPoolSize.type 				= DescriptorSetBinding[i][j].descriptorType;
					PoolSize.push_back(NewPoolSize);
				}
			}
		}
		return PoolSize;
	}

	void pipeline::rasterizer::clear() {
		delete this->ProgramHandle;
		this->zero_out();
	}

	bool pipeline::rasterizer::create(uint32_t aShaderCount, shader** aShaderList) {
		bool Success = true;

		if (Success) {

			// Load shader stages.
			for (uint32_t i = 0; i < aShaderCount; i++) {
				this->set(aShaderList[i]);
			}

			// Check if tesselation exists in this pipeline.
			if ((this->Shader[1] != nullptr) && (this->Shader[2] != nullptr)) {
				this->CreateInfo.pTessellationState = &this->Tesselation;
			}
			else {
				this->CreateInfo.pTessellationState = NULL;
			}

			for (int i = 0; i < GCL_RASTERIZER_STAGE_COUNT; i++) {
				if (this->Shader[i] != nullptr) {
					this->CreateInfo.stageCount += 1;
				}
			}

			Success = (this->CreateInfo.stageCount > 0);
		}

		// Link Shader Stages.
		if (Success) {
			Success = this->link_stages();
		}
		else {
			// No shaders provided.
			this->clear();
			Success = false;
		}

		// Build API reflection.
		if (Success) {
			Success = this->generate_metadata();
		}
		else {
			// Linking failed.
			this->clear();
			Success = false;
		}

		// Generate SPIRV code.
		if (Success) {
			Success = this->generate_spirv();
		}
		else {
			// failed to gather meta data.
			this->clear();
			Success = false;
		}

		return Success;
	}

	bool pipeline::rasterizer::set(shader* aShader) {
		switch (aShader->Stage) {
		default:
			return true;
		case shader::stage::VERTEX:
			if (this->Shader[0] == nullptr) {
				this->Shader[0] = aShader;
			}
			break;
		case shader::stage::TESSELLATION_CONTROL:
			if (this->Shader[1] == nullptr) {
				this->Shader[1] = aShader;
			}
			break;
		case shader::stage::TESSELLATION_EVALUATION:
			if (this->Shader[2] == nullptr) {
				this->Shader[2] = aShader;
			}
			break;
		case shader::stage::GEOMETRY:
			if (this->Shader[3] == nullptr) {
				this->Shader[3] = aShader;
			}
			break;
		case shader::stage::FRAGMENT:
			if (this->Shader[4] == nullptr) {
				this->Shader[4] = aShader;
			}
			break;
		}
		return false;
	}

	bool pipeline::rasterizer::link_stages() {
		bool LinkSuccessful = false;
		EShMessages Message = (EShMessages)(
			EShMessages::EShMsgAST |
			EShMessages::EShMsgSpvRules |
			EShMessages::EShMsgVulkanRules |
			EShMessages::EShMsgDebugInfo |
			EShMessages::EShMsgBuiltinSymbolTable
		);

		// Link various shader stages together.
		this->ProgramHandle = new glslang::TProgram();
		for (uint32_t i = 0; i < GCL_RASTERIZER_STAGE_COUNT; i++) {
			if (this->Shader[i] != nullptr) {
				this->ProgramHandle->addShader(this->Shader[i]->ShaderHandle);
			}
		}
		LinkSuccessful = this->ProgramHandle->link(Message);
		if (!LinkSuccessful) {
			std::cout << this->ProgramHandle->getInfoLog() << std::endl;
		}

		return LinkSuccessful;
	}

	bool pipeline::rasterizer::generate_metadata() {
		int k = 0;

		// Generates API Reflection.
		this->ProgramHandle->buildReflection(EShReflectionAllIOVariables);

		// -------------------- START -------------------- //
		
		// Get Uniform Buffers
		this->UniformBufferCount = this->ProgramHandle->getNumUniformBlocks();
		for (int i = 0; i < this->UniformBufferCount; i++) {
			const glslang::TObjectReflection& Variable = this->ProgramHandle->getUniformBlock(i);
			const glslang::TType* Type = Variable.getType();

			int Set					= Type->getQualifier().layoutSet;
			int Binding				= Type->getQualifier().layoutBinding;
			variable NewVariable	= convert_to_variable(Type, Variable.name.c_str());

			// Generate Bindings for Uniform Buffers.
			VkDescriptorSetLayoutBinding DSLB{};
			DSLB.binding				= Binding;
			DSLB.descriptorType			= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			DSLB.descriptorCount		= 1;
			DSLB.stageFlags				= Variable.stages; // TODO: Make conversion function later.
			DSLB.pImmutableSamplers		= NULL;

			this->DescriptorSetBinding[Set].push_back(DSLB);

			// Get Variable
			this->UniformBuffer[k] = NewVariable;
			k += 1;

			// Print Details
			std::cout << "layout" << " (set = " << Set << ", binding = " << Binding << ") uniform ";
			std::cout << NewVariable.to_string(false).ptr();
			std::cout << std::endl << std::endl;
		}
		std::cout << std::endl;

		// Get samplers
		for (int i = 0; i < this->ProgramHandle->getNumUniformVariables(); i++) {
			const glslang::TObjectReflection& Variable = this->ProgramHandle->getUniform(i);
			const glslang::TType* Type = Variable.getType();
			if (Type->getBasicType() == glslang::TBasicType::EbtSampler) {
				int Set									= Type->getQualifier().layoutSet;
				int Binding								= Type->getQualifier().layoutBinding;
				variable NewVariable					= convert_to_variable(Type, Variable.name.c_str());

				// Generate Bindings for Samplers.
				VkDescriptorSetLayoutBinding DSLB{};
				DSLB.binding							= Binding;
				DSLB.descriptorType						= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				DSLB.descriptorCount					= 1;
				DSLB.stageFlags							= VK_SHADER_STAGE_FRAGMENT_BIT; // Only exists in fragment shader.
				DSLB.pImmutableSamplers					= NULL;
				this->DescriptorSetBinding[Set].push_back(DSLB);

				// Get Sampler Metadata.
				this->Sampler[this->SamplerCount]		= NewVariable;
				this->SamplerCount						+= 1;

				// Print Details
				std::cout << "layout" << " (set = " << Set << ", binding = " << Binding << ") uniform ";
				std::cout << NewVariable.to_string(false).ptr();
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;

		// Find total number of descriptor sets with at least on Descriptor.
		for (int i = 0; i < GCL_DESCRIPTOR_SET_COUNT; i++) {
			if (this->DescriptorSetBinding[i].size() > 0) {
				this->DescriptorSetBindingCount += 1;
			}
		}

		//this->ProgramHandle->getNumBufferBlocks();
		//this->ProgramHandle->getNumBufferVariables();

		// Get Inputs
		k = 0;
		this->Input.vertexAttributeDescriptionCount = this->ProgramHandle->getNumPipeInputs();
		for (int i = 0; i < this->Input.vertexAttributeDescriptionCount; i++) {
			const glslang::TObjectReflection& Variable = this->ProgramHandle->getPipeInput(i);
			const glslang::TType* Type = Variable.getType();
			int Location = Type->getQualifier().layoutLocation;

			// Load Attribute Descriptions
			this->VertexAttributeVariable[i]				= convert_to_variable(Type, Variable.name.c_str());
			this->VertexAttributeDescription[i].location	= Location;
			this->VertexAttributeDescription[i].binding		= 0;
			this->VertexAttributeDescription[i].format		= (VkFormat)image::t2f(this->VertexAttributeVariable[i].Type.ID);
			this->VertexAttributeDescription[i].offset		= 0;

			std::cout << "layout (location = " << Location << ") in " << this->VertexAttributeVariable[i].Type.Name.ptr() << " " << this->VertexAttributeVariable[i].Name.ptr();
			for (int i = 0; i < this->VertexAttributeVariable[i].DimensionCount; i++) {
				std::cout << "[" << this->VertexAttributeVariable[i].Dimension[i] << "]";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;

		// Get Outputs
		k = 0;
		this->AttachmentCount = this->ProgramHandle->getNumPipeOutputs();
		for (int i = 0; i < this->AttachmentCount; i++) {
			const glslang::TObjectReflection& Variable = this->ProgramHandle->getPipeOutput(i);
			const glslang::TType* Type = Variable.getType();
			int Location = Type->getQualifier().layoutLocation;

			// TODO: Make sure that 
			this->AttachmentVariable[Location]				= convert_to_variable(Type, Variable.name.c_str());
			this->AttachmentDescription[Location].format	= (VkFormat)image::t2f(this->AttachmentVariable[i].Type.ID);
			this->AttachmentReference[Location].attachment	= 0;

			std::cout << "layout (location = " << Location << ") out " << this->AttachmentVariable[i].Type.Name.ptr() << " " << this->AttachmentVariable[i].Name.ptr();
			for (int i = 0; i < this->AttachmentVariable[i].DimensionCount; i++) {
				std::cout << "[" << this->AttachmentVariable[i].Dimension[i] << "]";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;

		return true;
	}

	bool pipeline::rasterizer::generate_spirv() {
		glslang::SpvOptions Option;
		spv::SpvBuildLogger Logger;

		for (uint32_t i = 0; i < GCL_RASTERIZER_STAGE_COUNT; i++) {
			if (this->Shader[i] != nullptr) {
				glslang::GlslangToSpv(*this->ProgramHandle->getIntermediate(this->Shader[i]->ShaderHandle->getStage()), this->SPIRVByteCode[i], &Logger, &Option);
			}
		}
		return true;
	}

	void pipeline::rasterizer::zero_out() {
		this->BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		this->Input = {};
		this->InputAssembly = {};
		this->Tesselation = {};
		this->Viewport = {};
		this->Rasterizer = {};
		this->Multisample = {};
		this->DepthStencil = {};
		this->ColorBlend = {};
		this->DynamicState = {};

		this->Input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		this->Input.pNext = NULL;
		this->Input.flags = 0;
		this->Input.vertexBindingDescriptionCount = 0;// this->VertexBufferBindingCount;
		this->Input.pVertexBindingDescriptions = this->VertexBufferBindingDescription;
		this->Input.vertexAttributeDescriptionCount = 0;// this->VertexAttributeCount;
		this->Input.pVertexAttributeDescriptions = this->VertexAttributeDescription;

		this->InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		this->InputAssembly.pNext = NULL;
		this->InputAssembly.flags = 0;

		this->Tesselation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		this->Tesselation.pNext = NULL;
		this->Tesselation.flags = 0;

		this->Viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		this->Viewport.pNext = NULL;
		this->Viewport.flags = 0;

		this->Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		this->Rasterizer.pNext = NULL;
		this->Rasterizer.flags = 0;

		this->Rasterizer.depthClampEnable = VK_FALSE;
		this->Rasterizer.rasterizerDiscardEnable = VK_FALSE;
		this->Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		this->Rasterizer.cullMode = VK_CULL_MODE_NONE;
		this->Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		this->Rasterizer.depthBiasEnable = VK_FALSE;
		this->Rasterizer.depthBiasConstantFactor = 0.0f;
		this->Rasterizer.depthBiasClamp = 0.0f;
		this->Rasterizer.depthBiasSlopeFactor = 0.0f;
		this->Rasterizer.lineWidth = 1.0f;

		this->Multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		this->Multisample.pNext = NULL;
		this->Multisample.flags = 0;
		//this->Multisample.rasterizationSamples		= (VkSampleCountFlagBits)0;
		this->Multisample.sampleShadingEnable = VK_FALSE;
		this->Multisample.minSampleShading = 1.0f;
		this->Multisample.pSampleMask = NULL;
		this->Multisample.alphaToCoverageEnable = VK_FALSE;
		this->Multisample.alphaToOneEnable = VK_FALSE;

		this->DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		this->DepthStencil.pNext = NULL;
		this->DepthStencil.flags = 0;
		this->DepthStencil.depthTestEnable = VK_FALSE;
		this->DepthStencil.depthWriteEnable = VK_FALSE;
		this->DepthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL; // Camera, +z is closer.
		this->DepthStencil.depthBoundsTestEnable = VK_FALSE;
		this->DepthStencil.stencilTestEnable = VK_FALSE;
		this->DepthStencil.front = {};
		this->DepthStencil.back = {};
		this->DepthStencil.minDepthBounds = 0.0f;
		this->DepthStencil.maxDepthBounds = 1.0f;

		this->ColorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		this->ColorBlend.pNext = NULL;
		this->ColorBlend.flags = 0;
		this->ColorBlend.logicOpEnable = VK_FALSE;
		this->ColorBlend.logicOp = VK_LOGIC_OP_COPY;
		this->ColorBlend.attachmentCount = 0;
		this->ColorBlend.pAttachments = NULL;
		this->ColorBlend.blendConstants[0] = 0.0f;
		this->ColorBlend.blendConstants[1] = 0.0f;
		this->ColorBlend.blendConstants[2] = 0.0f;
		this->ColorBlend.blendConstants[3] = 0.0f;

		this->DynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		this->DynamicState.pNext = NULL;
		this->DynamicState.flags = 0;

		this->UniformBufferCount = 0;
		this->SamplerCount = 0;
		this->StorageBufferCount = 0;

		// Referenced in pipeline create info.
		this->CreateInfo.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		this->CreateInfo.pNext					= NULL;
		this->CreateInfo.flags					= 0; // TODO: allow options later.
		this->CreateInfo.stageCount				= 0;
		this->CreateInfo.pStages				= NULL;
		this->CreateInfo.pVertexInputState		= &this->Input;
		this->CreateInfo.pInputAssemblyState	= &this->InputAssembly;
		this->CreateInfo.pTessellationState		= NULL;
		this->CreateInfo.pViewportState			= &this->Viewport;
		this->CreateInfo.pRasterizationState	= &this->Rasterizer;
		this->CreateInfo.pMultisampleState		= &this->Multisample;
		this->CreateInfo.pDepthStencilState		= NULL;// &this->DepthStencil; // Disabled by Default
		this->CreateInfo.pColorBlendState		= &this->ColorBlend;
		this->CreateInfo.pDynamicState			= NULL;// &this->DynamicState;
		this->CreateInfo.layout					= VK_NULL_HANDLE;
		this->CreateInfo.renderPass				= VK_NULL_HANDLE;
		this->CreateInfo.subpass				= 0;
		this->CreateInfo.basePipelineHandle		= VK_NULL_HANDLE;
		this->CreateInfo.basePipelineIndex		= 0;

		this->DescriptorSetBindingCount = 0;
		for (int i = 0; i < GCL_VERTEX_BUFFER_BINDING_COUNT; i++) {
			this->VertexBufferBindingDescription[i].binding		= i;
			this->VertexBufferBindingDescription[i].inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;
			this->VertexBufferBindingDescription[i].stride		= 0;
		}
		for (int i = 0; i < GCL_VERTEX_ATTRIBUTE_COUNT; i++) {
			this->VertexAttributeDescription[i].binding			= 0;
			this->VertexAttributeDescription[i].location		= i;
			this->VertexAttributeDescription[i].format			= VK_FORMAT_UNDEFINED;
			this->VertexAttributeDescription[i].offset			= 0;
		}

		this->AttachmentCount = 0;
		for (int i = 0; i < GCL_COLOR_ATTACHMENT_COUNT; i++) {
			this->AttachmentDescription[i].flags				= 0;
			this->AttachmentDescription[i].format				= VK_FORMAT_UNDEFINED;
			this->AttachmentDescription[i].samples				= (VkSampleCountFlagBits)0;
			this->AttachmentDescription[i].loadOp				= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			this->AttachmentDescription[i].storeOp				= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			this->AttachmentDescription[i].stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			this->AttachmentDescription[i].stencilStoreOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			this->AttachmentDescription[i].initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
			this->AttachmentDescription[i].finalLayout			= VK_IMAGE_LAYOUT_UNDEFINED;
			this->AttachmentReference[i].attachment				= 0;
			this->AttachmentReference[i].layout					= VK_IMAGE_LAYOUT_UNDEFINED;
		}

		for (int i = 0; i < 5; i++) {
			this->Shader[i] = nullptr;
		}
		this->ProgramHandle = nullptr;
	}

	pipeline::raytracer::raytracer() {

	}

	pipeline::compute::compute() {

	}

	pipeline::pipeline() {
		this->zero_out();
	}

	pipeline::pipeline(context* aContext, rasterizer& aRasterizer, VkRenderPass aRenderPass, uint32_t aSubpassIndex) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		this->BindPoint							= VK_PIPELINE_BIND_POINT_GRAPHICS;
		this->Context							= aContext;
		this->Rasterizer						= &aRasterizer;

		for (size_t i = 0; i < GCL_RASTERIZER_STAGE_COUNT; i++) {
			if (this->Rasterizer->Shader[i] != nullptr) {
				VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
				VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo{};

				ShaderModuleCreateInfo.sType							= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				ShaderModuleCreateInfo.pNext							= NULL;
				ShaderModuleCreateInfo.flags							= 0;
				ShaderModuleCreateInfo.codeSize							= this->Rasterizer->SPIRVByteCode[i].size() * sizeof(uint);
				ShaderModuleCreateInfo.pCode							= this->Rasterizer->SPIRVByteCode[i].data();

				PipelineShaderStageCreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				PipelineShaderStageCreateInfo.pNext						= NULL;
				PipelineShaderStageCreateInfo.flags						= 0;
				PipelineShaderStageCreateInfo.stage						= this->Rasterizer->Shader[i]->get_stage();
				PipelineShaderStageCreateInfo.module					= VK_NULL_HANDLE;
				PipelineShaderStageCreateInfo.pName						= "main";
				PipelineShaderStageCreateInfo.pSpecializationInfo		= NULL;

				Result = vkCreateShaderModule(Context->handle(), &ShaderModuleCreateInfo, NULL, &PipelineShaderStageCreateInfo.module);
				this->Stage.push_back(PipelineShaderStageCreateInfo);
			}
		}

		// Create Descriptor Set Layouts. Can be used to create Descriptor Sets for Objects.
		if (Result == VK_SUCCESS) {
			this->DescriptorSetLayoutCount = this->Rasterizer->DescriptorSetBindingCount;
			for (int i = 0; i < GCL_DESCRIPTOR_SET_COUNT; i++) {
				VkDescriptorSetLayoutCreateInfo CreateInfo {};
				CreateInfo.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				CreateInfo.pNext			= NULL;
				CreateInfo.flags			= 0; // Update After Binding Bit?
				CreateInfo.bindingCount		= Rasterizer->DescriptorSetBinding[i].size();
				CreateInfo.pBindings		= Rasterizer->DescriptorSetBinding[i].data();
				if (CreateInfo.bindingCount > 0) {
					Result = vkCreateDescriptorSetLayout(Context->handle(), &CreateInfo, NULL, &this->DescriptorSetLayout[i]);
				}
				else {
					this->DescriptorSetLayout[i] = VK_NULL_HANDLE;
				}
			}
		}

		// Create Pipeline Layout.
		if (Result == VK_SUCCESS) {
			// Create Pipeline Layout.
			VkPipelineLayoutCreateInfo CreateInfo{};
			CreateInfo.sType						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			CreateInfo.pNext						= NULL;
			CreateInfo.flags						= 0;
			CreateInfo.setLayoutCount				= this->DescriptorSetLayoutCount;
			CreateInfo.pSetLayouts					= this->DescriptorSetLayout;
			CreateInfo.pushConstantRangeCount		= 0;
			CreateInfo.pPushConstantRanges			= NULL;

			Result = vkCreatePipelineLayout(this->Context->handle(), &CreateInfo, NULL, &this->Layout);
		}
		else {

		}

		// Create Pipeline
		if (Result == VK_SUCCESS) {

			this->RasterizerCreateInfo							= this->Rasterizer->CreateInfo;
			this->RasterizerCreateInfo.stageCount				= this->Stage.size();
			this->RasterizerCreateInfo.pStages					= this->Stage.data();
			this->RasterizerCreateInfo.layout					= this->Layout;
			this->RasterizerCreateInfo.renderPass				= aRenderPass;
			this->RasterizerCreateInfo.subpass					= aSubpassIndex;
			this->RasterizerCreateInfo.basePipelineHandle		= VK_NULL_HANDLE;
			this->RasterizerCreateInfo.basePipelineIndex		= 0;
			
			Result = vkCreateGraphicsPipelines(this->Context->handle(), Cache, 1, &this->RasterizerCreateInfo, NULL, &this->Handle);
		}
		else {

		}
	}

	pipeline::pipeline(context* aContext, raytracer& aRaytracer) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		BindPoint	= VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
		Context		= aContext;
		Raytracer	= &aRaytracer;

		//Result = vkCreateRayTracingPipelinesKHR(Context->handle())
	}

	pipeline::pipeline(context* aContext, compute& aCompute) {
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		BindPoint	= VK_PIPELINE_BIND_POINT_COMPUTE;
		Context		= aContext;
		Compute		= &aCompute;

		//Result = vkCreateComputePipelines(Context->handle(), Cache, 1, &Compute.CreateInfo, NULL, &Handle);
	}

	pipeline::pipeline(const pipeline& aInput) {
		*this = aInput;
	}

	pipeline::pipeline(pipeline&& aInput) noexcept {
		this->BindPoint				= aInput.BindPoint;
		this->Layout				= aInput.Layout;
		this->Cache					= aInput.Cache;
		this->Handle				= aInput.Handle;

		this->DescriptorSetLayoutCount = aInput.DescriptorSetLayoutCount;
		for (int i = 0; i < GCL_DESCRIPTOR_SET_COUNT; i++) {
			this->DescriptorSetLayout[i] = aInput.DescriptorSetLayout[i];
		}

		this->Rasterizer			= aInput.Rasterizer;
		this->Raytracer				= aInput.Raytracer;
		this->Compute				= aInput.Compute;

		this->Context				= aInput.Context;
		this->Stage					= aInput.Stage;

		this->RasterizerCreateInfo	= aInput.RasterizerCreateInfo;
		this->RaytracerCreateInfo	= aInput.RaytracerCreateInfo;
		this->ComputeCreateInfo		= aInput.ComputeCreateInfo;

		aInput.zero_out();
	}

	pipeline::~pipeline() {
		this->clear();
	}

	pipeline& pipeline::operator=(const pipeline& aRhs) {
		VkResult Result = VK_SUCCESS;
		if (this == &aRhs) return *this;

		this->clear();

		switch (aRhs.BindPoint) {
		case VK_PIPELINE_BIND_POINT_GRAPHICS:
			Result = this->create(aRhs.Context, *aRhs.Rasterizer, aRhs.RasterizerCreateInfo.renderPass, aRhs.RasterizerCreateInfo.subpass);
			break;
		case VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR:
			// Does nothing atm
			break;
		case VK_PIPELINE_BIND_POINT_COMPUTE:
			// Does nothing atm.
			break;
		default:
			break;
		}

		return *this;
	}

	pipeline& pipeline::operator=(pipeline&& aRhs) noexcept {
		this->clear();

		this->BindPoint				= aRhs.BindPoint;
		this->Layout				= aRhs.Layout;
		this->Cache					= aRhs.Cache;
		this->Handle				= aRhs.Handle;

		this->DescriptorSetLayoutCount = aRhs.DescriptorSetLayoutCount;
		for (int i = 0; i < GCL_DESCRIPTOR_SET_COUNT; i++) {
			this->DescriptorSetLayout[i] = aRhs.DescriptorSetLayout[i];
		}

		this->Rasterizer			= aRhs.Rasterizer;
		this->Raytracer				= aRhs.Raytracer;
		this->Compute				= aRhs.Compute;

		this->Context				= aRhs.Context;
		this->Stage					= aRhs.Stage;

		this->RasterizerCreateInfo	= aRhs.RasterizerCreateInfo;
		this->RaytracerCreateInfo	= aRhs.RaytracerCreateInfo;
		this->ComputeCreateInfo		= aRhs.ComputeCreateInfo;

		aRhs.zero_out();
		return *this;
	}

	uint32_t pipeline::descriptor_set_count() const {
		switch (this->BindPoint) {		
		case VK_PIPELINE_BIND_POINT_GRAPHICS:
			return this->Rasterizer->DescriptorSetBindingCount;
		case VK_PIPELINE_BIND_POINT_COMPUTE:
			return 0;
		case VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR:
			return 0;
		default:
			return 0;
		}
	}

	std::vector<VkDescriptorPoolSize> pipeline::descriptor_pool_size() const {
		std::vector<VkDescriptorPoolSize> PoolSize;
		switch (this->BindPoint) {		
		case VK_PIPELINE_BIND_POINT_GRAPHICS:
			PoolSize = this->Rasterizer->descriptor_pool_size();
			break;
		case VK_PIPELINE_BIND_POINT_COMPUTE:
			break;
		case VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR:
			break;
		default:
			break;
		}
		return PoolSize;
	}

	VkResult pipeline::create(context* aContext, rasterizer& aRasterizer, VkRenderPass aRenderPass, uint32_t aSubpassIndex) {
		VkResult Result = VK_SUCCESS;
		this->BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		this->Context = aContext;
		this->Rasterizer = &aRasterizer;

		for (size_t i = 0; i < GCL_RASTERIZER_STAGE_COUNT; i++) {
			if (this->Rasterizer->Shader[i] != nullptr) {
				VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
				VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo{};

				ShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				ShaderModuleCreateInfo.pNext = NULL;
				ShaderModuleCreateInfo.flags = 0;
				ShaderModuleCreateInfo.codeSize = this->Rasterizer->SPIRVByteCode[i].size() * sizeof(uint);
				ShaderModuleCreateInfo.pCode = this->Rasterizer->SPIRVByteCode[i].data();

				PipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				PipelineShaderStageCreateInfo.pNext = NULL;
				PipelineShaderStageCreateInfo.flags = 0;
				PipelineShaderStageCreateInfo.stage = this->Rasterizer->Shader[i]->get_stage();
				PipelineShaderStageCreateInfo.module = VK_NULL_HANDLE;
				PipelineShaderStageCreateInfo.pName = "main";
				PipelineShaderStageCreateInfo.pSpecializationInfo = NULL;

				Result = vkCreateShaderModule(Context->handle(), &ShaderModuleCreateInfo, NULL, &PipelineShaderStageCreateInfo.module);
				this->Stage.push_back(PipelineShaderStageCreateInfo);
			}
		}

		// Create Descriptor Set Layouts. Can be used to create Descriptor Sets for Objects.
		if (Result == VK_SUCCESS) {
			this->DescriptorSetLayoutCount = this->Rasterizer->DescriptorSetBindingCount;
			for (int i = 0; i < GCL_DESCRIPTOR_SET_COUNT; i++) {
				VkDescriptorSetLayoutCreateInfo CreateInfo{};
				CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				CreateInfo.pNext = NULL;
				CreateInfo.flags = 0; // Update After Binding Bit?
				CreateInfo.bindingCount = Rasterizer->DescriptorSetBinding[i].size();
				CreateInfo.pBindings = Rasterizer->DescriptorSetBinding[i].data();
				if (CreateInfo.bindingCount > 0) {
					Result = vkCreateDescriptorSetLayout(Context->handle(), &CreateInfo, NULL, &this->DescriptorSetLayout[i]);
				}
				else {
					this->DescriptorSetLayout[i] = VK_NULL_HANDLE;
				}
			}
		}
		else {

		}

		// Create Pipeline Layout.
		if (Result == VK_SUCCESS) {
			// Create Pipeline Layout.
			VkPipelineLayoutCreateInfo CreateInfo{};
			CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			CreateInfo.pNext = NULL;
			CreateInfo.flags = 0;
			CreateInfo.setLayoutCount = this->DescriptorSetLayoutCount;
			CreateInfo.pSetLayouts = this->DescriptorSetLayout;
			CreateInfo.pushConstantRangeCount = 0;
			CreateInfo.pPushConstantRanges = NULL;

			Result = vkCreatePipelineLayout(this->Context->handle(), &CreateInfo, NULL, &this->Layout);
		}
		else {

		}

		// Create Pipeline
		if (Result == VK_SUCCESS) {

			this->RasterizerCreateInfo = this->Rasterizer->CreateInfo;
			this->RasterizerCreateInfo.stageCount = this->Stage.size();
			this->RasterizerCreateInfo.pStages = this->Stage.data();
			this->RasterizerCreateInfo.layout = this->Layout;
			this->RasterizerCreateInfo.renderPass = aRenderPass;
			this->RasterizerCreateInfo.subpass = aSubpassIndex;
			this->RasterizerCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			this->RasterizerCreateInfo.basePipelineIndex = 0;

			Result = vkCreateGraphicsPipelines(this->Context->handle(), Cache, 1, &this->RasterizerCreateInfo, NULL, &this->Handle);
		}
		else {

		}
		return Result;
	}

	void pipeline::clear() {
		if (this->Context != nullptr) {
			if (this->Handle != VK_NULL_HANDLE) {
				vkDestroyPipeline(this->Context->handle(), this->Handle, NULL);
				this->Handle = VK_NULL_HANDLE;
			}
			if (this->Cache != VK_NULL_HANDLE) {
				vkDestroyPipelineCache(this->Context->handle(), this->Cache, NULL);
				this->Cache = VK_NULL_HANDLE;
			}
			if (this->Layout != VK_NULL_HANDLE) {
				vkDestroyPipelineLayout(this->Context->handle(), this->Layout, NULL);
				this->Layout = VK_NULL_HANDLE;
			}
			this->BindPoint = VK_PIPELINE_BIND_POINT_MAX_ENUM;
			this->Context = nullptr;
		}
		this->zero_out();
	}

	void pipeline::zero_out() {
		// Public data.
		this->BindPoint					= VK_PIPELINE_BIND_POINT_MAX_ENUM;
		this->Layout					= VK_NULL_HANDLE;
		this->Cache						= VK_NULL_HANDLE;
		this->Handle					= VK_NULL_HANDLE;

		// Pipeline Construction Data.
		this->Rasterizer				= nullptr;
		this->Raytracer					= nullptr;
		this->Compute					= nullptr;

		// Pipline Specific Construction Data.
		this->Context					= nullptr;
		this->RasterizerCreateInfo		= {};
		this->RaytracerCreateInfo		= {};
		this->ComputeCreateInfo			= {};
	}

	//void pipeline::bind(VkCommandBuffer aCommandBuffer, uint32_t aVertexBufferCount, buffer** aVertexBufferList, buffer& aIndexBuffer, descriptor::block& aDescriptorBlock) {
	//	VkResult Result = VK_SUCCESS;
	//	VkBuffer VertexBufferArray[16];
	//	VkBuffer IndexBufferHandle = aIndexBuffer.handle();
	//	for (uint32_t i = 0; i < aVertexBufferCount; i++) {
	//		VertexBufferArray[i] = aVertexBufferList[i]->handle();
	//	}
	//	switch (this->BindPoint) {
	//	default:
	//		break;
	//	case VK_PIPELINE_BIND_POINT_GRAPHICS:
	//		vkCmdBindVertexBuffers(aCommandBuffer, 0, aVertexBufferCount, VertexBufferArray, NULL);
	//		vkCmdBindIndexBuffer(aCommandBuffer, IndexBufferHandle, 0, this->Rasterizer.IndexType);
	//		vkCmdBindDescriptorSets(aCommandBuffer, this->BindPoint, this->Layout, 0, this->Rasterizer.DescriptorSetCount, this->Rasterizer.DescriptorSet, 0, NULL);
	//		vkCmdBindPipeline(aCommandBuffer, this->BindPoint, this->Handle);
	//		break;
	//	}
	//}

	//void pipeline::bind(VkCommandBuffer aCommandBuffer) {
	//	switch (this->BindPoint) {
	//	default:
	//		break;
	//	case VK_PIPELINE_BIND_POINT_GRAPHICS:
	//		vkCmdBindVertexBuffers(aCommandBuffer, 0, this->Rasterizer.VertexBufferCount, this->Rasterizer.VertexBuffer, NULL);
	//		if (this->Rasterizer.IndexBuffer != VK_NULL_HANDLE) {
	//			vkCmdBindIndexBuffer(aCommandBuffer, this->Rasterizer.IndexBuffer, 0, this->Rasterizer.IndexType);
	//		}
	//		vkCmdBindDescriptorSets(aCommandBuffer, this->BindPoint, this->Layout, 0, this->Rasterizer.DescriptorSetCount, this->Rasterizer.DescriptorSet, 0, NULL);
	//		vkCmdBindPipeline(aCommandBuffer, this->BindPoint, this->Handle);
	//		break;
	//	}
	//}

}
