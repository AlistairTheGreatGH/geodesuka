#include <geodesuka/builtin/app/unit_test.h>

#include <cstring>

#include <geodesuka/engine.h>

namespace geodesuka::builtin::app {

	//using namespace core::gcl;
	//using namespace core::object;

	using namespace core;
	using namespace gcl;
	using namespace object;
	using namespace stage;

	//using namespace core;
	using namespace core::gcl;
	using namespace core::object;
	using namespace core::stage;

	unit_test::unit_test(engine* aEngine) : core::app(aEngine) {
		// Initialize everything.

		DeviceContext = nullptr;
		TimeStep.store(0.001);

	}

	unit_test::~unit_test() {
		// Clean up resources.
		//delete Context;
	}

	void unit_test::run() {

		// Needed for interaction with WSI.
		util::list<const char*> Layers;
		util::list<const char*> Extensions = util::list<const char*>(system_window::RequiredContextExtension.size(), system_window::RequiredContextExtension.data());

		// Create device context with required extensions.
		DeviceContext = new context(Engine, Engine->PrimaryDevice, Layers, Extensions);

		//this->full_test();

		//// Setup window properties.
		//system_window::create_info Property;
		//Property.Swapchain.FrameCount			= 3;
		//Property.Swapchain.FrameRate			= 60.0;
		//Property.Swapchain.ColorSpace			= swapchain::colorspace::SRGB_NONLINEAR;
		//Property.Swapchain.ImageUsage			= image::usage::COLOR_ATTACHMENT | image::usage::SAMPLED | image::usage::TRANSFER_SRC | image::usage::TRANSFER_DST;
		//Property.Swapchain.CompositeAlpha		= swapchain::composite::ALPHA_OPAQUE;
		//Property.Swapchain.PresentMode			= swapchain::present_mode::FIFO;
		//Property.Swapchain.Clipped				= true;
		//Property.Swapchain.PixelFormat			= image::format::R8G8B8A8_SRGB;
		////Property.Title							= "Unit Test";

		//// Application main window.
		//Window = new system_window(DeviceContext, Engine->Display[0], "Unit Test", Property, math::vec2<int>(0, 0), math::vec2<int>(640, 480));

		//// Create Compositor Canvas.
		//Compositor = new canvas(DeviceContext, Window);

		// Create Demo3D Stage.
		//Demo3D = new demo3d(Context);

		while (Engine->ThreadController.cycle(this->TimeStep.load())) {
			//*Engine->State.SystemTerminal << "AppThread\n";

		}

		delete Window;

		delete DeviceContext;
	}

	void unit_test::full_test() {
		// Random Unit Tests.
		this->math_unit_test();
		this->util_unit_test();
		this->logic_unit_test();
		this->io_unit_test();
		this->hid_unit_test();
		this->gcl_unit_test();
		this->object_unit_test();
		this->stage_unit_test();
		this->app_unit_test();
	}

	void unit_test::math_unit_test() {

	}

	void unit_test::util_unit_test() {
		/*
		str Str1("hello");
		str Str2 = "World";
		str Str = "The quick brown fox chased the little rabbit!";
		std::cout << "Str1 = " << Str1.Handle << std::endl;
		std::cout << "Str2 = " << Str2.Handle << std::endl;

		Str1.clear();
		Str1.push("Hel");
		Str1.push("lo");
		std::cout << "Str1 = " << Str1.Handle << std::endl;
		Str1.pop(3);
		std::cout << "Str1 = " << Str1.Handle << std::endl;
		Str1.insert(2, " is dumb");
		std::cout << "Str1 = " << Str1.Handle << std::endl;
		Str1.remove(3, 10);
		std::cout << "Str1 = " << Str1.Handle << std::endl;

		std::cout << "Str = " << Str.Handle << std::endl;
		Str.exchange(4, 10, 5);
		std::cout << "Str = " << Str.Handle << std::endl;
		Str.remove(4, 6);
		std::cout << "Str = " << Str.Handle << std::endl;
		Str.insert(10, "brown ");
		std::cout << "Str = " << Str.Handle << std::endl;
		Str.pop(8);
		std::cout << "Str = " << Str.Handle << std::endl;
		Str.push(" wabbit!");
		std::cout << "Str = " << Str.Handle << std::endl;
		str StrTemp = Str.split_at("brown");
		std::cout << "StrTemp = " << StrTemp.Handle << std::endl;
		std::cout << "Str = " << Str.Handle << std::endl;
		Str.permute(-4);
		std::cout << "Str = " << Str.Handle << std::endl;

		str Str3 = "";
		Str3.push('a');
		Str3.push('b');
		std::cout << "Str3 = " << Str3.Handle << std::endl;
		Str3.pop();
		Str3.push('\a'); // Angery computer noises
		std::cout << "Str3 = " << Str3.Handle << std::endl;
		Str3.insert(1, 'f');
		std::cout << "Str3 = " << Str3.Handle << std::endl;
		Str3.remove(0);
		std::cout << "Str3 = " << Str3.Handle << std::endl;
		Str3.push("hello");
		std::cout << "Str3 = " << Str3.Handle << std::endl;
		Str3.exchange(2, 3);
		std::cout << "Str3 = " << Str3.Handle << std::endl;

		float Simple = FLT_MAX;
		printf("Original Float:   %f\n", Simple);
		str Txt = str("%f", Simple);
		printf("String Float:    \"%s\"\n", Txt.ptr());
		float NewSimple = (float)atof(Txt.ptr());
		printf("Converted Float:  %f\n", NewSimple);

		float LeftMotor = 3.14;
		const char* Name = "Left Motor";
		str Send = str("%s|%+4.3e", Name, LeftMotor);
		printf("Send Message: \"%s\"\n", Send.ptr());
		str RecvName = Send.split_at('|');
		printf("\"%s\"\t\"%s\"\n", RecvName.ptr(), Send.ptr());
		printf("%s Value: %f\n", RecvName.ptr(), (float)atof(Send.ptr()));
		*/
	}

	void unit_test::logic_unit_test() {

	}

	void unit_test::io_unit_test() {

	}

	void unit_test::hid_unit_test() {

	}

	void unit_test::gcl_unit_test() {
		/*
		{
			type particle(type::id::STRUCT, "particle");
			particle.push(type::id::FLOAT3, "Position");
			particle.push(type::id::FLOAT3, "Momentum");
			particle.push(type::id::FLOAT3, "Force");
			particle.push(type::id::FLOAT, "Mass");

			type light(type::id::STRUCT, "light");
			light.push(type::id::FLOAT3, "Position");
			light.push(type::id::FLOAT4, "Color", 3);
			//int Array[2] = { 2, 3 };
			//light.push(type::id::REAL4, "Color", 2, Array);

			type glow(type::id::STRUCT, "glow");
			glow.push(particle, "Particle");
			glow.push(light, "Light");
			glow.push(type::id::FLOAT4X4, "Transform");
			glow.push(type::id::FLOAT3, "Position");
			glow.push(type::id::FLOAT, "Scale");

			variable Glow(glow, "Glow");
			//variable Glow(glow, "Glow");

			std::cout << Glow.get_str() << std::endl;
			std::cout << "sizeof(Glow) = " << variable::size_of(Glow) << " = " << Glow.Size << std::endl;

			type mega(type::id::STRUCT, "mega");
			mega.push(glow, "Huehue");
			mega.push(particle, "narwhals");
			mega.push(light, "ultra");
			variable Mega(mega, "Tyler");

			std::cout << Mega.get_str() << std::endl;
			std::cout << "sizeof(Tyler) = " << variable::size_of(Mega) << " = " << Mega.Size << std::endl;

			variable Test(type::id::STRUCT, "Test");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT3, "Pos");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT2, "TexCoord");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT4X4, "FakeTransform");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT3, "Normal");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT4X4, "Transform");
			std::cout << Test.get_str() << std::endl;
			Test.Type.remove(2);
			std::cout << Test.get_str() << std::endl;
			Test.Type.remove(1, 2);
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT3X2, "Var0");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT2X4, "Var1");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::UINT3, "Var2");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::INT2, "Var3");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::FLOAT, "Var4");
			std::cout << Test.get_str() << std::endl;
			Test.Type.push(type::id::INT, "Var5");
			std::cout << Test.get_str() << std::endl;
			//Test.Type.remove(0, 2);
			//std::cout << Test.get_str() << std::endl;
			variable temp(type::id::INT, "Var225");
			Test.Type.insert(5, 1, &temp);
			std::cout << Test.get_str() << std::endl;
			Test.Type.insert(6, type::id::INT, "Var250");
			std::cout << Test.get_str() << std::endl;
			Test.Type.insert(7, type::id::INT, "Var275");
			std::cout << Test.get_str() << std::endl;
			Test.Type.remove(3, 4);
			std::cout << Test.get_str() << std::endl;
			Test.Type.pop(-1);
			std::cout << Test.get_str() << std::endl;

			// Test Constructors
			type ta;
			type tb(type::id::FLOAT3X4);
			type tc(type::id::STRUCT);
			type td(type::id::STRUCT, "td");
			type te(type::id::FLOAT3X4, "te");
			//type tf(type::id::TVOID, "tf");

			//type tCopyConstructor(glow);
			//type tMoveConstructor(glow.clone());
			//type tCopyAssign = glow;
			//type tMoveAssign = glow.clone();

			//variable CopyConstructor(Glow);
			//variable MoveConstructor(Glow.clone());
			//variable CopyAssign = Glow;
			//variable MoveAssign = Glow.clone();

			//std::cout << CopyConstructor.get_str() << std::endl;
			//std::cout << MoveConstructor.get_str() << std::endl;
			//std::cout << CopyAssign.get_str() << std::endl;
			//std::cout << MoveAssign.get_str() << std::endl;
		}
		*/

		VkResult Result = VK_SUCCESS;

		buffer::create_info HostBufferCI(
			device::memory::HOST_VISIBLE | device::memory::HOST_COHERENT,
			buffer::usage::VERTEX | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST
		);

		// Memory Specification for a buffer that exists on device.
		buffer::create_info DeviceBufferCI(
			device::memory::DEVICE_LOCAL,
			buffer::usage::VERTEX | buffer::usage::TRANSFER_SRC | buffer::usage::TRANSFER_DST
		);

		image::create_info TextureCI(
			image::sample::COUNT_1,
			image::tiling::OPTIMAL,
			device::memory::DEVICE_LOCAL,
			image::usage::COLOR_ATTACHMENT | image::usage::SAMPLED | image::usage::TRANSFER_SRC | image::usage::TRANSFER_DST
		);

		{

			// -------------------- buffer.h unit testing ------------------------- //

			unsigned char BufferData[6 * 3] = {
				0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
				0x69, 0xAA, 0x69, 0xAA, 0xAA, 0x69,
				0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA,
			};

			unsigned char BufferData2[6 * 3] = {
				0x69, 0xAA, 0x69, 0xAA, 0xAA, 0x69,
				0xCC, 0xBB, 0xAA, 0xFF, 0xEE, 0xDD,
				0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
			};

			// This will be used as a return buffer.
			unsigned char ReturnData[6 * 3] = {
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			};


			// Unit Test of buffer.h and image.h
			buffer HostBuffer(DeviceContext, HostBufferCI, sizeof(BufferData), BufferData);

			buffer DeviceBuffer(DeviceContext, DeviceBufferCI, sizeof(BufferData));

			buffer ReturnBuffer(DeviceContext, HostBufferCI, sizeof(BufferData));

			// Copy To Device Buffer
			Result = DeviceBuffer.copy(0, HostBuffer, 0, sizeof(BufferData));

			// Copy To Return Buffer
			Result = ReturnBuffer.copy(0, DeviceBuffer, 0, sizeof(BufferData));

			// Gather data back from return buffer.
			Result = ReturnBuffer.read(0, ReturnData, 0, sizeof(BufferData));

			if (memcmp(BufferData, ReturnData, sizeof(BufferData)) == 0) {
				*Engine << "Buffer Data Matches, Operation Success!\n";
			}
			else {
				*Engine << "Buffer Data Does NOT Match, Operation Failure!\n";
			}

			Result = DeviceBuffer.write(0, BufferData2, 0, sizeof(BufferData));

			Result = DeviceBuffer.read(0, ReturnData, 0, sizeof(BufferData));

			if (memcmp(BufferData2, ReturnData, sizeof(BufferData)) == 0) {
				*Engine << "Buffer Data Matches, Operation Success!\n";
			}
			else {
				*Engine << "Buffer Data Does NOT Match, Operation Failure!\n";
			}
		}
		// -------------------- image.h unit testing ------------------------- //

		{

			uint TextureData[4 * 4] = {
				0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD,
				0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD,
				0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD,
				0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD, 0xAABBCCDD
			};

			uint ReturnData[4 * 4] = {
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0,
				0, 0, 0, 0
			};

			buffer StagingBuffer(DeviceContext, HostBufferCI, sizeof(TextureData), TextureData);

			buffer StagingBuffer2(DeviceContext, HostBufferCI, sizeof(TextureData));

			// Create Device Texture
			image Texture(DeviceContext, TextureCI, image::format::B8G8R8A8_SRGB, 4, 4);

			Result = Texture.transition(image::layout::SHADER_READ_ONLY_OPTIMAL, image::layout::TRANSFER_DST_OPTIMAL);

			Result = Texture.copy({ 0, 0, 0 }, 0, StagingBuffer, 0, { 4, 4, 1 });

			Result = Texture.transition(image::layout::TRANSFER_DST_OPTIMAL, image::layout::TRANSFER_SRC_OPTIMAL);

			Result = StagingBuffer2.copy(0, Texture, { 0, 0, 0 }, 0, { 4, 4, 1 });

			StagingBuffer2.read(0, ReturnData, 0, sizeof(TextureData));

			if (memcmp(TextureData, ReturnData, sizeof(TextureData)) == 0) {
				*Engine << "Texture Data Matches, Operation Success!\n";
			}
			else {
				*Engine << "Texture Data does NOT Match, Operation Failure!\n";
			}

			image SecondTexture(DeviceContext, TextureCI, image::format::B8G8R8A8_SRGB, 4, 4);

			//Result = DeviceTexture.transition(image::layout::SHADER_READ_ONLY_OPTIMAL, image::layout::TRANSFER_SRC_OPTIMAL);

			Result = SecondTexture.transition(image::layout::SHADER_READ_ONLY_OPTIMAL, image::layout::TRANSFER_DST_OPTIMAL);

			Result = SecondTexture.copy({ 0, 0, 0 }, 0, Texture, { 0, 0, 0 }, 0, { 4, 4, 1 });

			Result = Texture.transition(image::layout::TRANSFER_SRC_OPTIMAL, image::layout::SHADER_READ_ONLY_OPTIMAL);

			Result = SecondTexture.transition(image::layout::TRANSFER_DST_OPTIMAL, image::layout::TRANSFER_SRC_OPTIMAL);

			memset(ReturnData, 0x00, sizeof(TextureData));

			Result = SecondTexture.read({ 0, 0, 0 }, 0, ReturnData, 0, { 4, 4, 1 });

			if (memcmp(TextureData, ReturnData, sizeof(TextureData)) == 0) {
				*Engine << "Texture Data Matches, Operation Success!\n";
			}
			else {
				*Engine << "Texture Data does NOT Match, Operation Failure!\n";
			}

		}

		{
			
		}
	}

	void unit_test::object_unit_test() {

	}

	void unit_test::stage_unit_test() {

	}

	void unit_test::app_unit_test() {

	}

}
