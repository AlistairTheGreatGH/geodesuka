#pragma once
#ifndef GEODESUKA_CORE_OBJECT_WINDOW_H
#define GEODESUKA_CORE_OBJECT_WINDOW_H


/*
Types of Windows
	system_window: Not the Windows OS in particular, but a window that is 
	managed by the OS. Holds rendering context, and allows draw calls. A
	system window holds a context and is the default frame buffer.

	virtual_window: Does not hold a rendering context, and is a virtual window
	managed by an established 

	[NOT IMPLEMENTED]
	hmd_window: This will be for virtual reality headsets.

	camera: Does not hold a rendering context, but still serves as a rendering target.

Property List:
	Property I:
		A window object can stream it's contents to another window.

	Property II:
		Any window can be transformed into another window

*/

#include "../util/string.h"

#include "../math.h"

#include "../gcl/framebuffer.h"

#include "../object_t.h"
#include "render_target.h"

namespace geodesuka::core::object {

	// A window is a general type object that can be drawn to, which also has the properties
	// of every object, which it too can be drawn. Each window has a canvas, which is what is actually drawn
	// to. A full window is Canvas + Frame.
	class window : public render_target {
	public:

		struct setting {

			enum id {
				RESIZABLE,
				DECORATED,
				FOCUSED,
				AUTO_ICONIFY,
				FLOATING,
				MAXIMIZED,
				VISIBLE,
				SCALE_TO_MONITOR,
				CENTER_CURSOR,
				FOCUS_ON_SHOW,
				TRANSPARENCY
			};

			int Resizable;			// Creation Option
			int Decorated;			// Creation Option
			int UserFocused;		// Runtime Option
			int AutoMinimize;		// 
			int Floating;			// 
			int Maximized;			// 
			int Minimized;			// 
			int Visible;			// 
			int ScaleToMonitor;		// 
			int CenterCursor;		// 
			int FocusOnShow;		// 
			int Transparency;		// 
			int Fullscreen;			//
			int Hovered;			// 
			int ShouldClose; 		//

			setting();
		};

		~window();

		virtual void set_title(util::string aTitle);
		virtual void set_size(math::vec2<float> aSize);
		math::vec2<float> get_size();
		virtual void set_resolution(math::vec2<uint> aResolution);
		math::vec2<uint> get_resolution();
		virtual bool should_close();

		virtual std::vector<VkSubmitInfo> render(stage::canvas* aStage);

	protected:

		util::string 						Title;
		math::vec2<float> 					Size;
		setting 							Setting;
		VkSubmitInfo 						PrerenderOperation;
		VkSubmitInfo 						PostrenderOperation;

		window(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount);

		VkResult create_renderer();

	};

}

#endif // !GEODESUKA_CORE_OBJECT_WINDOW_H
