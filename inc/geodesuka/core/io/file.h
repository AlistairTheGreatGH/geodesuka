#pragma once
#ifndef GEODESUKA_CORE_IO_FILE_H
#define GEODESUKA_CORE_IO_FILE_H

#include <stdlib.h>

#include <vector>

#include "../util/string.h"

/*
Maybe this should maintain of all accessed files and directories
throughout its life.
*/

/*
* 
* Extension: A file may have an extension of what type of file it
* may be specifically.
*/

namespace geodesuka::core::io {

	/*
	* This can be seen as a simple file handle type, it will be the responsibility
	* of the game engine to insure that identical files are not loaded in twice.
	*/

	class file {
	public:

		friend class file_manager;

		enum extension {
			EID_UNKNOWN = 0,
			// --------------- Dynamic Libraries --------------- //
			DYNALIB_DYN,

			// --------------- Type Face Files --------------- //
			FONT_TTF,
			FONT_TTC,
			FONT_OTF,
			FONT_PFM,

			// --------------- Image Files --------------- //
			IMAGE_BMP,
			IMAGE_ICO,
			IMAGE_JPEG,
			IMAGE_JNG,
			IMAGE_KOALA,
			IMAGE_LBM,
			IMAGE_IFF,
			IMAGE_MNG,
			IMAGE_PBM,
			IMAGE_PBMRAW,
			IMAGE_PCD,
			IMAGE_PCX,
			IMAGE_PGM,
			IMAGE_PGMRAW,
			IMAGE_PNG,
			IMAGE_PPM,
			IMAGE_PPMRAW,
			IMAGE_RAS,
			IMAGE_TARGA,
			IMAGE_TIFF,
			IMAGE_WBMP,
			IMAGE_PSD,
			IMAGE_CUT,
			IMAGE_XBM,
			IMAGE_XPM,
			IMAGE_DDS,
			IMAGE_GIF,
			IMAGE_HDR,
			IMAGE_FAXG3,
			IMAGE_SGI,
			IMAGE_EXR,
			IMAGE_J2K,
			IMAGE_JP2,
			IMAGE_PFM,
			IMAGE_PICT,
			IMAGE_RAW,
			IMAGE_WEBP,
			IMAGE_JXR,

			// --------------- Model Files --------------- //
			MODEL_3MF,
			MODEL_DAE,
			MODEL_BVH,
			MODEL_3DS,
			MODEL_ASE,
			MODEL_GLTF,
			MODEL_FBX,
			MODEL_PLY,
			MODEL_DXF,
			MODEL_IFC,
			MODEL_NFF,
			MODEL_SMD,
			MODEL_VTA,
			MODEL_MDL,
			MODEL_MD2,
			MODEL_MD3,
			MODEL_PK3,
			MODEL_MDC,
			MODEL_MD5MESH,
			MODEL_MD5ANIM,
			MODEL_MD5CAMERA,
			MODEL_X,
			MODEL_Q3O,
			MODEL_Q3S,
			MODEL_AC,
			MODEL_AC3D,
			MODEL_STL,
			MODEL_IRRMESH,
			MODEL_IRR,
			MODEL_OFF,
			MODEL_OBJ,
			MODEL_TER,
			MODEL_HMP,
			MODEL_MESH_XML,
			MODEL_SKELETON_XML,
			MODEL_MATERIAL,
			MODEL_OGEX,
			MODEL_MS3D,
			MODEL_LWO,
			MODEL_LWS,
			MODEL_CSM,
			MODEL_COB,
			MODEL_SCN,
			MODEL_XGL,

			// --------------- Shader Files --------------- //

			// Rasterization
			SHADER_VSH,
			SHADER_TCSH,
			SHADER_TESH,
			SHADER_GSH,
			SHADER_PSH,

			// Compute Shader Stage.
			SHADER_CSH,

			// Raytracing Shader Stages.
			SHADER_RGSH,
			SHADER_AHSH,
			SHADER_CHSH,
			SHADER_MSSH,
			SHADER_INSH,
			SHADER_CASH,

			// Function Library
			SHADER_GLSL,

			// Shader Binary.
			SHADER_SPV,

			// OpenCL Kernel
			KERNEL_OCL,
			// Lua Script
			SCRIPT_LUA
		};

		enum caste {
			TID_UNKOWN,
			DYNALIB,
			FONT,
			IMAGE,
			MODEL,
			SHADER,


			AUDIO,
			KERNEL,
			BYTE_CODE,
			PLAIN_TEXT,
			SCRIPT,
		};

		static struct built_in_type {
			extension ID;
			std::vector<util::string> Extension;
		} BuiltInTypes[];

		static extension string_to_eid(util::string aString);
		static caste eid_to_cid(extension aEID);
		static util::string eid_to_string(extension aType);
		static file* open(const char* aFilePath);

		//static void close(file* aFile);

		~file();

		util::string get_path();
		util::string get_dir();
		util::string get_name();
		util::string get_ext();
		void* get_data(size_t& ReturnSize);

	protected:

		// Path = Directory + "/" + Name + "." Extension
		util::string	Path;
		util::string	Directory;
		util::string	Name;
		util::string	Extension;
		extension		ID;

		// Host Data
		size_t			HostSize;
		void*			HostData;

		file();
		file(const char* aFilePath);
		file(util::string aFilePath);

		bool setup(util::string aFilePath);

		bool read(const char* aFilePath);

		//bool resize(size_t aNewCount);

		//void zero_out();

	};

}

#endif // GEODESUKA_CORE_IO_FILE_H
