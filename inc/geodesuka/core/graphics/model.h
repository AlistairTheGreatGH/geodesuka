#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MODEL_H
#define GEODESUKA_CORE_GRAPHICS_MODEL_H

#include <vector>

#include "../io/file.h"

#include "mesh.h"
#include "animation.h"
#include "material.h"

namespace geodesuka::core::object {
	class camera;
	class light;
}

namespace geodesuka::core::graphics {

	class model : public io::file {
	public:

		friend class geodesuka::engine;

		struct node {
			// Tree Info.
			node*			Root;
			node*			Parent;
			int				ChildCount;
			node*			Child;

			// Node Info.
			util::string	Name;
			int				MeshIndexCount;
			int*			MeshIndex;
			math::mat4<float>		Transformation;

			gcl::context* 	Context;
			gcl::buffer 	UniformBuffer;

			node();
			node(const node& aInput);
			node(node&& aInput) noexcept;
			~node();

			node& operator[](int aIndex);
			node& operator=(const node& aRhs);
			node& operator=(node&& aRhs) noexcept;

			bool resize_children(int aNewCount);
			bool resize_indices(int aNewCount);

			// Total Number of Nodes from this point on.
			int count();
			// Counts the total number of mesh references in the tree.
			int mesh_reference_count();
			// Turns entire node tree into linearized array with multiplied transforms.
			node linearize();

			// For this node, it will calculate the world transform for the node.
			math::mat4<float> global_transform();

			void clear();

		private:

			void linearize(int& aOffset, node& aNode);

			void set_root(node* aRoot);

			void zero_out();

		};



		// --------------- Aggregate Model Resources --------------- //

		// Node Hierarchy
		util::string						Name;
		node								RootNode;			// Root Node Hierarchy 
		std::vector<node> 					LinearNode;			// 
		std::vector<node> 					MeshNode;			// Contains List Mesh Transforms

		// Resources
		gcl::context*						Context;
		std::vector<mesh*> 					Mesh;
		std::vector<material*> 				Material;
		std::vector<animation*> 			Animation;
		std::vector<gcl::image*> 			Texture;
		std::vector<object::light*> 		Light;
		std::vector<object::camera*> 		Camera;
		gcl::buffer 						UniformBuffer;

		model();

		// Load into host memory.
		model(util::string& aFilePath);
		model(const char* aFilePath);

		// Load into host memory and 
		model(gcl::context* aContext, util::string& aFilePath);
		model(gcl::context* aContext, const char* aFilePath);

		// Create Model for usage for a device context.
		model(gcl::context* aContext, model* aModel);

		~model();

		// Calculates the total number of command buffers needed to
		// represent the model.
		size_t command_buffer_count() const;

	private:

		static bool initialize();
		static void terminate();

		bool load_host(const char* aFilePath);

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MODEL_H
