#pragma once
#ifndef GEODESUKA_BUILTIN_OBJECT_TRIANGLE_H
#define GEODESUKA_BUILTIN_OBJECT_TRIANGLE_H

#include <geodesuka/engine.h>

namespace geodesuka::builtin::object {

	class triangle : public core::object_t {
	public:

		friend class renderer;

		triangle(core::gcl::context* aContext, core::stage_t* aStage, const char* aName);
		~triangle();

		//virtual core::gcl::command_list draw(core::object::render_target* aRenderTarget) override;

	protected:

	private:

		core::gcl::buffer* VertexBuffer;
		core::gcl::shader VertexShader;
		core::gcl::shader PixelShader;

		// Hardcoded section for debugging.
		VkRenderPass RenderPass;
		std::vector<VkFramebuffer> FrameBuffer;
		std::vector<VkCommandBuffer> CommandBuffer;

		VkPipelineLayout PipelineLayout;
		VkPipeline Pipeline;

	};

}

#endif // !GEODESUKA_BUILTIN_OBJECT_TRIANGLE_H
