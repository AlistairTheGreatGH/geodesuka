#include <geodesuka/engine.h>
#include <geodesuka/core/object/camera3d.h>

#include <assert.h>

#include <cmath>

#include <geodesuka/core/stage/scene3d.h>

#define CAMERA3D_FRAME_COUNT 3

namespace geodesuka::core::object {

	using namespace gcl;

	// Ze = Near -> Zn = +1
	// Ze = Far -> Zn = -1
	// The farther out a vertex, the more negative it becomes.
	static math::mat4<float> perspective(float aFOV, float aAspectRatio, float aNear, float aFar) {
		//tex:
		// Aspect Ratio: $$a$$
		// Field of View (Radians): $$\theta$$
		// Near Point: $$n$$
		// Far Point: $$f$$
		// $$ x_{n} = \frac{1}{\tan{\frac{\theta}{2}}} \frac{x_{e}}{z_{e}}$$
		// $$ y_{n} = \frac{a}{\tan{\frac{\theta}{2}}} \frac{y_{e}}{z_{e}}$$
		// $$ z_{n} = \frac{1}{z_{e}} \bigg(-\frac{f+n}{f-n} z_{e} + \frac{2fn}{f-n} \bigg)$$ 
		// The $z$ term is why the perspective matrix must be a math::mat4<float> type 
		// and not just a math::mat3<float>. The set of equations above describe
		// the transform from what the perspective of the camera
		// to the screen space of the context.
		// 
		// The matrix then takes the form of 
		// $$ P =
		// \begin{bmatrix}
		// \frac{1}{\tan{\frac{\theta}{2}}} & 0 & 0 & 0 \\
		// 0 & \frac{a}{\tan{\frac{\theta}{2}}} & 0 & 0 \\
		// 0 & 0 & - \frac{f + n}{f - n} & \frac{2fn}{f - n} \\
		// 0 & 0 & 1 & 0 \\
		// \end{bmatrix}
		// $$

		float Tangent = std::tan(aFOV / 2.0);
		return math::mat4<float>(
			(1.0 / Tangent), 0.0, 0.0, 0.0,
			0.0, (aAspectRatio / Tangent), 0.0, 0.0,
			0.0, 0.0, (-((aFar + aNear) / (aFar - aNear))), ((2.0 * aFar * aNear) / ((double)aFar - (double)aNear)),
			0.0, 0.0, 1.0, 0.0
		);
	}

	// TODO: Create orthographic transformation matrix later.
	static math::mat4<float> orthographic() {
		return math::mat4<float>(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	//void camera3d::draw(object_t* aObject) {
	//	// Checks if this and aObject are the same object.
	//	if ((object_t*)this == aObject) return;
	//
	//	// Checks if both camera3d instance and aObject have
	//	// the same rendering context.
	//	//if (this->RenderingContext != aObject->RenderingContext) return -2;
	//
	//	// New idea for rendering objects to camera, sort
	//	// from nearest to farthest to make efficient rendering.
	//	// How to deal with alpha?
	//
	//	// DepthList:
	//	// The depth list is a list of sorted objects based
	//	// on the distance from the camera they are. The opaque
	//	// objects nearest to the camera will be rendered first.
	//
	//	// AlphaList:
	//	// Objects with that have translucency or are transparent
	//	// (i.e. Alpha != 1.0) will be drawn where the furthest objects
	//	// are rendered first for appropriate ordering.
	//
	//	//math::real DeltaPos = math::length(temp);
	//
	//	aObject->draw(this);
	//
	//}

	int camera3d::id() {
		return ID;
	}

	camera3d::camera3d(gcl::context* aContext, stage::scene3d* aScene3D, const char* aName, math::vec3<float> aPosition, math::vec2<int> aResolution, double aFrameRate, uint32_t aFrameCount) : 
		camera(aContext, aScene3D, aName, math::vec3<uint>(aResolution.x, aResolution.y, 1u), aFrameRate, aFrameCount, 4) 
	{
		VkResult Result = VK_SUCCESS;

		// ---------- camera3d ---------- //

		this->FieldOfView 	= 74.0f;
		this->AspectRatio 	= 1.0f;
		this->MinDistance 	= 10.0f;
		this->MaxDistance 	= 1000.0f;
		this->Theta 		= math::constant::pi;
		this->Phi 			= 0.0f;
		this->DirectionX 	= math::vec3<float>(sin(Phi), -cos(Phi), 0.0f);
		this->DirectionY 	= math::vec3<float>(-cos(Phi) * cos(Theta), -sin(Phi) * cos(Theta), sin(Theta));
		this->DirectionZ 	= math::vec3<float>(cos(Phi) * sin(Theta), sin(Phi) * sin(Theta), cos(Theta));

		this->Projection = perspective(this->FieldOfView, this->AspectRatio, this->MinDistance, this->MaxDistance);

		// Transforms to a RH coordinate system where +Z is popping out of the screen.
		// TODO: Insure default pipeline recognizes this definition in its constructor.
		this->Rotation = math::mat4<float>(
			this->DirectionX.x, this->DirectionX.y, this->DirectionX.z, 0.0f,
			this->DirectionZ.x, this->DirectionZ.y, this->DirectionZ.z, 0.0f,
			this->DirectionY.x, this->DirectionY.y, this->DirectionY.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		this->Translation = math::mat4<float>(
			1.0f, 0.0f, 0.0f, this->Position.x,
			0.0f, 1.0f, 0.0f, this->Position.y,
			0.0f, 0.0f, 1.0f, this->Position.z,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		//
		// Composition matrix created on host. +z pops out of the screen, -z
		// goes into the screen. To the right of the screen is +x, and -x is
		// to the left. Up is +y, and down is -y. Pictures are often more
		// helpful than words.
		// 
		//										+y
		// 	
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX^XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// -x	<-------------------------------X------------------------------->  +x
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		//		XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXvXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		// 
		//										-y
		//
		this->PRT = this->Projection * this->Rotation * this->Translation;

		// Fill out uniform buffer.

		this->create_images();

		this->create_render_pass();

		this->create_framebuffers();

		this->create_pipelines();

	}

	void camera3d::update(double aDeltaTime) {

		//VkSubmitInfo UpdateInfo{};
		this->FrameRateTimer.update(aDeltaTime);
		//return UpdateInfo;
	}

	std::vector<VkSubmitInfo> camera3d::render(stage_t* aStage) {
		std::vector<VkSubmitInfo> RenderBatch;
		// Can only exist as a render target in Scene3D.
		stage::scene3d* Scene3D = (stage::scene3d*)aStage;
		object_list OpaqueObject;
		object_list TransparentObject;
		object_list TranslucentObject;
		object_list SortedList(Scene3D->Object.count());

		// Prepares next frame for rendering operation by render target.
		this->next_frame();

		// Seperate by transparency.
		//assert(!this->RenderingGroup[this->FrameDrawIndex].resize(Scene3D->Object.count()));
		for (int i = 0; i < Scene3D->Object.count(); i++) {
			Scene3D->Object[i]->draw(this);
		}

		// Yield command 
		//RenderBatch = this->RenderingGroup[this->FrameDrawIndex].build();

		return RenderBatch;
	}

	camera3d::geometry_buffer::geometry_buffer(context* aContext, math::vec2<uint> aResolution) {
		VkResult Result = VK_SUCCESS;

		// New API design?
		image::create_info DepthCreateInfo;
		DepthCreateInfo.Sample		= image::sample::COUNT_1;
		DepthCreateInfo.Tiling		= image::tiling::OPTIMAL;
		DepthCreateInfo.Memory		= device::memory::DEVICE_LOCAL;
		DepthCreateInfo.Usage		= image::usage::SAMPLED | image::usage::DEPTH_STENCIL_ATTACHMENT;

		image::create_info ColorCreateInfo;
		ColorCreateInfo.Sample		= image::sample::COUNT_1;
		ColorCreateInfo.Tiling		= image::tiling::OPTIMAL;
		ColorCreateInfo.Memory		= device::memory::DEVICE_LOCAL;
		ColorCreateInfo.Usage		= image::usage::SAMPLED | image::usage::COLOR_ATTACHMENT;

		//this->DepthBuffer		= image(aContext, DepthCreateInfo, image::D32_SFLOAT, aResolution);
		//this->PixelColor		= image(aContext, ColorCreateInfo, type::id::FLOAT4, aResolution);
		//this->PixelPosition		= image(aContext, ColorCreateInfo, type::id::FLOAT4, aResolution);
		//this->PixelNormal		= image(aContext, ColorCreateInfo, type::id::FLOAT4, aResolution);
		//this->PixelSpecular		= image(aContext, ColorCreateInfo, type::id::FLOAT4, aResolution);

		VkFence Fence 							= aContext->create_fence();
		VkCommandBuffer CommandBuffer 		= aContext->create_command_buffer(device::GRAPHICS, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VkCommandBufferBeginInfo BeginInfo;

		BeginInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext					= NULL;
		BeginInfo.flags					= 0;
		BeginInfo.pInheritanceInfo		= NULL;

		Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);

		//this->DepthBuffer.cmd_transition(CommandBuffer,
		//	VK_ACCESS_MEMORY_WRITE_BIT,
		//	VK_ACCESS_MEMORY_READ_BIT,
		//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//);

		//this->PixelColor.cmd_transition(CommandBuffer,
		//	VK_ACCESS_MEMORY_WRITE_BIT,
		//	VK_ACCESS_MEMORY_READ_BIT,
		//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//);

		//this->PixelPosition.cmd_transition(CommandBuffer,
		//	VK_ACCESS_MEMORY_WRITE_BIT,
		//	VK_ACCESS_MEMORY_READ_BIT,
		//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//);

		//this->PixelNormal.cmd_transition(CommandBuffer,
		//	VK_ACCESS_MEMORY_WRITE_BIT,
		//	VK_ACCESS_MEMORY_READ_BIT,
		//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//);

		//this->PixelSpecular.cmd_transition(CommandBuffer,
		//	VK_ACCESS_MEMORY_WRITE_BIT,
		//	VK_ACCESS_MEMORY_READ_BIT,
		//	VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		//	VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		//	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		//);

		Result = vkEndCommandBuffer(CommandBuffer);

		Result = aContext->execute(device::GRAPHICS, CommandBuffer, Fence);
		Result = aContext->wait(Fence, VK_TRUE);

		aContext->destroy_fence(Fence);
		aContext->destroy_command_buffer(device::GRAPHICS, CommandBuffer);
	}

	VkResult camera3d::create_images() {

		// Create Images.
		for (size_t i = 0; i < this->Frame.size(); i++) {
			GeometryBuffer.emplace_back(Context, math::vec2<uint>(FrameResolution.x, FrameResolution.y));
		}

		// Fill out attachment description
		//AttachmentDescription[0] = this->GeometryBuffer[0].DepthBuffer.description(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE);
		//AttachmentDescription[1] = this->GeometryBuffer[0].PixelColor.description(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE);
		//AttachmentDescription[2] = this->GeometryBuffer[0].PixelPosition.description(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE);
		//AttachmentDescription[3] = this->GeometryBuffer[0].PixelNormal.description(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE);
		//AttachmentDescription[4] = this->GeometryBuffer[0].PixelSpecular.description(VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_STORE);

		return VK_SUCCESS;
	}

	VkResult camera3d::create_render_pass() {
		VkSubpassDescription Subpass;
		VkSubpassDependency Dependency;
		VkAttachmentReference ColorAttachment[4];
		VkAttachmentReference DepthAttachment;
		VkRenderPassCreateInfo RenderPassCreateInfo{};

		DepthAttachment.attachment				= 0;
		DepthAttachment.layout					= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		ColorAttachment[0].attachment			= 1;
		ColorAttachment[0].layout				= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		ColorAttachment[1].attachment			= 2;
		ColorAttachment[1].layout				= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		ColorAttachment[2].attachment			= 3;
		ColorAttachment[2].layout				= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		ColorAttachment[3].attachment			= 4;
		ColorAttachment[3].layout				= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		Subpass.flags							= 0;
		Subpass.pipelineBindPoint				= VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.inputAttachmentCount			= 0;
		Subpass.pInputAttachments				= NULL;
		Subpass.colorAttachmentCount			= 4;
		Subpass.pColorAttachments				= ColorAttachment;
		Subpass.pResolveAttachments				= NULL;
		Subpass.pDepthStencilAttachment			= &DepthAttachment;
		Subpass.preserveAttachmentCount			= 0;
		Subpass.pPreserveAttachments			= NULL;

		Dependency.srcSubpass					= VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass					= 0;
		Dependency.srcStageMask					= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.dstStageMask					= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		Dependency.srcAccessMask				= VK_ACCESS_MEMORY_WRITE_BIT;
		Dependency.dstAccessMask				= VK_ACCESS_MEMORY_READ_BIT;
		Dependency.dependencyFlags				= 0;

		RenderPassCreateInfo.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassCreateInfo.pNext				= NULL;
		RenderPassCreateInfo.flags				= 0;
		RenderPassCreateInfo.attachmentCount	= this->AttachmentDescription.size();
		RenderPassCreateInfo.pAttachments		= this->AttachmentDescription.data();
		RenderPassCreateInfo.subpassCount		= 1;
		RenderPassCreateInfo.pSubpasses			= &Subpass;
		RenderPassCreateInfo.dependencyCount	= 1;
		RenderPassCreateInfo.pDependencies		= &Dependency;

		return vkCreateRenderPass(Context->handle(), &RenderPassCreateInfo, NULL, &RenderPass);
	}

	VkResult camera3d::create_pipelines() {
		VkResult Result = VK_SUCCESS;

		// Per Color Attachment Blending.
		this->BlendingOperation.resize(4);
		for (size_t i = 0; i < this->BlendingOperation.size(); i++){
			this->BlendingOperation[i].blendEnable					= VK_FALSE;
			this->BlendingOperation[i].srcColorBlendFactor			= VK_BLEND_FACTOR_SRC_ALPHA;
			this->BlendingOperation[i].dstColorBlendFactor			= VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			this->BlendingOperation[i].colorBlendOp					= VK_BLEND_OP_ADD;
			this->BlendingOperation[i].srcAlphaBlendFactor			= VK_BLEND_FACTOR_ONE;
			this->BlendingOperation[i].dstAlphaBlendFactor			= VK_BLEND_FACTOR_ZERO;
			this->BlendingOperation[i].alphaBlendOp					= VK_BLEND_OP_ADD;
			this->BlendingOperation[i].colorWriteMask				= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		}

		// Load Shader Source, and compile to SPIRV. 
		gcl::shader* VertexSource = (gcl::shader*)Engine->open("assets/shader/camera3d.vert");
		gcl::shader* PixelSource = (gcl::shader*)Engine->open("assets/shader/camera3d.frag");

		gcl::shader* ShaderList[2] = { VertexSource, PixelSource };
		Rasterizer = gcl::pipeline::rasterizer(2, ShaderList);

		// Subpass 0:

		// Describe how Vertex Buffers must be be accessed while bound.
		Rasterizer.Input.vertexBindingDescriptionCount = 1;
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 0, offsetof(physics::mesh::vertex, Position));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 1, offsetof(physics::mesh::vertex, TextureCoordinate));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 2, offsetof(physics::mesh::vertex, Color));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 3, offsetof(physics::mesh::vertex, Normal));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 4, offsetof(physics::mesh::vertex, Tangent));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 5, offsetof(physics::mesh::vertex, Bitangent));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 6, offsetof(physics::mesh::vertex, BoneID));
		Rasterizer.bind(0, sizeof(physics::mesh::vertex), 7, offsetof(physics::mesh::vertex, BoneWeight));

		// How to intepret vertex data in rasterization.
		Rasterizer.InputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		Rasterizer.InputAssembly.primitiveRestartEnable		= false;

		// Viewport crap.
		Rasterizer.Viewport.scissorCount					= 1;
		Rasterizer.Viewport.pScissors						= &this->DefaultScissor;
		Rasterizer.Viewport.viewportCount					= 1;
		Rasterizer.Viewport.pViewports						= &this->DefaultViewport;

		// Rasterizer Info
		Rasterizer.Rasterizer.rasterizerDiscardEnable		= VK_FALSE;
		Rasterizer.Rasterizer.polygonMode					= VK_POLYGON_MODE_FILL;
		Rasterizer.Rasterizer.cullMode						= VK_CULL_MODE_BACK_BIT;
		Rasterizer.Rasterizer.frontFace						= VK_FRONT_FACE_COUNTER_CLOCKWISE;

		// Copy Paste
		Rasterizer.Multisample.rasterizationSamples			= VK_SAMPLE_COUNT_1_BIT;

		// Oncoming Depth Value [OPERATOR] Depth Value In Buffer
		// Needed for 3D graphics.
		Rasterizer.DepthStencil.depthTestEnable				= VK_TRUE;
		Rasterizer.DepthStencil.depthWriteEnable			= VK_TRUE;
		Rasterizer.DepthStencil.depthCompareOp				= VK_COMPARE_OP_GREATER; // Camera, +z is closer.
		Rasterizer.DepthStencil.minDepthBounds				= -1.0f;
		Rasterizer.DepthStencil.maxDepthBounds				= +1.0f;

		Rasterizer.ColorBlend.logicOpEnable					= VK_FALSE;
		Rasterizer.ColorBlend.logicOp						= VK_LOGIC_OP_COPY;
		Rasterizer.ColorBlend.attachmentCount				= this->BlendingOperation.size();
		Rasterizer.ColorBlend.pAttachments					= this->BlendingOperation.data();

		// Activate Depth Stencil Functionality.
		Rasterizer.CreateInfo.pDepthStencilState 			= &Rasterizer.DepthStencil;

		Pipeline.resize(1);
		Pipeline[0] = gcl::pipeline(Context, Rasterizer, VK_NULL_HANDLE, 0);

		return VK_SUCCESS;
	}

}