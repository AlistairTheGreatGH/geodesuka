#pragma once
#ifndef GEODESUKA_CORE_GCL_DESCRIPTOR_H
#define GEODESUKA_CORE_GCL_DESCRIPTOR_H

#include <vector>

#include "config.h"

#include "context.h"
#include "buffer.h"
#include "image.h"
#include "shader.h"
//#include "pipeline.h"

namespace geodesuka::core::gcl {

	class descriptor {
	public:

		enum type {
			SAMPLER = 0,
			COMBINED_IMAGE_SAMPLER = 1,
			SAMPLED_IMAGE = 2,
			STORAGE_IMAGE = 3,
			UNIFORM_TEXEL_BUFFER = 4,
			STORAGE_TEXEL_BUFFER = 5,
			UNIFORM_BUFFER = 6,
			STORAGE_BUFFER = 7,
			UNIFORM_BUFFER_DYNAMIC = 8,
			STORAGE_BUFFER_DYNAMIC = 9,
			INPUT_ATTACHMENT = 10,
			INLINE_UNIFORM_BLOCK = 1000138000,
			ACCELERATION_STRUCTURE_KHR = 1000150000,
			ACCELERATION_STRUCTURE_NV = 1000165000,
			MUTABLE_VALVE = 1000351000,
		};

		class pool {
		public:

			pool();
			pool(uint32_t aDescriptorSetCount, const std::vector<VkDescriptorPoolSize>& aDescriptorPoolSizes);
			//pool(const std::vector<gcl::pipeline>& aPipelineList);
			~pool();
			
		private:

			std::vector<VkDescriptorPoolSize> 	PoolSize;

			context* 								Context;
			VkDescriptorPoolCreateInfo 			CreateInfo;
			VkDescriptorPool 						Handle;

		};

		// Contains the layout of a single descriptor set.
		class set {
		public:

			set();
			set(const set& aInput);
			set(set&& aInput) noexcept;
			~set();

			descriptor& operator[](int Index);
			descriptor operator[](int Index) const;
			set& operator=(const set& aRhs);
			set& operator=(set&& aRhs) noexcept;

			uint32_t count() const;

			bool resize(uint32_t aNewCount);

			void clear();

		private:

			//std::vector<descriptor> Handle;

			uint32_t Count;
			descriptor* Descriptor;

			void zero_out();

		};

		// Contains the layout of a series of descriptor sets.
		class table {
		public:

			table();
			table(const table& aInput);
			table(table&& aInput) noexcept;
			~table();

			set& operator[](uint32_t aIndex);
			set operator[](uint32_t aIndex) const;

			uint32_t count() const;

			bool resize(uint32_t aNewCount);

			void clear();

		private:

			uint32_t Count;
			set* Set;

			void zero_out();

		};

		class block {
		public:

			// Used to bind to pipeline.
			uint32_t SetCount;
			VkDescriptorSet SetHandle[4];

			block();
			block(context* aContext, table& aDescriptorTable);
			block(const block& aInput);
			block(block&& aInput) noexcept;
			~block();

			VkDescriptorSet& operator[](uint32_t aIndex);
			VkDescriptorSet operator[](uint32_t aIndex) const;
			block& operator=(const block& aRhs);
			block& operator=(block&& aRhs) noexcept;

			// Bind Uniform buffer
			void set_uniform(uint32_t aSet, uint32_t aBinding, buffer& aBuffer);

			// Bind Uniform buffer
			void set_uniform(uint32_t aSet, uint32_t aBinding, image& aBuffer);

		private:

			table DescriptorTable;

			context* Context;
			VkDescriptorPoolCreateInfo CreateInfo{};
			VkDescriptorPool Pool;

		};

		descriptor(uint32_t aBindingIndex, type aType, shader::stage aStage);
		descriptor(uint32_t aBindingIndex, type aType, uint32_t aCount, shader::stage aStage);
		descriptor(uint32_t aBindingIndex, type aType, shader::stage aStage, VkSampler* aSampler);
		descriptor(uint32_t aBindingIndex, type aType, uint32_t aCount, shader::stage aStage, VkSampler* aSampler);

	private:

		// VkDescriptorSetLayoutBinding
		uint32_t			Binding;
		type				Type;
		uint32_t			Count;
		shader::stage		Stage;
		VkSampler*			Sampler;
		
		// VkWriteDescriptorSet

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_DESCRIPTOR_H
