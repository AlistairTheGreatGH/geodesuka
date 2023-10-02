#pragma once
#ifndef GEODESUKA_CORE_UTIL_LOG_H
#define GEODESUKA_CORE_UTIL_LOG_H

#include <thread>

// Engine Configuration Options.
#include "../../config.h"

// GCL Configuration Options.
#include "../gcl/config.h"

//Context << log(log::type::ERROR, "buffer", 24, CODE, Message); 
//Engine << log(log::type::ERROR, "buffer", 24, CODE, Message);

#define LOG_MESSAGE_CONTENT_SIZE 512

namespace geodesuka::core::util {

	//typeid(*this).name();
	class log {
	public:

		enum type {
			INFO			,
			WARNING			,
			ERROR			,
		};
		
		enum code : int {
            UNKNOWN 											= -1,
			SUCCESS 											= 0,

			// FreeType
			// TODO: Register FreeType Errors Later!

			// Port Audio
			NOT_INITIALIZED										,
			UNANTICIPATED_HOST_ERROR							,
			INVALID_CHANNEL_COUNT								,
			INVALID_SAMPLE_RATE									,
			INVALID_DEVICE										,
			INVALID_FLAG										,
			SAMPLE_FORMAT_NOT_SUPPORTED							,
			BAD_IO_DEVICE_COMBINATION							,
			INSUFFICIENT_MEMORY									,
			BUFFER_TOO_BIG										,
			BUFFER_TOO_SMALL									,
			NULL_CALLBACK										,
			BAD_STREAM_PTR										,
			TIMED_OUT											,
			INTERNAL_ERROR										,
			DEVICE_UNAVAILABLE									,
			INCOMPATIBLE_HOST_API_SPECIFIC_STREAM_INFO			,
			STREAM_IS_STOPPED									,
			STREAM_IS_NOT_STOPPED								,
			INPUT_OVERFLOWED									,
			OUTPUT_UNDERFLOWED									,
			HOST_API_NOT_FOUND									,
			INVALID_HOST_API									,
			CAN_NOT_READ_FROM_A_CALLBACK_STREAM					,
			CAN_NOT_WRITE_TO_A_CALLBACK_STREAM					,
			CAN_NOT_READ_FROM_AN_OUTPUT_ONLY_STREAM				,
			CAN_NOT_WRITE_TO_AN_INPUT_ONLY_STREAM				,
			INCOMPATIBLE_STREAM_HOST_API						,
			BAD_BUFFER_PTR										,

			// GLFW Error Codes
			NO_CURRENT_CONTEXT									,
			INVALID_ENUM										,
			INVALID_VALUE										,
			OUT_OF_MEMORY										,
			API_UNAVAILABLE										,
			VERSION_UNAVAILABLE									,
			PLATFORM_ERROR										,
			FORMAT_UNAVAILABLE									,
			NO_WINDOW_CONTEXT									,

			// Vulkan Error List			
			NOT_READY											, 
			TIMEOUT												, 
			EVENT_SET											, 
			EVENT_RESET											, 
			INCOMPLETE											, 
			OUT_OF_HOST_MEMORY 									, 
			OUT_OF_DEVICE_MEMORY 								, 
			INITIALIZATION_FAILED 								, 
			DEVICE_LOST 										, 
			MEMORY_MAP_FAILED 									, 
			LAYER_NOT_PRESENT 									, 
			EXTENSION_NOT_PRESENT 								, 
			FEATURE_NOT_PRESENT 								, 
			INCOMPATIBLE_DRIVER 								, 
			TOO_MANY_OBJECTS									, 
			FORMAT_NOT_SUPPORTED								, 
			FRAGMENTED_POOL										, 
			OUT_OF_POOL_MEMORY 									, 
			INVALID_EXTERNAL_HANDLE 							, 
			FRAGMENTATION 										, 
			INVALID_OPAQUE_CAPTURE_ADDRESS 						, 
			PIPELINE_COMPILE_REQUIRED							, 
			SURFACE_LOST 										, 
			NATIVE_WINDOW_IN_USE 								, 
			SUBOPTIMAL											, 
			OUT_OF_DATE 										, 
			INCOMPATIBLE_DISPLAY 								, 
			VALIDATION_FAILED									, 
			INVALID_SHADER 										, 
			INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT			, 
			NOT_PERMITTED 										, 
			FULL_SCREEN_EXCLUSIVE_MODE_LOST						, 
			THREAD_IDLE											, 
			THREAD_DONE											, 
			OPERATION_DEFERRED									, 
			OPERATION_NOT_DEFERRED								, 
			COMPRESSION_EXHAUSTED								, 

        };

		enum api : int {
			GEODESUKA	,
			GLSLANG		,
			FREETYPE	,
			FREEIMAGE	,
			ASSIMP		,
			PORTAUDIO	,
			GLFW		,
			VULKAN		,
		};
			
		class message {
		public:

			// Info: Success|API: geodesuka|object: system_window|instance: "Main Window"|Message: "Engine Startup Failure: glslang failed to initialize!"

			static const char* type_to_string(type aType);
			static const char* code_to_string(code aCode);
			static const char* api_to_string(api aAPI);

			type 				Type;
			code 				Code;
			const char* 		Phase;
			api 				API;
			std::thread::id 	ThreadID;
			//const char* 		CallStack[64];
			const char* 		ObjectType;
			const char* 		ObjectName;
			char 				Content[LOG_MESSAGE_CONTENT_SIZE];

			message(const char* aObjectName, type aType, code aCode, const char* aContent);
			
			message(type aType, code aCode, const char* aPhase, api aAPI, const char* aObjectType, const char* aObjectName, const char* aContent);
			message(int aAPICode, const char* aPhase, api aAPI, const char* aObjectType, const char* aObjectName, const char* aContent);

		private:

			void zero_out();
			void infer_code(api aAPI, int aAPICode);
		};

		// Parse error into string.
		static const char* get_er_str(VkResult Res);

		log();
		log(message aMessage);
		log(const log& aInput);
		//log(log&& aInput) noexcept;
		~log();

		message operator[](int aIndex) const;
		void operator<<(log aNewLog);
		void operator<<(message aNewMessage);
		log& operator=(const log& aRhs);
		//log& operator=(log&& aRhs) noexcept;

		bool resize(size_t aNewCount);
		bool error_detected() const;
		int count() const;

	private:

		bool Error;
		int MessageCount;
		message *Message;

		void zero_out();
		void create(int aMessageCount, message* aMessage);

	};

}

#endif // !GEODESUKA_CORE_UTIL_LOG_H
