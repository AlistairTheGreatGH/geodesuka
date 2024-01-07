#pragma once
#ifndef GEODESUKA_CORE_OBJECT_CAMERA3D_H
#define GEODESUKA_CORE_OBJECT_CAMERA3D_H

#include "../object_t.h"
#include "camera.h"

namespace geodesuka::core::object {

	class camera3d : public camera {
	public:

		struct geometry_buffer {
			gcl::image DepthBuffer;
			gcl::image PixelColor;
			gcl::image PixelPosition;
			gcl::image PixelNormal;
			gcl::image PixelSpecular;
			geometry_buffer(gcl::context* aContext, math::vec2<uint> aResolution);
		};

		static constexpr int				ID = 5;

		gcl::pipeline::rasterizer 			Rasterizer;
		std::vector<geometry_buffer> 		GeometryBuffer;

		// ----- object_t methods ----- //

		// ----- rendertarget methods ----- //

		virtual int id() override;

		// ----- camera methods ----- //

		// ----- camera3d methods ----- //

		camera3d(gcl::context* aContext, stage::scene3d* aScene3D, const char* aName, math::vec3<float> aPosition, math::vec2<int> aResolution, double aFrameRate, uint32_t aFrameCount);
		//~camera3d();

		std::vector<VkSubmitInfo> render(stage::scene3d* aStage);

	protected:

		// ----- object_t methods ----- //

		virtual void update(double aDeltaTime) override;

		// ----- rendertarget methods ----- //

	private:

		// Generates the Perspective Projection Matrix.
		float							FieldOfView;
		float							AspectRatio;
		float							MinDistance;
		float							MaxDistance;

		// Orientation Info.
		float							Theta;
		float							Phi;

		// Camera Transform Info.
		math::mat4<float>				PRT;
		math::mat4<float>				Projection;
		math::mat4<float>				Rotation;
		math::mat4<float>				Translation;

		gcl::buffer 					Camera3DUniformBuffer;

		// Subpass 0:

		std::vector<VkPipelineColorBlendAttachmentState> BlendingOperation;

		// Opaque Objects:
		// The depth list is a list of sorted objects based
		// on the distance from the camera they are. The opaque
		// objects nearest to the camera will be rendered first.

		// Transparent Objects:
		// Objects that have transparancey will be rendered
		// furthest from the camera first, to nearest to the camera
		// last.
		
		// Translucent Objects:
		// Objects with that have translucency (i.e. Alpha != 1.0) 
		// will be drawn where the furthest objects are rendered 
		// first, and lastly objectes nearest to the camera will
		// be last.

		// These are temporary grouping, only used to generate 
		// command lists.
		object_list OpaqueObject;
		object_list TransparentObject;
		object_list TranslucentObject;

		gcl::command_list *OpaqueObjectCommandList;
		gcl::command_list *TransparentObjectCommandList;
		gcl::command_list *TranslucentObjectCommandList;

		VkResult create_images();

		VkResult create_pipelines();


	};

}
#endif // !GEODESUKA_CORE_OBJECT_CAMERA3D_H
