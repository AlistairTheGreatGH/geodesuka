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
			std::string							Name;				// Name of the Node in Hierarchy
			float 								Weight;				// Default Weight of the Node is 1.0f. 
			math::mat4<float>					Transformation;		// Static Bone to Model Space Transform Component = T0*T1*T2*...*Tn*Vbs
			std::vector<animation> 				Animation; 			// Overrides 
			std::vector<mesh::instance> 		MeshInstance; 		//

			node();
			node(const node& aInput);
			node(node&& aInput) noexcept;
			~node();

			node& operator=(const node& aRhs);
			node& operator=(node&& aRhs) noexcept;

			node& operator[](int aIndex);
			node& operator[](const char* aName);

			// Update the node hierarchy. (Applies Node & Mesh Animations)
			void update(double aTime);	
			// Total Number of Nodes from this point on.
			size_t node_count() const;
			// Counts the total number of mesh references in the tree.
			size_t instance_count() const;
			// For this node, it will calculate the model transform for a node at a particular time.
			// It uses the node's animation data to calculate the transform.
			math::mat4<float> global_transform(double aTime);
			// Gathers a list of references to MeshInstance objects.
			std::vector<mesh::instance*> gather_mesh_instances();

		private:

			void set_root(node* aRoot);

			void clear();

			void zero_out();

		};



		// --------------- Aggregate Model Resources --------------- //

		// Node Hierarchy
		std::string							Name;
		node								Hierarchy;			// Root Node Hierarchy 
		double 								Time;

		// Resources
		gcl::context*						Context;
		std::vector<mesh> 					Mesh;
		std::vector<material> 				Material;
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
