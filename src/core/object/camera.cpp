#include <geodesuka/core/object/camera.h>

#include <math.h>

#include <vector>

#include <geodesuka/core/math.h>

//#include "../hid/mouse.h"
//#include "../hid/keyboard.h"
//#include "../hid/joystick.h"

//#define PI 3.14f

// x = r*sin(Theta)*cos(Phi), y = r*sin(Theta)*sin(Phi), z = r*cos(Theta)

namespace geodesuka::core::object {

	camera::~camera() {

	}

	//void camera::draw(object_t* aObject) {}

	camera::camera(gcl::context* aContext, stage_t* aStage, const char* aName, math::vec3<uint> aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount) : 
		render_target(aContext, aStage, aName, aFrameResolution, aFrameRate, aFrameCount, aAttachmentCount) 
	{

	}

	/*
	camera::camera() {

		Translation = math::real4x4::I;
		Rotation = math::real4x4::I;
		Projection = math::real4x4::I;

		Time = 0.0f;
		Position = math::real3(0.0f, 0.0f, 5.0f);
		Mass = 1.0f;
		Momentum = math::real3(0.0f, 0.0f, 0.0f);
		Force = math::real3(0.0f, 0.0f, 0.0f);

		Theta = math::constant::pi;
		Phi = 0.0f;
		Right = math::real3(sin(Phi), -cos(Phi), 0.0f);
		Up = math::real3(-cos(Phi) * cos(Theta), -sin(Phi) * cos(Theta), sin(Theta));
		Forward = math::real3(cos(Phi) * sin(Theta), sin(Phi) * sin(Theta), cos(Theta));


		FieldOfView = 74.0f;
		AspectRatio = 1.0f;
		MinDistance = 10.0f;
		MaxDistance = 1000.0f;

		RotationalVelocity = 1.0f;
		MovementVelocity = 1.0f;
	}

	camera::~camera() {

	}

	math::integer camera::input(const hid::keyboard& aKeyboard) {
		//MoveDirection[MOVE_FORWARD] = Keyboard[keyboard::KEY_W];
		//MoveDirection[MOVE_LEFT] = Keyboard[keyboard::KEY_A];
		//MoveDirection[MOVE_BACK] = Keyboard[keyboard::KEY_S];
		//MoveDirection[MOVE_RIGHT] = Keyboard[keyboard::KEY_D];
		//MoveDirection[MOVE_DOWN] = Keyboard[keyboard::KEY_Q];
		//MoveDirection[MOVE_UP] = Keyboard[keyboard::KEY_E];

		return 0;
	}

	math::integer camera::input(const hid::mouse& aMouse) {
		return 0;
	}

	integer camera::update(real Dt) {
		Time += Dt;

		//Theta = (Mouse.Position.y - 1.0f) * constant::pi / 2.0f;
		//Phi = -Mouse.Position.x * 2.0f * constant::pi;

		if (Theta > constant::pi) {
			Theta = constant::pi;
		}
		else if (Theta < 0.0f) {
			Theta = 0.0f;
		}

		if (Phi > 2.0f * constant::pi) {
			Phi -= 2.0f * constant::pi;
		}
		else if (Phi < 0.0f) {
			Phi += 2.0f * constant::pi;
		}

		Right = real3(sin(Phi), -cos(Phi), 0.0f);
		Up = real3(-cos(Phi) * cos(Theta), -sin(Phi) * cos(Theta), sin(Theta));
		Forward = real3(cos(Phi) * sin(Theta), sin(Phi) * sin(Theta), cos(Theta));

		Rotation = //glm::transpose(
			real4x4(
				Right.x, Right.y, Right.z, 0.0f,
				Up.x, Up.y, Up.z, 0.0f,
				Forward.x, Forward.y, Forward.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				//)
			);

		//Rotation = glm::transpose(
		//	math::mat4<float>(
		//		sin(Phi),				-cos(Phi),				0.0f,			0.0f,
		//		-cos(Theta)*cos(Phi),	-cos(Theta)*sin(Phi),	sin(Theta),		0.0f,
		//		sin(Theta)*cos(Phi),	sin(Theta)*sin(Phi),	cos(Theta),		0.0f,
		//		0.0f,					0.0f,					0.0f,			1.0f
		//	)
		//);

		//Projection = glm::perspective(FieldOfView, AspectRatio, MinDistance, MaxDistance);

		Momentum += Force * Dt;
		Position += (Momentum * Dt + Force * Dt * Dt / 2.0f) / Mass;

		if (MoveDirection[MOVE_FORWARD]) {
			Position += MovementVelocity * Forward * Dt;
		}
		if (MoveDirection[MOVE_LEFT]) {
			Position -= MovementVelocity * Right * Dt;
		}
		if (MoveDirection[MOVE_BACK]) {
			Position -= MovementVelocity * Forward * Dt;
		}
		if (MoveDirection[MOVE_RIGHT]) {
			Position += MovementVelocity * Right * Dt;
		}
		if (MoveDirection[MOVE_DOWN]) {
			Position -= MovementVelocity * Up * Dt;
		}
		if (MoveDirection[MOVE_UP]) {
			Position += MovementVelocity * Up * Dt;
		}

		return 0;
	}

	math::integer camera::draw(system_display* SystemDisplay) {
		return math::integer();
	}

	math::integer camera::draw(system_window* SystemWindow) {
		return math::integer();
	}

	math::integer camera::draw(virtual_window* VirtualWindow) {
		return math::integer();
	}

	math::integer camera::draw(camera* Camera) {
		return math::integer();
	}

	math::integer camera::draw(object* Object) {
		if ((object*)this == Object) return -1;
		return Object->draw(this);
	}




	//math::real4x4 camera::get_transform(math::real3 p) const {
	//
	//    // This generates the perspective matrix.
	//	math::real4x4 Pers = math::perspective(this->FOV, (double)this->Width / (double)this->Height, 1.0, 100.0);
	//
	//	// This rotates the vertex to the camera.
	//	math::real4x4 Rot = math::real4x4(
	//		this->Right.x,		this->Right.y,		this->Right.z,		0.0,
	//		this->Up.x,			this->Up.y,			this->Up.z,			0.0,
	//		this->Forward.x,	this->Forward.y,	this->Forward.z,	0.0,
	//		0.0,				0.0,				0.0,				1.0
	//	);
	//
	//	// This translates the vertex to the camera coordinate space.
	//	math::real4x4 Trans = math::real4x4(
	//		1.0, 0.0, 0.0, (p.x-this->Pos.x),
	//		0.0, 1.0, 0.0, (p.y-this->Pos.y),
	//		0.0, 0.0, 1.0, (p.z-this->Pos.z),
	//		0.0, 0.0, 0.0, 1.0
	//	);
	//
	//    return (Pers * Rot * Trans);
	//}

	//*/

}
