#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_MODEL_H
#define GEODESUKA_CORE_GRAPHICS_MODEL_H

#include <vector>

#include "../io/file.h"

#include "mesh.h"
#include "material.h"
#include "animation.h"

namespace geodesuka::core::object {
	class camera;
	class light;
}

namespace geodesuka::core::graphics {

	class model : public io::file {
	public:

		friend class geodesuka::engine;

		struct node {
			
			// Traversal Data
			node*								Root;
			node*								Parent;
			std::vector<node>					Child;

			// Metadata
			std::string							Name;
			math::mat4<float>					Transformation;
			std::vector<mesh::instance> 		MeshInstance;

			node();
			node(const node& aInput);
			node(node&& aInput) noexcept;
			~node();

			node& operator[](int aIndex);
			node& operator=(const node& aRhs);
			node& operator=(node&& aRhs) noexcept;

			// Update the node hierarchy. (Applies Node & Mesh Animations)
			void update(const animation* aAnimation, double aDeltaTime);	
			// Total Number of Nodes from this point on.
			size_t node_count() const;
			// Counts the total number of mesh references in the tree.
			size_t instance_count() const;
			// Turns entire node tree into linearized array with multiplied transforms.
			node linearize() const;
			// For this node, it will calculate the world transform for the node.
			math::mat4<float> global_transform() const;

		private:

			void linearize(int& aOffset, const node& aNode);

			void set_root(node* aRoot);

			void clear();

			void zero_out();

		};



		// --------------- Aggregate Model Resources --------------- //

		// Node Hierarchy
		std::string							Name;
		node								Hierarchy;			// Root Node Hierarchy 

		// Resources
		gcl::context*						Context;
		std::vector<mesh> 					Mesh;
		std::vector<material> 				Material;
		std::vector<animation> 				Animation;
		std::vector<gcl::image> 			Texture;
		// std::vector<object::light*> 		Light;				// Not Relevant To Model, open as stage.
		// std::vector<object::camera*> 		Camera;			// Not Relevant To Model, open as stage.
		gcl::buffer 						UniformBuffer;

		model();
		model(const char* aFilePath);
		model(gcl::context* aContext, const char* aFilePath);
		model(gcl::context* aContext, model* aModel);
		~model();

		void update(double aDeltaTime);

	private:

		static bool initialize();
		static void terminate();

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_MODEL_H
