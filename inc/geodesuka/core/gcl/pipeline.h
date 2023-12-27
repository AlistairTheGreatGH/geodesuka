#pragma once
#ifndef GEODESUKA_CORE_GCL_PIPELINE_H
#define GEODESUKA_CORE_GCL_PIPELINE_H

#include <vector>

#include "config.h"

#include "variable.h"
#include "device.h"
#include "context.h"
#include "descriptor.h"
#include "buffer.h"
#include "image.h"
#include "shader.h"

/*
Flow Chart of Pipeline Creation.
Start:		     Vertex       Pixel
			[    Source       Source
Shader		[	    |			|
			[      AST         AST
					  \       / 
			[          Program
Rasterizer	[		  /       \
			[     SPIRV       SPIRV
			        |           |
			[     Module      Module
Device		[         \        / 
			[		   Pipeline
*/

namespace geodesuka::core::gcl {

	class pipeline {
	public:

		/*
		The point of these structure is a straightforward api to map vulkan managed resources,
		to slots indicated by the shaders provided.
		*/

		enum stage : uint {
			TOP_OF_PIPE 							= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			DRAW_INDIRECT 							= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
			VERTEX_INPUT 							= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VERTEX_SHADER 							= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			TESSELLATION_CONTROL_SHADER 			= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT,
			TESSELLATION_EVALUATION_SHADER 			= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT,
			GEOMETRY_SHADER 						= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT,
			FRAGMENT_SHADER 						= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			EARLY_FRAGMENT_TESTS 					= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			LATE_FRAGMENT_TESTS 					= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			COLOR_ATTACHMENT_OUTPUT 				= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			COMPUTE_SHADER 							= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			TRANSFER 								= VK_PIPELINE_STAGE_TRANSFER_BIT,
			BOTTOM_OF_PIPE 							= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			HOST 									= VK_PIPELINE_STAGE_HOST_BIT,
			ALL_GRAPHICS 							= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
			ALL_COMMANDS 							= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			NONE 									= VK_PIPELINE_STAGE_NONE,
			// TRANSFORM_FEEDBACK_EXT 					= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT,
			// CONDITIONAL_RENDERING_EXT 				= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT,
			// ACCELERATION_STRUCTURE_BUILD_KHR 		= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
			// RAY_TRACING_SHADER_KHR 					= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
			// FRAGMENT_DENSITY_PROCESS_EXT 			= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT,
			// FRAGMENT_SHADING_RATE_ATTACHMENT_KHR 	= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
			// COMMAND_PREPROCESS_NV 					= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV,
			// TASK_SHADER_EXT 						= VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT,
			// MESH_SHADER_EXT 						= VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT,
			// SHADING_RATE_IMAGE_NV 					= VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV,
			// RAY_TRACING_SHADER_NV 					= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV,
			// ACCELERATION_STRUCTURE_BUILD_NV 		= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV,
			// TASK_SHADER_NV 							= VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV,
			// MESH_SHADER_NV 							= VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV,
			// NONE_KHR 								= VK_PIPELINE_STAGE_NONE_KHR,
		};

		// Pre creation options for a rasterizer pipeline.
		struct rasterizer {

			friend class pipeline;

			// Vulkan Objects
			VkPipelineBindPoint									BindPoint;
			VkPipelineVertexInputStateCreateInfo				Input;
			VkPipelineInputAssemblyStateCreateInfo				InputAssembly;
			VkPipelineTessellationStateCreateInfo				Tesselation;
			VkPipelineViewportStateCreateInfo					Viewport;
			VkPipelineRasterizationStateCreateInfo				Rasterizer;
			VkPipelineMultisampleStateCreateInfo				Multisample;
			VkPipelineDepthStencilStateCreateInfo				DepthStencil;
			VkPipelineColorBlendStateCreateInfo					ColorBlend;
			VkPipelineDynamicStateCreateInfo					DynamicState;
			VkGraphicsPipelineCreateInfo						CreateInfo;

			// Uniform
			uint32_t 											DescriptorSetBindingCount;
			std::vector<VkDescriptorSetLayoutBinding> 			DescriptorSetBinding[GCL_DESCRIPTOR_SET_COUNT];
			int													UniformBufferCount;
			variable											UniformBuffer[GCL_MAX_DESCRIPTOR_SET_UNIFORM_BUFFERS];
			int													SamplerCount;
			variable											Sampler[GCL_MAX_DESCRIPTOR_SET_SAMPLERS];
			int													StorageBufferCount;
			variable											StorageBuffer[GCL_MAX_DESCRIPTOR_SET_STORAGE_BUFFERS];
			// Vertex Attribute Input
			//uint32_t											VertexBufferBindingCount;
			VkVertexInputBindingDescription						VertexBufferBindingDescription[GCL_VERTEX_BUFFER_BINDING_COUNT];
			//uint32_t											VertexAttributeCount;
			VkVertexInputAttributeDescription					VertexAttributeDescription[GCL_VERTEX_ATTRIBUTE_COUNT];
			variable											VertexAttributeVariable[GCL_VERTEX_ATTRIBUTE_COUNT];
			// Frame Attachment Output
			uint32_t											AttachmentCount;
			VkAttachmentDescription								AttachmentDescription[GCL_COLOR_ATTACHMENT_COUNT];
			VkAttachmentReference								AttachmentReference[GCL_COLOR_ATTACHMENT_COUNT];
			variable											AttachmentVariable[GCL_COLOR_ATTACHMENT_COUNT];

			rasterizer();
			rasterizer(uint32_t aShaderCount, shader** aShaderList);
			rasterizer(const rasterizer& aInput);
			~rasterizer();

			rasterizer& operator=(const rasterizer& aRhs);

			void bind(uint32_t aBindingReference, size_t aVertexStride, uint32_t aLocation, size_t aVertexOffset);

			std::vector<VkDescriptorPoolSize> descriptor_pool_size();

			void clear();

		private:

			shader*										Shader[GCL_RASTERIZER_STAGE_COUNT];
			glslang::TProgram*							ProgramHandle;
			std::vector<uint>							SPIRVByteCode[GCL_RASTERIZER_STAGE_COUNT];

			bool create(uint32_t aShaderCount, shader** aShaderList);

			// Set shader stage.
			bool set(shader* aShader);
			bool link_stages();
			bool generate_metadata();
			bool generate_spirv();

			void zero_out();

		};

		// Pre creation options for a raytracer pipeline.
		struct raytracer {

			VkPipelineBindPoint						BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
			VkRayTracingPipelineCreateInfoKHR		CreateInfo{};

			raytracer();
			//raytracer(uint32_t aShaderCount, shader** aShaderList);

		};

		// Pre creation options for a compute pipeline.
		// Requires on a single compute shader.
		struct compute {

			math::vec3<uint> 								GroupCount; // Number of Groups
			math::vec3<uint> 								GroupSize; //  Number of Items

			VkPipelineBindPoint						BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
			VkComputePipelineCreateInfo				CreateInfo{};

			compute();
			//compute(shader* aComputeShader);

		private:

			shader*									Shader;
			glslang::TProgram*						ProgramHandle;
			std::vector<uint>				ByteCode;

		};

		VkPipelineBindPoint 		BindPoint;
		VkPipelineLayout 			Layout;
		VkPipelineCache 			Cache;
		VkPipeline 					Handle;

		// Used to construct PipelineLayout and Allocate DescriptorSets.
		uint32_t 					DescriptorSetLayoutCount;
		VkDescriptorSetLayout 		DescriptorSetLayout[GCL_DESCRIPTOR_SET_COUNT];

		pipeline();

		// Creates rasterizer pipeline.
		pipeline(context* aContext, rasterizer& aRasterizer, VkRenderPass aRenderPass, uint32_t aSubpassIndex);

		// Creates raytracer pipeline.
		pipeline(context* aContext, raytracer& aRaytracer);

		// Creates compute pipeline.
		pipeline(context* aContext, compute& aCompute);

		pipeline(const pipeline& aInput);

		pipeline(pipeline&& aInput) noexcept;

		// Destructor
		~pipeline();

		pipeline& operator=(const pipeline& aRhs);
		pipeline& operator=(pipeline&& aRhs) noexcept;

		uint32_t descriptor_set_count() const;
		std::vector<VkDescriptorPoolSize> descriptor_pool_size() const;		

	private:

		// Reference to Host Memory
		rasterizer*											Rasterizer;
		raytracer*											Raytracer;
		compute*											Compute;

		// Pipline Construction.
		context*											Context;
		std::vector<VkPipelineShaderStageCreateInfo> 		Stage;

		VkGraphicsPipelineCreateInfo						RasterizerCreateInfo;
		VkRayTracingPipelineCreateInfoKHR					RaytracerCreateInfo;
		VkComputePipelineCreateInfo							ComputeCreateInfo;

		VkResult create(context* aContext, rasterizer& aRasterizer, VkRenderPass aRenderPass, uint32_t aSubpassIndex);

		void clear();

		void zero_out();

	};

	// RenderPass.begin(
}

#endif // !GEODESUKA_CORE_GCL_PIPELINE_H
