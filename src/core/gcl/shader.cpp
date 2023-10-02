#include <geodesuka/core/gcl/shader.h>

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
//#include <glslang/SPIRV/SpvTools.h>
//#include <glslang/SPIRV/Logger.h>
#include <glslang/SPIRV/GlslangToSpv.h>
//#include <glslang/SPIRV/spirv.hpp>
//#include <glslang/SPIRV/spvIR.h>
//#include <glslang/SPIRV/SPVRemapper.h>

// Included for compiling
#include "ResourceLimits.h"

/*

#define double2 dvec2
#define double3 dvec3
#define double4 dvec4
#define double2x2 dmat2
#define double2x3 dmat2x3
#define double2x4 dmat2x4
#define double3x2 dmat3x2
#define double3x3 dmat3
#define double3x4 dmat3x4
#define double4x2 dmat4x2
#define double4x3 dmat4x3
#define double4x4 dmat4
*/

/*

// No Debug
SDL2-static.lib
SDL2.lib
SDL2main.lib
VkLayer_utils.lib
vulkan-1.lib

// Compiled Debug
GenericCodeGend.lib
glslang-default-resource-limitsd.lib
glslangd.lib
HLSLd.lib
MachineIndependentd.lib
OGLCompilerd.lib
OSDependentd.lib
shadercd.lib
shaderc_combinedd.lib
shaderc_sharedd.lib
shaderc_utild.lib
spirv-cross-c-sharedd.lib
spirv-cross-cd.lib
spirv-cross-cored.lib
spirv-cross-cppd.lib
spirv-cross-glsld.lib
spirv-cross-hlsld.lib
spirv-cross-msld.lib
spirv-cross-reflectd.lib
spirv-cross-utild.lib
SPIRV-Tools-diffd.lib
SPIRV-Tools-linkd.lib
SPIRV-Tools-lintd.lib
SPIRV-Tools-optd.lib
SPIRV-Tools-reduced.lib
SPIRV-Tools-sharedd.lib
SPIRV-Toolsd.lib
SPIRVd.lib
SPVRemapperd.lib

// Release
GenericCodeGen.lib
glslang-default-resource-limits.lib
glslang.lib
HLSL.lib
MachineIndependent.lib
OGLCompiler.lib
OSDependent.lib
shaderc.lib
shaderc_combined.lib
shaderc_shared.lib
shaderc_util.lib
spirv-cross-c-shared.lib
spirv-cross-c.lib
spirv-cross-core.lib
spirv-cross-cpp.lib
spirv-cross-glsl.lib
spirv-cross-hlsl.lib
spirv-cross-msl.lib
spirv-cross-reflect.lib
spirv-cross-util.lib
SPIRV-Tools-diff.lib
SPIRV-Tools-link.lib
SPIRV-Tools-lint.lib
SPIRV-Tools-opt.lib
SPIRV-Tools-reduce.lib
SPIRV-Tools-shared.lib
SPIRV-Tools.lib
SPIRV.lib
SPVRemapper.lib

*/

namespace geodesuka::core::gcl {

	shader::shader() {
		this->zero_out();
	}

	shader::shader(util::string& aFilePath) : io::file(aFilePath) {
		// Load Source File
		this->read(aFilePath.ptr());

		// Determine Stage and compile to SPIRV.
		this->Stage = eid_to_stage(this->ID);

		// Compile and generate SPIRV.
		this->compile_source(this->Stage, (const char*)this->HostData);
	}

	shader::shader(const char* aFilePath) : io::file(aFilePath) {
		// Load Source File
		this->read(aFilePath);

		// Determine Stage and compile to SPIRV.
		this->Stage = eid_to_stage(this->ID);

		// Compile and generate SPIRV.
		this->compile_source(this->Stage, (const char*)this->HostData);
	}

	shader::shader(stage aStage, util::string& aSource) {
		this->zero_out();

		if (aSource.ptr() != NULL) {
			this->HostData = malloc((aSource.size() + 1) * sizeof(char));
			if (this->HostData != NULL) {
				this->HostSize = aSource.size();
				memcpy(this->HostData, aSource.ptr(), this->HostSize * sizeof(char));
				((char*)this->HostData)[this->HostSize] = '\0';
			}
		}

		// Compile and generate SPIRV.
		this->compile_source(this->Stage, (const char*)this->HostData);
	}

	shader::shader(stage aStage, const char* aSource) {
		this->zero_out();

		if (aSource != NULL) {
			size_t StrSize = strlen(aSource);
			this->HostData = malloc((StrSize + 1) * sizeof(char));
			if (this->HostData != NULL) {
				this->HostSize = StrSize;
				memcpy(this->HostData, aSource, this->HostSize * sizeof(char));
				((char*)this->HostData)[this->HostSize] = '\0';
			}
		}

		this->compile_source(this->Stage, (const char*)this->HostData);
	}

	shader::shader(context* aContext, const shader* aShaderSource) {
		bool Failure = false;
		VkResult Result = VK_SUCCESS;
		this->zero_out();

		// Copy over shader source code.
		if (aShaderSource != NULL) {
			this->HostData = malloc((aShaderSource->HostSize + 1) * sizeof(char));
			if (this->HostData != NULL) {
				this->HostSize = aShaderSource->HostSize;
				memcpy(this->HostData, aShaderSource->HostData, this->HostSize * sizeof(char));
				((char*)this->HostData)[this->HostSize] = '\0';
			}
		}

		// Generate SPIRV code.
		if (this->HostData != NULL) {
			Failure = this->compile_source(this->Stage, (const char*)this->HostData);
		}
		else {
			this->Stage = stage::UNKNOWN;
			this->ShaderHandle = nullptr;
		}

		// Generate Device Executable Code.
		if (!Failure) {
			Result = this->create_module(aContext, this->ByteCode);
		}
		else {
			this->Context = nullptr;
			this->Handle = VK_NULL_HANDLE;
		}
	}

	shader::shader(context* aDeviceContext, util::string& aFilePath) : io::file(aFilePath) {
		bool Failure = false;
		VkResult Result = VK_SUCCESS;

		// Load Source File
		this->read(aFilePath.ptr());

		// Determine Stage and compile to SPIRV.
		this->Stage = eid_to_stage(this->ID);

		// Compile out SPIRV.
		Failure = this->compile_source(this->Stage, (const char*)this->HostData);

		// Create Device Context Shader Code.
		Result = this->create_module(aDeviceContext, this->ByteCode);
	}

	shader::shader(context* aDeviceContext, const char* aFilePath) : io::file(aFilePath) {
		bool Failure = false;
		VkResult Result = VK_SUCCESS;

		// Load Source File
		Failure = this->read(aFilePath);

		// Determine Stage and compile to SPIRV.
		this->Stage = eid_to_stage(this->ID);

		// Compile out SPIRV.
		Failure = this->compile_source(this->Stage, (const char*)this->HostData);

		// Create Device Context Shader Code.
		Result = this->create_module(aDeviceContext, this->ByteCode);
	}

	shader::shader(context* aDeviceContext, stage aStage, util::string& aSource) {
		bool Failure = false;
		VkResult Result = VK_SUCCESS;

		this->zero_out();

		// Compile out SPIRV.
		Failure = this->compile_source(aStage, aSource.ptr());

		// Create Device Context Shader Code.
		Result = this->create_module(aDeviceContext, this->ByteCode);
	}

	shader::shader(context* aDeviceContext, stage aStage, const char* aSource) {
		bool Failure = false;
		VkResult Result = VK_SUCCESS;

		this->zero_out();

		// Compile out SPIRV.
		Failure = this->compile_source(aStage, aSource);

		// Create Device Context Shader Code.
		Result = this->create_module(aDeviceContext, this->ByteCode);
	}

	shader::~shader() {
		if ((this->Context != nullptr) && (this->Handle != VK_NULL_HANDLE)) {
			vkDestroyShaderModule(this->Context->handle(), this->Handle, NULL);
			this->Context = nullptr;
			this->Handle = VK_NULL_HANDLE;
		}
	}

	VkShaderStageFlagBits shader::get_stage() {
		return (VkShaderStageFlagBits)this->Stage;
	}

	VkShaderModule shader::handle() {
		return this->Handle;
	}

	VkPipelineShaderStageCreateInfo shader::stageci() {
		VkPipelineShaderStageCreateInfo Temp;
		Temp.sType					= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		Temp.pNext					= NULL;
		Temp.flags					= 0;
		Temp.stage					= this->get_stage();
		Temp.module					= this->Handle;
		Temp.pName					= "main";  // Entry Point function name
		Temp.pSpecializationInfo	= NULL;
		return Temp;
	}

	bool shader::initialize() {
		return glslang::InitializeProcess();
	}

	void shader::terminate() {
		glslang::FinalizeProcess();
	}

	shader::stage shader::eid_to_stage(file::extension aFileExtension){
		// Determine Stage and compile to SPIRV.
		switch (aFileExtension) {
		default:
			return stage::UNKNOWN;
		case SHADER_VSH:
			return stage::VERTEX;
		case SHADER_TCSH:
			return stage::TESSELLATION_CONTROL;
		case SHADER_TESH:
			return stage::TESSELLATION_EVALUATION;
		case SHADER_GSH:
			return stage::GEOMETRY;
		case SHADER_PSH:
			return stage::FRAGMENT;
		}
	}

	bool shader::compile_source(stage aStage, const char* aSource) {
		EShLanguage ShaderStage;
		bool CompilationSuccess = false;

		switch (aStage) {
		default:
			ShaderStage = (EShLanguage)0;
			break;
		case VERTEX:
			ShaderStage = EShLanguage::EShLangVertex;
			break;
		case TESSELLATION_CONTROL:
			ShaderStage = EShLanguage::EShLangTessControl;
			break;
		case TESSELLATION_EVALUATION:
			ShaderStage = EShLanguage::EShLangTessEvaluation;
			break;
		case GEOMETRY:
			ShaderStage = EShLanguage::EShLangGeometry;
			break;
		case FRAGMENT:
			ShaderStage = EShLanguage::EShLangFragment;
			break;
		case COMPUTE:
			ShaderStage = EShLanguage::EShLangCompute;
			break;
		}

		glslang::SpvOptions Option;
		spv::SpvBuildLogger Logger;

		glslang::EShSource Source									= glslang::EShSource::EShSourceGlsl;
		glslang::EShClient Client									= glslang::EShClient::EShClientVulkan;
		int ClientInputSemanticsVersion								= 100;
		const int DefaultVersion									= 100;
		glslang::EShTargetClientVersion ClientVersion				= glslang::EShTargetClientVersion::EShTargetVulkan_1_0;
		glslang::EShTargetLanguage TargetLanguage					= glslang::EShTargetLanguage::EShTargetSpv;
		glslang::EShTargetLanguageVersion TargetLanguageVersion		= glslang::EShTargetLanguageVersion::EShTargetSpv_1_0;

		//EShMessages Options = (EShMessages)(EShMessages::EShMsgAST | EShMessages::EShMsgSpvRules | EShMessages::EShMsgVulkanRules);
		EShMessages Options = (EShMessages)(
			EShMessages::EShMsgAST | 
			EShMessages::EShMsgSpvRules | 
			EShMessages::EShMsgVulkanRules | 
			EShMessages::EShMsgDebugInfo | 
			EShMessages::EShMsgBuiltinSymbolTable
		);

		Option.generateDebugInfo		= false;
		Option.disableOptimizer			= false;
		Option.optimizeSize				= false;
		Option.disassemble				= false;
		Option.validate					= false;

		//const int DefaultVersion = Options & EOptionDefaultDesktop ? 110 : 100;

		// Setup
		this->ShaderHandle = new glslang::TShader(ShaderStage);
		this->ShaderHandle->setStrings(&aSource, 1);
		this->ShaderHandle->setEnvInput(Source, ShaderStage, Client, ClientInputSemanticsVersion);
		this->ShaderHandle->setEnvClient(Client, ClientVersion);
		this->ShaderHandle->setEnvTarget(TargetLanguage, TargetLanguageVersion);
		this->ShaderHandle->setEntryPoint("main");

		//this->isValid = lShader.preprocess(&glslang::DefaultTBuiltInResource, DefaultVersion, ENoProfile, false, false, Options, NULL);
		CompilationSuccess = this->ShaderHandle->parse(&glslang::DefaultTBuiltInResource, DefaultVersion, false, Options);
		if (!CompilationSuccess) {
			std::cout << this->Name.ptr() << "." << this->Extension.ptr() << std::endl;
			std::cout << this->ShaderHandle->getInfoLog() << std::endl;
		}

		/*
		glslang::TProgram Program;
		Program.addShader(this->ShaderHandle);
		Program.link(Options);
		Program.buildReflection(EShReflectionAllIOVariables);

		std::cout << "Input Count:  " << Program.getNumPipeInputs() << std::endl;
		std::cout << "Output Count: " << Program.getNumPipeOutputs() << std::endl;
		std::cout << "Uniform Count: " << Program.getNumUniformBlocks() << std::endl;
		std::cout << "Uniform Count: " << Program.getNumUniformBlocks() << std::endl;

		for (int i = 0; i < Program.getNumPipeInputs(); i++) {
			std::cout << Program.getPipeInput(i).name << std::endl;
		}

		for (int i = 0; i < Program.getNumUniformBlocks(); i++) {
			std::cout << Program.getUniformBlock(i).name << std::endl;
		}

		for (int i = 0; i < Program.getNumUniformVariables(); i++) {
			std::cout << Program.getUniform(i).name << std::endl;
		}

		// Generate SPIRV code.
		glslang::GlslangToSpv(*Program.getIntermediate(ShaderStage), this->ByteCode, &Logger, &Option);
		std::cout << Logger.getAllMessages() << std::endl;
		*/

		return false;
	}

	VkResult shader::create_module(context* aContext, std::vector<uint>& aSPIRV) {
		VkResult Result = VK_ERROR_INITIALIZATION_FAILED;
		if (aContext != nullptr) {
			this->Context				= aContext;
			this->CreateInfo.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			this->CreateInfo.pNext		= NULL;
			this->CreateInfo.flags		= 0;
			this->CreateInfo.codeSize	= aSPIRV.size();
			this->CreateInfo.pCode		= aSPIRV.data();
			Result = vkCreateShaderModule(this->Context->handle(), &this->CreateInfo, NULL, &this->Handle);
		}
		else {
			this->Context = nullptr;
			this->Handle = VK_NULL_HANDLE;
		}
		return Result;
	}

	void shader::zero_out() {
		this->Path			= "";
		this->Directory		= "";
		this->Name			= "";
		this->Extension		= "";
		this->ID			= io::file::EID_UNKNOWN;

		this->HostSize			= 0;
		this->HostData			= NULL;

		this->Stage			= shader::stage::UNKNOWN;
		this->ShaderHandle	= nullptr;

		this->Context		= nullptr;
		this->Handle		= VK_NULL_HANDLE;
	}

}


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#include <glslang/MachineIndependent/attribute.h>
//#include <glslang/MachineIndependent/gl_types.h>
////#include <glslang/MachineIndependent/glslang_tab.cpp.h>
//#include <glslang/MachineIndependent/Initialize.h>
//#include <glslang/MachineIndependent/iomapper.h>
//#include <glslang/MachineIndependent/LiveTraverser.h>
//#include <glslang/MachineIndependent/localintermediate.h>
//#include <glslang/MachineIndependent/ParseHelper.h>
//#include <glslang/MachineIndependent/parseVersions.h>
//#include <glslang/MachineIndependent/pch.h>
//#include <glslang/MachineIndependent/propagateNoContraction.h>
//#include <glslang/MachineIndependent/reflection.h>
//#include <glslang/MachineIndependent/RemoveTree.h>
//#include <glslang/MachineIndependent/Scan.h>
//#include <glslang/MachineIndependent/ScanContext.h>
//#include <glslang/MachineIndependent/SymbolTable.h>
//#include <glslang/MachineIndependent/Versions.h>
//
//#include <glslang/Include/arrays.h>
//#include <glslang/Include/BaseTypes.h>
//#include <glslang/Include/Common.h>
//#include <glslang/Include/InitializeGlobals.h>
//#include <glslang/Include/intermediate.h>
//#include <glslang/Include/PoolAlloc.h>
//#include <glslang/Include/ResourceLimits.h>
//#include <glslang/Include/revision.h>
//#include <glslang/Include/ShHandle.h>
//#include <glslang/Include/Types.h>
//
//#include <glslang/Public/ShaderLang.h>

//#include <StandAlone/ResourceLimits.h>


//shader::shader() {
//
//}
//
////shader::shader(int ShaderType) {
////	Type = ShaderType;
////	ID = glCreateShader(ShaderType);
////}
//
//shader::~shader() {
//	if (ID != 0) { glDeleteShader(ID); ID = 0; }
//}
//
//GLenum shader::set_source(const char * Source) {
//	if (Source != NULL) {
//		size_t StrLength = strlen(Source);
//		if (StrLength > 0) {
//			if (Sauce != NULL) { free(Sauce); Sauce = NULL; }
//			Sauce = (char *)malloc((StrLength + 1)*sizeof(char));
//			if (Sauce != NULL) {
//				memcpy(Sauce, Source, StrLength*sizeof(char));
//				Sauce[StrLength] = '\0';
//			}
//		}
//	}
//	if ((ID != 0) && (Sauce != NULL)) {
//		glShaderSource(ID, 1, &Sauce, NULL);
//	}
//	return glGetError();
//}
//
//const char * shader::get_source() const {
//	return Sauce;
//}
//
//GLenum shader::get_param(GLenum ParamName, GLint * ParamVal) {
//	if (ID == 0) return -1;
//	glGetShaderiv(ID, ParamName, ParamVal);
//	return glGetError();
//}

//int shader::read(const char* FilePath) {
//	FILE* File = fopen(FilePath, "r");
//	if (File == NULL) return -1;
//	size_t FileSize = 0;
//	size_t FileLength = 0;
//	size_t SauceLength = 0;
//	int tErrorCode = 0;
//	tErrorCode |= fseek(File, 0L, SEEK_END);
//	FileSize = ftell(File);
//	tErrorCode |= fseek(File, 0L, SEEK_SET);
//	FileLength = FileSize / sizeof(char);
//	if (this->Sauce != NULL) {
//		SauceLength = strlen(this->Sauce);
//	}
//
//	void* nptr = NULL;
//	if (FileLength > 0) {
//		if (this->Sauce == NULL) {
//			nptr = malloc((FileLength + 1) * sizeof(char));
//		}
//		else {
//			if (FileLength != SauceLength) {
//				nptr = realloc(this->Sauce, (FileLength + 1) * sizeof(char));
//			}
//			else {
//				nptr = (void *)this->Sauce;
//			}
//		}
//	}
//
//	if (nptr == NULL) {
//		fclose(File);
//		return -1;
//	}
//	if (nptr != (void*)this->Sauce) { this->Sauce = (char*)nptr; }
//	nptr = NULL;
//
//	fclose(File);
//
//	return 0;
//}

//int shader::write(const char* FilePath) {
//	return 0;
//}

//int shader::load(const char * ShaderSource) {
//	if (Sauce != NULL) { free(Sauce); Sauce = NULL; }
//
//	if (ShaderSource != NULL) {
//		Sauce = (char *)malloc((strlen(ShaderSource) + 1) * sizeof(char));
//		if (Sauce != NULL) {
//			memcpy(Sauce, ShaderSource, strlen(ShaderSource) * sizeof(char));
//			Sauce[strlen(ShaderSource)] = '\0';
//		}
//	}
//
//	if ((Sauce == NULL) && (ShaderSource != NULL)) { return -1; } // Probably out of memory...
//	if (ID == 0) { return 0; }
//
//	glShaderSource(ID, 1, &Sauce, NULL);
//	
//	return 0;
//}

//int shader::clear() {
//	if (Sauce != NULL) { free(Sauce); Sauce = NULL; }
//	return 0;
//}

/*
int shader::parse() {
	int Msgs = EShMessages::EShMsgDefault | EShMessages::EShMsgAST | EShMessages::EShMsgDebugInfo;

	glslang::TShader Shader(EShLanguage::EShLangVertex);
	Shader.setAutoMapLocations(true);
	Shader.setAutoMapBindings(true);

	Shader.setStrings(&Sauce, 1);
	Shader.setEnvInput(glslang::EShSource::EShSourceGlsl, EShLanguage::EShLangVertex, glslang::EShClient::EShClientOpenGL, 330);
	Shader.setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EShTargetClientVersion::EShTargetOpenGL_450);
	if (Shader.parse(&DefaultTBuiltInResource, 330, EProfile::ECoreProfile, false, false, (EShMessages)Msgs)) {
		std::cout << "SUCCESS" << std::endl;
	}
	else {
		std::cout << "FAILURE" << std::endl;
	}
	std::cout << Shader.getInfoLog() << std::endl;
	std::cout << Shader.getInfoDebugLog() << std::endl;
	std::cout << "\n\n\n";

	glslang::TIntermediate *AST = Shader.getIntermediate();
	TIntermNode *Root = AST->getTreeRoot();
	glslang::TIntermSequence Seq = Root->getAsAggregate()->getSequence();
	glslang::TIntermSequence LinkerSeq = Seq.back()->getAsAggregate()->getSequence();
	
	std::vector<glslang::TIntermSymbol*> Symbol;
	std::vector<glslang::TIntermTyped*> Type;
	for (int i = 0; i < LinkerSeq.size(); i++) {
		Symbol.push_back(LinkerSeq[i]->getAsSymbolNode());
		Type.push_back(LinkerSeq[i]->getAsSymbolNode()->getAsTyped());
	}


	return 0;
}
*/

//int shader::compile() {
//	if (ID == 0) { return true; }
//	glCompileShader(ID);
//	glGetShaderiv(ID, GL_COMPILE_STATUS, &isCompiled);
//	if (isCompiled == GL_TRUE) {
//		return false;
//	}
//	else {
//		char ErrorLog[512];
//		glGetShaderInfoLog(ID, 512, NULL, ErrorLog);
//		printf(ErrorLog);
//		//std::cout << ErrorLog << std::endl;
//		return true;
//	}
//}

