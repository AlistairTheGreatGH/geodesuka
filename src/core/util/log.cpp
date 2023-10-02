#include <geodesuka/core/util/log.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#include <glslang/Include/arrays.h>
#include <glslang/Include/BaseTypes.h>
#include <glslang/Include/Common.h>
#include <glslang/Include/ConstantUnion.h>
#include <glslang/Include/intermediate.h>
#include <glslang/Include/PoolAlloc.h>
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Include/SpirvIntrinsics.h>
#include <glslang/Include/Types.h>

#include <glslang/MachineIndependent/localintermediate.h>

#include <glslang/Public/ShaderLang.h>

// Converts shader source into SPIRV.
//#include <glslang/SPIRV/SpvTools.h>
//#include <glslang/SPIRV/Logger.h>
#include <glslang/SPIRV/GlslangToSpv.h>
//#include <glslang/SPIRV/spirv.hpp>
//#include <glslang/SPIRV/spvIR.h>
//#include <glslang/SPIRV/SPVRemapper.h>

// Included for compiling
#include "../gcl/ResourceLimits.h"

// Font Loading
#include <ft2build.h>
#include FT_FREETYPE_H

//// Image Loading
//#define FREEIMAGE_LIB
//#include <FreeImage.h>

// Model Loading
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//// System Audio Interface
//#include <portaudio.h>

// System Window Interface
#include <GLFW/glfw3.h>

//FT_Error;
//PaError

#define LOG_CHUNK_SIZE 1024

int calculate_chunk_count(int aMessageCount) {
	return (((aMessageCount - (aMessageCount % LOG_CHUNK_SIZE)) / LOG_CHUNK_SIZE) + 1);
}

namespace geodesuka::core::util {

	const char* log::message::type_to_string(type aType) {
		switch (aType) {
		case INFO:		return "Info";
		case WARNING:	return "Warning";
		case ERROR:		return "Error";
		default: 		return "";
		}
	}

	const char* log::message::code_to_string(code aCode) {
		switch (aCode) {
		case UNKNOWN											: return "Unknown";
		case SUCCESS											: return "Success";
		case NOT_INITIALIZED									: return "not initialized";
		case UNANTICIPATED_HOST_ERROR							: return "unanticipated host error";
		case INVALID_CHANNEL_COUNT								: return "invalid channel count";
		case INVALID_SAMPLE_RATE								: return "invalid sample rate";
		case INVALID_DEVICE										: return "invalid device";
		case INVALID_FLAG										: return "invalid flag";
		case SAMPLE_FORMAT_NOT_SUPPORTED						: return "sample format not supported";
		case BAD_IO_DEVICE_COMBINATION							: return "bad io device combination";
		case INSUFFICIENT_MEMORY								: return "insufficient memory";
		case BUFFER_TOO_BIG										: return "buffer too big";
		case BUFFER_TOO_SMALL									: return "buffer too small";
		case NULL_CALLBACK										: return "null callback";
		case BAD_STREAM_PTR										: return "bad stream ptr";
		case TIMED_OUT											: return "timed out";
		case INTERNAL_ERROR										: return "internal error";
		case DEVICE_UNAVAILABLE									: return "device unavailable";
		case INCOMPATIBLE_HOST_API_SPECIFIC_STREAM_INFO			: return "incompatible host api specific stream info";
		case STREAM_IS_STOPPED									: return "stream is stopped";
		case STREAM_IS_NOT_STOPPED								: return "stream is not stopped";
		case INPUT_OVERFLOWED									: return "input overflowed";
		case OUTPUT_UNDERFLOWED									: return "output underflowed";
		case HOST_API_NOT_FOUND									: return "host api not found";
		case INVALID_HOST_API									: return "invalid host api";
		case CAN_NOT_READ_FROM_A_CALLBACK_STREAM				: return "can not read from a callback stream";
		case CAN_NOT_WRITE_TO_A_CALLBACK_STREAM					: return "can not write to a callback stream";
		case CAN_NOT_READ_FROM_AN_OUTPUT_ONLY_STREAM			: return "can not read from an output only stream";
		case CAN_NOT_WRITE_TO_AN_INPUT_ONLY_STREAM				: return "can not write to an input only stream";
		case INCOMPATIBLE_STREAM_HOST_API						: return "incompatible stream host api";
		case BAD_BUFFER_PTR										: return "bad buffer ptr";
		case NO_CURRENT_CONTEXT									: return "no current context";
		case INVALID_ENUM										: return "invalid enum";
		case INVALID_VALUE										: return "invalid value";
		case OUT_OF_MEMORY										: return "out of memory";
		case API_UNAVAILABLE									: return "api unavailable";
		case VERSION_UNAVAILABLE								: return "version unavailable";
		case PLATFORM_ERROR										: return "platform error";
		case FORMAT_UNAVAILABLE									: return "format unavailable";
		case NO_WINDOW_CONTEXT									: return "no window context";
		case NOT_READY											: return "not ready";
		case TIMEOUT											: return "timeout";
		case EVENT_SET											: return "event set";
		case EVENT_RESET										: return "event reset";
		case INCOMPLETE											: return "incomplete";
		case OUT_OF_HOST_MEMORY 								: return "out of host memory";
		case OUT_OF_DEVICE_MEMORY 								: return "out of device memory";
		case INITIALIZATION_FAILED 								: return "initialization failed";
		case DEVICE_LOST 										: return "device lost";
		case MEMORY_MAP_FAILED 									: return "memory map failed";
		case LAYER_NOT_PRESENT 									: return "layer not present";
		case EXTENSION_NOT_PRESENT 								: return "extension not present";
		case FEATURE_NOT_PRESENT 								: return "feature not present";
		case INCOMPATIBLE_DRIVER 								: return "incompatible driver";
		case TOO_MANY_OBJECTS									: return "too many objects";
		case FORMAT_NOT_SUPPORTED								: return "format not supported";
		case FRAGMENTED_POOL									: return "fragmented pool";
		case OUT_OF_POOL_MEMORY 								: return "out of pool memory";
		case INVALID_EXTERNAL_HANDLE 							: return "invalid external handle";
		case FRAGMENTATION 										: return "fragmentation";
		case INVALID_OPAQUE_CAPTURE_ADDRESS 					: return "invalid opaque capture address";
		case PIPELINE_COMPILE_REQUIRED							: return "pipeline compile required";
		case SURFACE_LOST 										: return "surface lost";
		case NATIVE_WINDOW_IN_USE 								: return "native window in use";
		case SUBOPTIMAL											: return "suboptimal";
		case OUT_OF_DATE 										: return "out of date";
		case INCOMPATIBLE_DISPLAY 								: return "incompatible display";
		case VALIDATION_FAILED									: return "validation failed";
		case INVALID_SHADER 									: return "invalid shader";
		case INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT			: return "invalid drm format modifier plane layout";
		case NOT_PERMITTED 										: return "not permitted";
		case FULL_SCREEN_EXCLUSIVE_MODE_LOST					: return "full screen exclusive mode lost";
		case THREAD_IDLE										: return "thread idle";
		case THREAD_DONE										: return "thread done";
		case OPERATION_DEFERRED									: return "operation deferred";
		case OPERATION_NOT_DEFERRED								: return "operation not deferred";
		case COMPRESSION_EXHAUSTED								: return "compression exhausted ";
		default													: return "";
		}
	}

	const char* log::message::api_to_string(api aAPI) {
		switch (aAPI) {
		case GEODESUKA:		return "geodesuka ";
		case GLSLANG: 		return "glslang   ";
		case FREETYPE: 		return "FreeType  ";
		case FREEIMAGE: 	return "FreeImage ";
		case ASSIMP: 		return "Assimp    ";
		case PORTAUDIO: 	return "PortAudio ";
		case GLFW: 			return "GLFW      ";
		case VULKAN: 		return "Vulkan    ";
		default:			return "          ";
		}
	}

	log::message::message(const char* aObjectName, type aType, code aCode, const char* aContent) {
		this->zero_out();
		this->Type					= aType;
		this->Code					= aCode;
		this->ObjectName			= aObjectName;
		size_t Size = strlen(aContent);
		if (Size < LOG_MESSAGE_CONTENT_SIZE) {
			memcpy(this->Content, aContent, Size*sizeof(char));
		} else {
			memcpy(this->Content, aContent, (LOG_MESSAGE_CONTENT_SIZE - 1)*sizeof(char));
		}
	}

	log::message::message(type aType, code aCode, const char* aPhase, api aAPI, const char* aObjectType, const char* aObjectName, const char* aContent) {
		this->zero_out();
		this->Type					= aType;
		this->Code					= aCode;
		this->Phase					= aPhase;
		this->API					= aAPI;
		this->ObjectType			= aObjectType;
		this->ObjectName			= aObjectName;
		size_t Size = strlen(aContent);
		if (Size < LOG_MESSAGE_CONTENT_SIZE) {
			memcpy(this->Content, aContent, Size*sizeof(char));
		} else {
			memcpy(this->Content, aContent, (LOG_MESSAGE_CONTENT_SIZE - 1)*sizeof(char));
		}
	}

	log::message::message(int aAPICode, const char* aPhase, api aAPI, const char* aObjectType, const char* aObjectName, const char* aContent) {
		this->zero_out();
		this->infer_code(aAPI, aAPICode);
	}

	void log::message::zero_out() {
		this->Type					= log::INFO;
		this->Code					= log::SUCCESS;
		this->Phase					= "";
		this->API					= log::GEODESUKA;
		this->ThreadID				= std::this_thread::get_id();
		//this->CallStack[64]		;
		this->ObjectType			= "";
		this->ObjectName			= "";
		memset(this->Content, 0x00, sizeof(this->Content));
	}

	void log::message::infer_code(api aAPI, int aAPICode) {
		switch (aAPI) {
		case GEODESUKA:
			break;
		case GLSLANG:
			break;
		case FREETYPE:
			break;
		case FREEIMAGE:
			break;
		case ASSIMP:
			break;
		//case PORTAUDIO:
		//	this->Type = log::ERROR;
		//	switch (aAPICode) {
		//	case paNoError:
		//		this->Type = INFO;
		//		this->Code = SUCCESS;
		//		break;
		//	case paNotInitialized:
		//		this->Code = NOT_INITIALIZED;
		//		break;
		//	case paUnanticipatedHostError:
		//		this->Code = UNANTICIPATED_HOST_ERROR;
		//		break;
		//	case paInvalidChannelCount:
		//		this->Code = INVALID_CHANNEL_COUNT;
		//		break;
		//	case paInvalidSampleRate:
		//		this->Code = INVALID_SAMPLE_RATE;
		//		break;
		//	case paInvalidDevice:
		//		this->Code = INVALID_DEVICE;
		//		break;
		//	case paInvalidFlag:
		//		this->Code = INVALID_FLAG;
		//		break;
		//	case paSampleFormatNotSupported:
		//		this->Code = SAMPLE_FORMAT_NOT_SUPPORTED;
		//		break;
		//	case paBadIODeviceCombination:
		//		this->Code = BAD_IO_DEVICE_COMBINATION;
		//		break;
		//	case paInsufficientMemory:
		//		this->Code = INSUFFICIENT_MEMORY;
		//		break;
		//	case paBufferTooBig:
		//		this->Code = BUFFER_TOO_BIG;
		//		break;
		//	case paBufferTooSmall:
		//		this->Code = BUFFER_TOO_SMALL;
		//		break;
		//	case paNullCallback:
		//		this->Code = NULL_CALLBACK;
		//		break;
		//	case paBadStreamPtr:
		//		this->Code = BAD_STREAM_PTR;
		//		break;
		//	case paTimedOut:
		//		this->Code = TIMED_OUT;
		//		break;
		//	case paInternalError:
		//		this->Code = INTERNAL_ERROR;
		//		break;
		//	case paDeviceUnavailable:
		//		this->Code = DEVICE_UNAVAILABLE;
		//		break;
		//	case paIncompatibleHostApiSpecificStreamInfo:
		//		this->Code = INCOMPATIBLE_HOST_API_SPECIFIC_STREAM_INFO;
		//		break;
		//	case paStreamIsStopped:
		//		this->Code = STREAM_IS_STOPPED;
		//		break;
		//	case paStreamIsNotStopped:
		//		this->Code = STREAM_IS_NOT_STOPPED;
		//		break;
		//	case paInputOverflowed:
		//		this->Code = INPUT_OVERFLOWED;
		//		break;
		//	case paOutputUnderflowed:
		//		this->Code = OUTPUT_UNDERFLOWED;
		//		break;
		//	case paHostApiNotFound:
		//		this->Code = HOST_API_NOT_FOUND;
		//		break;
		//	case paInvalidHostApi:
		//		this->Code = INVALID_HOST_API;
		//		break;
		//	case paCanNotReadFromACallbackStream:
		//		this->Code = CAN_NOT_READ_FROM_A_CALLBACK_STREAM;
		//		break;
		//	case paCanNotWriteToACallbackStream:
		//		this->Code = CAN_NOT_WRITE_TO_A_CALLBACK_STREAM;
		//		break;
		//	case paCanNotReadFromAnOutputOnlyStream:
		//		this->Code = CAN_NOT_READ_FROM_AN_OUTPUT_ONLY_STREAM;
		//		break;
		//	case paCanNotWriteToAnInputOnlyStream:
		//		this->Code = CAN_NOT_WRITE_TO_AN_INPUT_ONLY_STREAM;
		//		break;
		//	case paIncompatibleStreamHostApi:
		//		this->Code = INCOMPATIBLE_STREAM_HOST_API;
		//		break;
		//	case paBadBufferPtr:
		//		this->Code = BAD_BUFFER_PTR;
		//		break;
		//	}
		//	break;
		case GLFW:
			this->Type = log::ERROR;
			switch (aAPICode) {
			case GLFW_NOT_INITIALIZED:
				this->Code = NOT_INITIALIZED;
				break;
			case GLFW_NO_CURRENT_CONTEXT:
				this->Code = NO_CURRENT_CONTEXT;
				break;
			case GLFW_INVALID_ENUM:
				this->Code = INVALID_ENUM;
				break;
			case GLFW_INVALID_VALUE:
				this->Code = INVALID_VALUE;
				break;
			case GLFW_OUT_OF_MEMORY:
				this->Code = OUT_OF_MEMORY;
				break;
			case GLFW_API_UNAVAILABLE:
				this->Code = API_UNAVAILABLE;
				break;
			case GLFW_VERSION_UNAVAILABLE:
				this->Code = VERSION_UNAVAILABLE;
				break;
			case GLFW_PLATFORM_ERROR:
				this->Code = PLATFORM_ERROR;
				break;
			case GLFW_FORMAT_UNAVAILABLE:
				this->Code = FORMAT_UNAVAILABLE;
				break;
			case GLFW_NO_WINDOW_CONTEXT:
				this->Code = NO_WINDOW_CONTEXT;
				break;
			default:
				this->Code = UNKNOWN;
				break;
			}
			break;
		case VULKAN:
			switch (aAPICode) {
			case VK_SUCCESS:
				this->Type = INFO;
				this->Code = SUCCESS;
				break;
			case VK_NOT_READY:
				this->Type = INFO;
				this->Code = NOT_READY;
				break;
			case VK_TIMEOUT:
				this->Type = INFO;
				this->Code = TIMEOUT;
				break;
			case VK_EVENT_SET:
				this->Type = INFO;
				this->Code = EVENT_SET;
				break;
			case VK_EVENT_RESET:
				this->Type = INFO;
				this->Code = EVENT_RESET;
				break;
			case VK_INCOMPLETE:
				this->Type = INFO;
				this->Code = INCOMPLETE;
				break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				this->Type = ERROR;
				this->Code = OUT_OF_HOST_MEMORY;
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				this->Type = ERROR;
				this->Code = OUT_OF_DEVICE_MEMORY;
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				this->Type = ERROR;
				this->Code = INITIALIZATION_FAILED;
				break;
			case VK_ERROR_DEVICE_LOST:
				this->Type = ERROR;
				this->Code = DEVICE_LOST;
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				this->Type = ERROR;
				this->Code = MEMORY_MAP_FAILED;
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				this->Type = ERROR;
				this->Code = LAYER_NOT_PRESENT;
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				this->Type = ERROR;
				this->Code = EXTENSION_NOT_PRESENT;
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				this->Type = ERROR;
				this->Code = FEATURE_NOT_PRESENT;
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				this->Type = ERROR;
				this->Code = INCOMPATIBLE_DRIVER;
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				this->Type = ERROR;
				this->Code = TOO_MANY_OBJECTS;
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				this->Type = ERROR;
				this->Code = FORMAT_NOT_SUPPORTED;
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				this->Type = ERROR;
				this->Code = FRAGMENTED_POOL;
				break;
			case VK_ERROR_UNKNOWN:
				this->Type = ERROR;
				this->Code = UNKNOWN;
				break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				this->Type = ERROR;
				this->Code = OUT_OF_POOL_MEMORY;
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				this->Type = ERROR;
				this->Code = INVALID_EXTERNAL_HANDLE;
				break;
			case VK_ERROR_FRAGMENTATION:
				this->Type = ERROR;
				this->Code = FRAGMENTATION;
				break;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				this->Type = ERROR;
				this->Code = INVALID_OPAQUE_CAPTURE_ADDRESS;
				break;
			case VK_PIPELINE_COMPILE_REQUIRED:
				this->Type = INFO;
				this->Code = PIPELINE_COMPILE_REQUIRED;
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				this->Type = ERROR;
				this->Code = SURFACE_LOST;
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				this->Type = ERROR;
				this->Code = NATIVE_WINDOW_IN_USE;
				break;
			case VK_SUBOPTIMAL_KHR:
				this->Type = INFO;
				this->Code = SUBOPTIMAL;
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				this->Type = ERROR;
				this->Code = OUT_OF_DATE;
				break;
			case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
				this->Type = ERROR;
				this->Code = INCOMPATIBLE_DISPLAY;
				break;
			case VK_ERROR_VALIDATION_FAILED_EXT:
				this->Type = ERROR;
				this->Code = VALIDATION_FAILED;
				break;
			case VK_ERROR_INVALID_SHADER_NV:
				this->Type = ERROR;
				this->Code = INVALID_SHADER;
				break;
			case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
				this->Type = ERROR;
				this->Code = INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT;
				break;
			case VK_ERROR_NOT_PERMITTED_KHR:
				this->Type = ERROR;
				this->Code = NOT_PERMITTED;
				break;
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
				this->Type = ERROR;
				this->Code = FULL_SCREEN_EXCLUSIVE_MODE_LOST;
				break;
			case VK_THREAD_IDLE_KHR:
				this->Type = INFO;
				this->Code = THREAD_IDLE;
				break;
			case VK_THREAD_DONE_KHR:
				this->Type = INFO;
				this->Code = THREAD_DONE;
				break;
			case VK_OPERATION_DEFERRED_KHR:
				this->Type = INFO;
				this->Code = OPERATION_DEFERRED;
				break;
			case VK_OPERATION_NOT_DEFERRED_KHR:
				this->Type = INFO;
				this->Code = OPERATION_NOT_DEFERRED;
				break;
			case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
				this->Type = ERROR;
				this->Code = COMPRESSION_EXHAUSTED;
				break;
			}
			break;
		default:
			break;
		}
	}

	log::log() {
		this->zero_out();
		this->create(0, NULL);
	}

	log::log(message aMessage) {
		message MessageList[1] = {aMessage};
		this->zero_out();
		this->create(1, MessageList);
	}

	log::log(const log& aInput) {
		*this = aInput;
	}

	log::~log() {
		free(this->Message);
		this->Message = NULL;
		this->MessageCount = 0;
		this->Error = false;
	}

	log::message log::operator[](int aIndex) const {
		return this->Message[aIndex];
	}

	void log::operator<<(log aNewLog) {
		int TotalMessage = this->MessageCount + aNewLog.MessageCount;
		if (calculate_chunk_count(this->MessageCount) != calculate_chunk_count(TotalMessage)) {
			this->Message = (message*)realloc(this->Message, calculate_chunk_count(TotalMessage) * LOG_CHUNK_SIZE * sizeof(message));
			// If allocation fails.
			if (this->Message == NULL)
				throw log::message(typeid(*this).name(), type::ERROR, code::OUT_OF_HOST_MEMORY, "Logging Aggregation Failure!");
		}
		// Load new messages
		for (int i = this->MessageCount; i < TotalMessage; i++) {
			this->Message[i] = aNewLog.Message[i - this->MessageCount];
		}
		this->MessageCount = TotalMessage;
	}

	void log::operator<<(message aNewMessage) {
		*this << log(aNewMessage);
	}

	log& log::operator=(const log& aRhs) {
		if (this == &aRhs) return *this;
		int LhsChunkSize = calculate_chunk_count(this->MessageCount); 
		int RhsChunkSize = calculate_chunk_count(aRhs.MessageCount);
		if (LhsChunkSize != RhsChunkSize) {
			this->Message = (message*)realloc(this->Message, calculate_chunk_count(aRhs.MessageCount) * LOG_CHUNK_SIZE * sizeof(message));
			if (this->Message) {
				throw log::message("log", type::ERROR, code::OUT_OF_HOST_MEMORY, "Copy Assignment Operator Failure!");
			}
		}
		// Copy new data over.
		memcpy(this->Message, aRhs.Message, aRhs.MessageCount * sizeof(message));
		if (aRhs.MessageCount > this->MessageCount) {
			memset(this->Message, 0x00, (RhsChunkSize - aRhs.MessageCount) * sizeof(message));
		}
		this->MessageCount = aRhs.MessageCount;
		return *this;
	}

	// log& log::operator=(log&& aRhs) noexcept {
	// 	this->ErrorDetected 	= aRhs.ErrorDetected;
	// 	this->MessageCount 		= aRhs.MessageCount;
	// 	this->Message 			= aRhs.Message;
	// 	aRhs.ErrorDetected 		= false;
	// 	aRhs.MessageCount 		= 0;
	// 	aRhs.Message 			= NULL;
	// 	return *this;
	// }

	bool log::error_detected() const {
		return this->Error;
	}

	int log::count() const {
		return this->MessageCount;
	}

	void log::zero_out() {
		this->Error = false;
		this->MessageCount = 0;
		this->Message = NULL;
	}

	void log::create(int aMessageCount, message* aMessage) {
		this->Error = false;
		this->MessageCount = aMessageCount;
		this->Message = (message*)malloc(calculate_chunk_count(aMessageCount) * LOG_CHUNK_SIZE * sizeof(message));
		if (this->Message == NULL)
			throw log::message("log", type::ERROR, code::OUT_OF_HOST_MEMORY, "Creation Failed");
		memset(this->Message, 0x00, calculate_chunk_count(aMessageCount) * LOG_CHUNK_SIZE * sizeof(message));
		for (int i = 0; i < aMessageCount; i++) {
			this->Message[i] = aMessage[i];
			if (aMessage[i].Type == type::ERROR) {
				this->Error = true;
			}
		}
	}

	const char* log::get_er_str(VkResult Res) {
		const char* temp;
		switch (Res) {
		default:
			temp = "Error: Unknown";
			break;
		case VK_SUCCESS:
			temp = "Success";
			break;
		case VK_NOT_READY:
			temp = "Not Ready.";
			break;
		case VK_TIMEOUT:
			temp = "Timeout.";
			break;
		case VK_EVENT_SET:
			temp = "Event Set.";
			break;
		case VK_EVENT_RESET:
			temp = "Event Reset.";
			break;
		case VK_INCOMPLETE:
			temp = "Incomplete.";
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			temp = "Error: Out of Host Memory.";
			break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			temp = "Error: Out of Device Memory.";
			break;
		case VK_ERROR_INITIALIZATION_FAILED:
			temp = "Error: Initialization failed.";
			break;
		case VK_ERROR_DEVICE_LOST:
			temp = "Error: Device Lost";
			break;
		case VK_ERROR_MEMORY_MAP_FAILED:
			temp = "Error: Memory Map Failed.";
			break;
		case VK_ERROR_LAYER_NOT_PRESENT:
			temp = "Error: Layer Not Present.";
			break;
		case VK_ERROR_EXTENSION_NOT_PRESENT:
			temp = "Error: Extension not present.";
			break;
		case VK_ERROR_FEATURE_NOT_PRESENT:
			temp = "Error: Feature Not Present.";
			break;
		case VK_ERROR_INCOMPATIBLE_DRIVER:
			temp = "Error: Incompatible Driver.";
			break;
		case VK_ERROR_TOO_MANY_OBJECTS:
			temp = "Error: Too many objects.";
			break;
		case VK_ERROR_FORMAT_NOT_SUPPORTED:
			temp = "Error: Format not supported.";
			break;
		case VK_ERROR_FRAGMENTED_POOL:
			temp = "Error: Fragmented pool.";
			break;
		case VK_ERROR_UNKNOWN:
			temp = "Error: Unknown.";
			break;
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			temp = "Error: Out of pool memory.";
			break;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE:
			temp = "Error: Invalid external handle.";
			break;
		case VK_ERROR_FRAGMENTATION:
			temp = "Error: Fragmentation.";
			break;
		case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			temp = "Error: Invalid opaque capture address.";
			break;
		case VK_ERROR_SURFACE_LOST_KHR:
			temp = "Error: Surface lost (KHR)";
			break;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			temp = "Error: Native window in use (KHR)";
			break;
		case VK_SUBOPTIMAL_KHR:
			temp = "Suboptimal (khr)";
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			temp = "Error: Out of date (khr)";
			break;
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			temp = "Error: Incompatible display.";
			break;
		case VK_ERROR_VALIDATION_FAILED_EXT:
			temp = "Error: Validation failed (ext).";
			break;
		case VK_ERROR_INVALID_SHADER_NV:
			temp = "Error: Invalid shader (nv)";
			break;
		case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
			temp = "Error: Invalid DRM format modifier plane layout (ext)";
			break;
		case VK_ERROR_NOT_PERMITTED_EXT:
			temp = "Error: Not permitted extension";
			break;
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			temp = "Error: Fullscreen exclusive mode lost.";
			break;
		case VK_THREAD_IDLE_KHR:
			temp = "Thread Idle (khr)";
			break;
		case VK_THREAD_DONE_KHR:
			temp = "Thread Done (khr)";
			break;
		case VK_OPERATION_DEFERRED_KHR:
			temp = "Operation Deferred (khr)";
			break;
		case VK_OPERATION_NOT_DEFERRED_KHR:
			temp = "Operation not deferred (khr)";
			break;
		case VK_PIPELINE_COMPILE_REQUIRED_EXT:
			temp = "Pipeline compile required. (ext)";
			break;
		}
		return temp;
		/*
		VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR = VK_ERROR_INVALID_EXTERNAL_HANDLE,
		VK_ERROR_FRAGMENTATION_EXT = VK_ERROR_FRAGMENTATION,
		VK_ERROR_INVALID_DEVICE_ADDRESS_EXT = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,
		VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR = VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,
		VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT = VK_PIPELINE_COMPILE_REQUIRED_EXT,
		VK_RESULT_MAX_ENUM = 0x7FFFFFFF
		*/
	}

}