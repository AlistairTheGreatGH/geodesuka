#pragma once
#ifndef GEODESUKA_CORE_OBJECT_RENDER_TARGET_H
#define GEODESUKA_CORE_OBJECT_RENDER_TARGET_H

#include <vector>

#include "../gcl/config.h"
#include "../gcl/context.h"
#include "../gcl/image.h"

#include "../gcl/command_list.h"
#include "../gcl/command_pool.h"

#include "../logic/timer.h"

#include "../object_t.h"
#include "../object_list.h"

/// <summary>
/// render_target is an extendable class which the engine user can use to create custom
/// render targets for whatever the user may need. For something to be considered a render
/// target, it must have frame attachments which can be used as targets for rendering commands
/// by derived object classes.
/// </summary>

namespace geodesuka::core::object {

	// TODO: subject_t
	class render_target : public object_t {
	public:

		friend class geodesuka::engine;
		friend class core::stage_t;

		struct render_info {
			std::vector<VkSubmitInfo> SubmitInfo;
			std::vector<VkPresentInfoKHR> PresentInfo;
		};

		// This renderer overide is needed for canvas compositor
		class render_target_renderer : public graphics::render_operation {
		public:

			render_target_renderer(gcl::context* aContext, window* aWindow, render_target* aRenderTarget);

		private:

		};

		struct frame {
			std::vector<gcl::command_list>				RenderingInfo;
			std::vector<gcl::image>						Image;
			std::vector<VkImageView>					Attachment;
			frame() {}
			frame(int aAttachmentCount);
		};

		// ----- Render Target Resources ----- //

		gcl::command_pool*							CommandPool;
		double										FrameRate;
		logic::timer 								FrameRateTimer;
		math::vec3<uint>							FrameResolution; 
		uint32_t									FrameReadIndex;
		uint32_t									FrameDrawIndex;
		std::vector<frame>							Frame;
		//std::vector<VkAttachmentDescription>		AttachmentDescription;

		// ----- Render Target Renderer ----- //
		// Render Target Defined Render Operations. (As opposed to object defined)

		// Default Renderer Info (Filled out by render_target implementations.)
		VkViewport									DefaultViewport;
		VkRect2D 									DefaultScissor;
		VkRect2D 									RenderArea;
		VkRenderPass 								RenderPass;
		std::vector<gcl::pipeline> 					Pipeline;

		// ----- Render Target Canvas Variables ----- ///

		// Used for Canvas Rendering of Render Target Contents
		math::vec2<float>							ScreenPosition;
		math::vec2<float>							ScreenSize;
		gcl::buffer 								RenderTargetUniformBuffer;

		~render_target();
		
		// Used for runtime rendertarget discrimination.
		virtual int id() = 0;

		// -------------------- Called Internally by render_target ------------------------- \\

		virtual VkResult next_frame();
		virtual VkPresentInfoKHR present_frame();

	protected:

		render_target(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount);

	};

}

#endif // !GEODESUKA_CORE_OBJECT_RENDER_TARGET_H
