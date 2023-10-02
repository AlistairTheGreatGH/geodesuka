#pragma once
#ifndef GEODESUKA_CORE_GCL_SHADER_H
#define GEODESUKA_CORE_GCL_SHADER_H

#include <vector>

#include "device.h"
#include "context.h"

#include "../io/file.h"

namespace glslang {
	class TShader;
	class TProgram;
}

namespace geodesuka::core::gcl {

	class shader : public io::file /*: public glslang::TIntermTraverser*/ {
	public:

		friend class geodesuka::engine;
		friend class pipeline;

		enum stage {
			UNKNOWN,
			// Rasterization Shader Stages.
			VERTEX						= 0x00000001,
			TESSELLATION_CONTROL		= 0x00000002,
			TESSELLATION_EVALUATION		= 0x00000004,
			GEOMETRY					= 0x00000008,
			FRAGMENT					= 0x00000010,
			PIXEL						= FRAGMENT,
			// Compute Shader Stage.
			COMPUTE						= 0x00000020,
			// Raytracing Shader Stages.
			RAYGEN						= 0x00000100,
			ANY_HIT						= 0x00000200,
			CLOSEST_HIT					= 0x00000400,
			MISS						= 0x00000800,
			INTERSECTION				= 0x00001000,
			CALLABLE					= 0x00002000,
		};

		// ---------- Host Memory Object ---------- //

		shader();

		// Loads from file.
		shader(util::string& aFilePath);
		shader(const char* aFilePath);

		// Creates Shader from source code.
		shader(stage aStage, util::string& aSource);
		shader(stage aStage, const char* aSource);

		// ---------- Device Specific Memory Object ---------- //

		// Takes a contextless shader and creates it with device context.
		shader(context* aContext, const shader* aShaderSource);

		// Loads from file, compiles SPIRV, and creates shader module.
		shader(context* aDeviceContext, util::string& aFilePath);
		shader(context* aDeviceContext, const char* aFilePath);

		// Creates Shader from source code.
		shader(context* aDeviceContext, stage aStage, util::string& aSource);
		shader(context* aDeviceContext, stage aStage, const char* aSource);

		// Just clears shader up.
		~shader();
		
		VkShaderStageFlagBits get_stage();
		VkShaderModule handle();

		VkPipelineShaderStageCreateInfo stageci();

	private:

		static bool initialize();
		static void terminate();
		static stage eid_to_stage(file::extension aFileExtension);

		// Host memory data.
		stage							Stage;
		glslang::TShader*				ShaderHandle;
		std::vector<uint>		ByteCode;

		// Device memory data.
		context*						Context;
		VkShaderModuleCreateInfo	CreateInfo{};
		VkShaderModule				Handle;

		// TODO: Rename to something else, does not generate SPIRV.
		bool compile_source(stage aStage, const char* aSource);
		//bool generate_spirv();
		VkResult create_module(context* aContext, std::vector<uint>& aSPIRV);
		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_SHADER_H
