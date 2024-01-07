#include <geodesuka/engine.h>
#include <geodesuka/core/object_t.h>

#include <assert.h>

#include <vector>

// Recognized Render Targets
#include <geodesuka/core/object/render_target.h>
#include <geodesuka/core/object/window.h>
#include <geodesuka/core/object/system_display.h>
#include <geodesuka/core/object/system_window.h>
#include <geodesuka/core/object/virtual_window.h>
#include <geodesuka/core/object/camera.h>
#include <geodesuka/core/object/camera2d.h>
#include <geodesuka/core/object/camera3d.h>
//#include <geodesuka/core/object/cameravr.h>

// ------------------------------ public methods ------------------------------- //
// ------------------------------ protected methods ---------------------------- //
// ------------------------------ private methods ------------------------------ //

namespace geodesuka::core {

	object_t::default_renderer::default_renderer(gcl::context* aContext, object::window* aWindow, object_t* aObject) {
		this->Context = aContext;

		this->DrawCommand.resize(aWindow->Frame.size());

		for (size_t i = 0; i < aWindow->Frame.size(); i++) {

		}
		
	}

	object_t::default_renderer::default_renderer(gcl::context* aContext, object::camera2d* aCamera2D, object_t* aObject) {

	}

	object_t::default_renderer::default_renderer(gcl::context* aContext, object::camera3d* aCamera3D, object_t* aObject) {
		VkResult Result = VK_SUCCESS;

		this->Context = aContext;

		//// The number of descriptor sets needed must be determined by the render target.
		//std::vector<VkDescriptorPoolSize> DescriptorPoolSize = aCamera3D->descriptor_pool_sizes();
		//VkDescriptorPoolCreateInfo DescriptorPoolCreateInfo{};
		//DescriptorPoolCreateInfo.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		//DescriptorPoolCreateInfo.pNext				= NULL;
		//DescriptorPoolCreateInfo.flags				= 0;
		//DescriptorPoolCreateInfo.maxSets			= aCamera3D->descriptor_set_count();
		//DescriptorPoolCreateInfo.poolSizeCount		= DescriptorPoolSize.size();
		//DescriptorPoolCreateInfo.pPoolSizes			= DescriptorPoolSize.data();
		//
		//// ---------- Create Descriptor Sets ---------- //

		//this->DescriptorSet.resize(aCamera3D->Frame.size());
		//for (size_t i = 0; i < aCamera3D->Frame.size(); i++) {
		//	VkDescriptorSetAllocateInfo AllocateInfo{};
		//	AllocateInfo.sType					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		//	AllocateInfo.pNext					= NULL;
		//	AllocateInfo.descriptorPool			= this->DescriptorPool;
		//	AllocateInfo.descriptorSetCount		;
		//	AllocateInfo.pSetLayouts			;
		//	Result = vkAllocateDescriptorSets(aContext->handle(), &AllocateInfo, this->DescriptorSet[i].data());
		//}

		// ---------- Command Buffers ---------- //

		// When rendering a model, we will allocate a command buffer for every mesh instance 
		// in the node hierarchy of the model.
		DrawCommand = std::vector<std::vector<VkCommandBuffer>>(aCamera3D->Frame.size());
		for (size_t i = 0; i < aCamera3D->Frame.size(); i++) {
			// Get the mesh instance list to 
			std::vector<graphics::mesh::instance*> MeshInstance = aObject->Model->Hierarchy.gather_mesh_instances();
			DrawCommand[i] = std::vector<VkCommandBuffer>(MeshInstance.size());
			for (size_t j = 0; j < MeshInstance.size(); j++) {
				// Get reference for readability.
				graphics::mesh& Mesh 				= aObject->Model->Mesh[MeshInstance[j]->Index];
				gcl::pipeline& Pipeline 			= aCamera3D->Pipeline[0];
				std::vector<VkBuffer> VertexBuffer 	= {
					// This contains the vertex data for the static mesh.
					Mesh.VertexBuffer.handle(),
					// This buffer contains the per-vertex weight data {BoneID, BoneWeight}.
					MeshInstance[j]->VertexWeightBuffer.handle()
				};
				size_t IndexCount = 0;
				switch(aObject->Model->Mesh[MeshInstance[j]->Index].Index.Type) {
				case VK_INDEX_TYPE_UINT16:
					IndexCount = aObject->Model->Mesh[MeshInstance[j]->Index].Index.Data16.size();
					break;
				case VK_INDEX_TYPE_UINT32:
					IndexCount = aObject->Model->Mesh[MeshInstance[j]->Index].Index.Data32.size();
					break;
				default:
					IndexCount = 0;
					break;
				}

				// Draw here.
				Result = aContext->begin(DrawCommand[i][j]);
				aContext->begin_rendering(DrawCommand[i][j], aCamera3D->RenderArea, aCamera3D->Frame[i].Attachment);
				aContext->bind_vertex_buffers(DrawCommand[i][j], VertexBuffer);
				aContext->bind_index_buffer(DrawCommand[i][j], Mesh.IndexBuffer.handle(), Mesh.Index.Type);
				aContext->bind_descriptor_sets(DrawCommand[i][j], Pipeline.BindPoint, Pipeline.Layout, this->DescriptorSet[i]);
				aContext->bind_pipeline(DrawCommand[i][j], Pipeline.BindPoint, Pipeline.Handle);
				aContext->draw_indexed(DrawCommand[i][j], IndexCount);
				aContext->end_rendering(DrawCommand[i][j]);
				Result = aContext->end(DrawCommand[i][j]);
				// Draw here.
			}
		}

	}

	// ------------------------------ public methods ------------------------------- //

	object_t::~object_t() {

	}

	void object_t::set_position(math::vec3<float> aPosition) {
		this->Mutex.lock();
		this->Position = aPosition;
		this->Mutex.unlock();
	}

	math::vec3<float> object_t::get_position() const {
		return this->Position;
	}

	void object_t::set_name(const char* aName) {
		this->Mutex.lock();
		this->Name = aName;
		this->Mutex.unlock();
	}

	const char* object_t::get_name() const {
		return this->Name.ptr();
	}

	graphics::render_operation* object_t::generate_render_operation(object::render_target* aRenderTarget) {
		switch (aRenderTarget->id()) {
		case object::system_window::ID:
		case object::virtual_window::ID:
		// Check if object is a render target, the swap render ops.
			return new default_renderer(
				Context,
				(object::window*)aRenderTarget,
				this
			);
			break;
		case object::camera2d::ID:
			return new default_renderer(
				Context,
				(object::camera2d*)aRenderTarget,
				this
			);
			break;
		case object::camera3d::ID:
			return new default_renderer(
				Context,
				(object::camera3d*)aRenderTarget,
				this
			);
			break;
		default:
			return nullptr;
		}
	}

	std::vector<VkCommandBuffer> object_t::draw(object::render_target* aRenderTarget) {

		// Check if rendering operation exists for render target.
		if (this->RenderOperation.count(aRenderTarget) == 0) {
			// If render operation does not exist, generate one.
			this->RenderOperation[aRenderTarget] = this->generate_render_operation(aRenderTarget);
		}

		// Grab the command buffer list for the selected render targer and frame.
		return (*this->RenderOperation[aRenderTarget])[aRenderTarget->FrameDrawIndex];
	}

	// ------------------------------ protected methods ---------------------------- //

	//object_t::object_t() {}

	object_t::object_t(gcl::context* aContext, stage_t* aStage, const char* aName) {

		// Internal API.
		if (aContext != nullptr){
			Engine			= aContext->parent_engine();
		}
		else {
			Engine 			= nullptr;
		}
		Context			= aContext;
		Stage			= aStage;
		Name			= aName;

		InputVelocity	= math::vec3<float>(0.0, 0.0, 0.0);
		InputForce		= math::vec3<float>(0.0, 0.0, 0.0);

		Mass			= 1.0;
		Time			= 0.0;
		Position		= math::vec3<float>(0.0, 0.0, 0.0);
		Momentum		= math::vec3<float>(0.0, 0.0, 0.0);
		AngularMomentum = math::vec3<float>(0.0, 0.0, 0.0);
		Force			= math::vec3<float>(0.0, 0.0, 0.0);
		DirectionX		= math::vec3<float>(1.0, 0.0, 0.0);
		DirectionY		= math::vec3<float>(0.0, 1.0, 0.0);
		DirectionZ		= math::vec3<float>(0.0, 0.0, 1.0);
	}

	void object_t::input(const hid::mouse& aMouse, const hid::keyboard& aKeyboard) {

	}

	void object_t::update(double aDeltaTime) {
		//tex:
		// Update equations for generic object type with applied forces.
		// $$ d\vec{p} = \big( \vec{F}_{\text{applied}} + \vec{F}_{user} \big) dt $$ 
		// $$ d \vec{x} = \big( \frac{\vec{p}}{m} + \vec{v}_{user} \big) dt $$

		this->Mutex.lock();

		// Generic Free body motion equations.
		// Momentum += (Force + InputForce) * aDeltaTime;
		// Position += ((Momentum / Mass) + InputVelocity) * aDeltaTime;

		this->Mutex.unlock();
	}

	VkSubmitInfo object_t::transfer() {
		VkSubmitInfo TransferBatch{};
		TransferBatch.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		TransferBatch.pNext = NULL;
		TransferBatch.waitSemaphoreCount = 0;
		TransferBatch.pWaitSemaphores = NULL;
		TransferBatch.pWaitDstStageMask = NULL;
		TransferBatch.commandBufferCount = 0;
		TransferBatch.pCommandBuffers = NULL;
		TransferBatch.signalSemaphoreCount = 0;
		TransferBatch.pSignalSemaphores = NULL;
		return TransferBatch;
	}

	VkSubmitInfo object_t::compute() {
		VkSubmitInfo ComputeBatch{};
		ComputeBatch.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		ComputeBatch.pNext = NULL;
		ComputeBatch.waitSemaphoreCount = 0;
		ComputeBatch.pWaitSemaphores = NULL;
		ComputeBatch.pWaitDstStageMask = NULL;
		ComputeBatch.commandBufferCount = 0;
		ComputeBatch.pCommandBuffers = NULL;
		ComputeBatch.signalSemaphoreCount = 0;
		ComputeBatch.pSignalSemaphores = NULL;
		return ComputeBatch;
	}

}
