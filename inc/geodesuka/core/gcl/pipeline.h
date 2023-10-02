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

			shader*												Shader[GCL_RASTERIZER_STAGE_COUNT];
			glslang::TProgram*									ProgramHandle;
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
