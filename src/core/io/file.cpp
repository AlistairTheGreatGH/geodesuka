#include <geodesuka/core/io/file.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <string>
//#include <vector>
//#include <fstream>
//#include <sstream>

// Used for generic file loading.
#include <geodesuka/core/io/dynalib.h>

#include <geodesuka/core/gcl/shader.h>
#include <geodesuka/core/gcl/image.h>

#include <geodesuka/core/graphics/font.h>
#include <geodesuka/core/graphics/model.h>

//// Lua Script
//#include "lua.hpp"
//#include "printstack.h"


namespace geodesuka::core::io {

	using namespace gcl;
	using namespace graphics;

	// This is just a lookup table for file type extensions.
	// Extension ID, Extension String, Category
	file::built_in_type file::BuiltInTypes[] = {
		{ file::extension::DYNALIB_DYN,				{"dll", "so", "dylib"}		},
		{ file::extension::FONT_TTF,				{ "ttf" }					},
		{ file::extension::FONT_TTC,				{ "ttc" }					},
		{ file::extension::FONT_OTF,				{ "otf" }					},
		{ file::extension::FONT_PFM,				{ "pfm" }					},
		{ file::extension::IMAGE_BMP,				{ "bmp" }					},
		{ file::extension::IMAGE_ICO,				{ "ico" }					},
		{ file::extension::IMAGE_JPEG,				{ "jpeg" }					},
		{ file::extension::IMAGE_JNG,				{ "jng" }					},
		{ file::extension::IMAGE_KOALA,				{ "koala" }					},
		{ file::extension::IMAGE_LBM,				{ "lbm" }					},
		{ file::extension::IMAGE_IFF,				{ "iff" }					},
		{ file::extension::IMAGE_MNG,				{ "mng" }					},
		{ file::extension::IMAGE_PBM,				{ "pbm" }					},
		{ file::extension::IMAGE_PBMRAW,			{ "pbmraw" }				},
		{ file::extension::IMAGE_PCD,				{ "pcd" }					},
		{ file::extension::IMAGE_PCX,				{ "pcx" }					},
		{ file::extension::IMAGE_PGM,				{ "pgm" }					},
		{ file::extension::IMAGE_PGMRAW,			{ "pgmraw" }				},
		{ file::extension::IMAGE_PNG,				{ "png" }					},
		{ file::extension::IMAGE_PPM,				{ "ppm" }					},
		{ file::extension::IMAGE_PPMRAW,			{ "ppmraw" }				},
		{ file::extension::IMAGE_RAS,				{ "ras" }					},
		{ file::extension::IMAGE_TARGA,				{ "targa" }					},
		{ file::extension::IMAGE_TIFF,				{ "tiff" }					},
		{ file::extension::IMAGE_WBMP,				{ "wbmp" }					},
		{ file::extension::IMAGE_PSD,				{ "psd" }					},
		{ file::extension::IMAGE_CUT,				{ "cut" }					},
		{ file::extension::IMAGE_XBM,				{ "xbm" }					},
		{ file::extension::IMAGE_XPM,				{ "xpm" }					},
		{ file::extension::IMAGE_DDS,				{ "dds" }					},
		{ file::extension::IMAGE_GIF,				{ "gif" }					},
		{ file::extension::IMAGE_HDR,				{ "hdr" }					},
		{ file::extension::IMAGE_FAXG3,				{ "faxg3" }					},
		{ file::extension::IMAGE_SGI,				{ "sgi" }					},
		{ file::extension::IMAGE_EXR,				{ "exr" }					},
		{ file::extension::IMAGE_J2K,				{ "j2k" }					},
		{ file::extension::IMAGE_JP2,				{ "jp2" }					},
		{ file::extension::IMAGE_PFM,				{ "pfm" }					},
		{ file::extension::IMAGE_PICT,				{ "pict" }					},
		{ file::extension::IMAGE_RAW,				{ "raw" }					},
		{ file::extension::IMAGE_WEBP,				{ "webp" }					},
		{ file::extension::IMAGE_JXR,				{ "jxr" }					},
		{ file::extension::MODEL_3MF,				{ "3mf" }					},
		{ file::extension::MODEL_DAE,				{ "dae" }					},
		{ file::extension::MODEL_BVH,				{ "bvh" }					},
		{ file::extension::MODEL_3DS,				{ "3ds" }					},
		{ file::extension::MODEL_ASE,				{ "ase" }					},
		{ file::extension::MODEL_GLTF,				{ "gltf" }					},
		{ file::extension::MODEL_FBX,				{ "fbx" }					},
		{ file::extension::MODEL_PLY,				{ "ply" }					},
		{ file::extension::MODEL_DXF,				{ "dxf" }					},
		{ file::extension::MODEL_IFC,				{ "ifc" }					},
		{ file::extension::MODEL_NFF,				{ "nff" }					},
		{ file::extension::MODEL_SMD,				{ "smd" }					},
		{ file::extension::MODEL_VTA,				{ "vta" }					},
		{ file::extension::MODEL_MDL,				{ "mdl" }					},
		{ file::extension::MODEL_MD2,				{ "md2" }					},
		{ file::extension::MODEL_MD3,				{ "md3" }					},
		{ file::extension::MODEL_PK3,				{ "pk3" }					},
		{ file::extension::MODEL_MDC,				{ "mdc" }					},
		{ file::extension::MODEL_MD5MESH,			{ "md5mesh" }				},
		{ file::extension::MODEL_MD5ANIM,			{ "md5anim" }				},
		{ file::extension::MODEL_MD5CAMERA,			{ "md5camera" }				},
		{ file::extension::MODEL_X,					{ "x" }						},
		{ file::extension::MODEL_Q3O,				{ "q3o" }					},
		{ file::extension::MODEL_Q3S,				{ "q3s" }					},
		{ file::extension::MODEL_AC,				{ "ac" }					},
		{ file::extension::MODEL_AC3D,				{ "ac3d" }					},
		{ file::extension::MODEL_STL,				{ "stl" }					},
		{ file::extension::MODEL_IRRMESH,			{ "irrmesh" }				},
		{ file::extension::MODEL_IRR,				{ "irr" }					},
		{ file::extension::MODEL_OFF,				{ "off" }					},
		{ file::extension::MODEL_OBJ,				{ "obj" }					},
		{ file::extension::MODEL_TER,				{ "ter" }					},
		{ file::extension::MODEL_HMP,				{ "hmp" }					},
		{ file::extension::MODEL_MESH_XML,			{ "mesh_xml" }				},
		{ file::extension::MODEL_SKELETON_XML,		{ "skeleton_xml" }			},
		{ file::extension::MODEL_MATERIAL,			{ "material" }				},
		{ file::extension::MODEL_OGEX,				{ "ogex" }					},
		{ file::extension::MODEL_MS3D,				{ "ms3d" }					},
		{ file::extension::MODEL_LWO,				{ "lwo" }					},
		{ file::extension::MODEL_LWS,				{ "lws" }					},
		{ file::extension::MODEL_CSM,				{ "csm" }					},
		{ file::extension::MODEL_COB,				{ "cob" }					},
		{ file::extension::MODEL_SCN,				{ "scn" }					},
		{ file::extension::MODEL_XGL,				{ "xgl" }					},
		{ file::extension::SHADER_VSH,				{ "vsh", "vert" }			},
		{ file::extension::SHADER_TCSH,				{ "tcsh" }					},
		{ file::extension::SHADER_TESH,				{ "tesh" }					},
		{ file::extension::SHADER_GSH,				{ "gsh" }					},
		{ file::extension::SHADER_PSH,				{ "psh", "fsh", "frag" }	},		
		{ file::extension::SHADER_RGSH,				{ "rgsh" }					},
		{ file::extension::SHADER_AHSH,				{ "ahsh" }					},
		{ file::extension::SHADER_CHSH,				{ "chsh" }					},
		{ file::extension::SHADER_MSSH,				{ "mssh" }					},
		{ file::extension::SHADER_INSH,				{ "insh" }					},
		{ file::extension::SHADER_CASH,				{ "cash" }					},
		{ file::extension::SHADER_GLSL,				{ "glsl" }					},
		{ file::extension::SHADER_SPV,				{ "spv" }					},
		
	};

	file::extension file::string_to_eid(util::string aString) {
		extension ReturnID = EID_UNKNOWN;
		size_t BITCount = sizeof(BuiltInTypes) / sizeof(built_in_type);
		for (size_t i = 0; i < BITCount; i++) {
			for (size_t j = 0; j < BuiltInTypes[i].Extension.size(); j++)
				if (BuiltInTypes[i].Extension[j] == aString) {
					return BuiltInTypes[i].ID;
				}
		}
		return ReturnID;
	}

	file::caste file::eid_to_cid(extension aEID) {
		caste CID = TID_UNKOWN;
		switch (aEID) {
		default:
			CID = TID_UNKOWN;
			break;
		case DYNALIB_DYN:
			CID = DYNALIB;
			break;
		case FONT_TTF:
		case FONT_TTC:
		case FONT_OTF:
		case FONT_PFM:
			CID = FONT;
			break;
		case IMAGE_BMP:
		case IMAGE_ICO:
		case IMAGE_JPEG:
		case IMAGE_JNG:
		case IMAGE_KOALA:
		case IMAGE_LBM:
		case IMAGE_MNG:
		case IMAGE_PBM:
		case IMAGE_PBMRAW:
		case IMAGE_PCD:
		case IMAGE_PCX:
		case IMAGE_PGM:
		case IMAGE_PGMRAW:
		case IMAGE_PNG:
		case IMAGE_PPM:
		case IMAGE_PPMRAW:
		case IMAGE_RAS:
		case IMAGE_TARGA:
		case IMAGE_TIFF:
		case IMAGE_WBMP:
		case IMAGE_PSD:
		case IMAGE_CUT:
		case IMAGE_XBM:
		case IMAGE_XPM:
		case IMAGE_DDS:
		case IMAGE_GIF:
		case IMAGE_HDR:
		case IMAGE_FAXG3:
		case IMAGE_SGI:
		case IMAGE_EXR:
		case IMAGE_J2K:
		case IMAGE_JP2:
		case IMAGE_PICT:
		case IMAGE_RAW:
		case IMAGE_WEBP:
		case IMAGE_JXR:
			CID = IMAGE;
			break;
		case MODEL_3MF:
		case MODEL_DAE:
		case MODEL_BVH:
		case MODEL_3DS:
		case MODEL_ASE:
		case MODEL_GLTF:
		case MODEL_FBX:
		case MODEL_PLY:
		case MODEL_DXF:
		case MODEL_IFC:
		case MODEL_NFF:
		case MODEL_SMD:
		case MODEL_VTA:
		case MODEL_MDL:
		case MODEL_MD2:
		case MODEL_MD3:
		case MODEL_PK3:
		case MODEL_MDC:
		case MODEL_MD5MESH:
		case MODEL_MD5ANIM:
		case MODEL_MD5CAMERA:
		case MODEL_X:
		case MODEL_Q3O:
		case MODEL_Q3S:
		case MODEL_AC:
		case MODEL_AC3D:
		case MODEL_STL:
		case MODEL_IRRMESH:
		case MODEL_IRR:
		case MODEL_OFF:
		case MODEL_OBJ:
		case MODEL_TER:
		case MODEL_HMP:
		case MODEL_MESH_XML:
		case MODEL_SKELETON_XML:
		case MODEL_MATERIAL:
		case MODEL_OGEX:
		case MODEL_MS3D:
		case MODEL_LWO:
		case MODEL_LWS:
		case MODEL_CSM:
		case MODEL_COB:
		case MODEL_SCN:
		case MODEL_XGL:
			CID = MODEL;
			break;
		case SHADER_VSH:
		case SHADER_TCSH:
		case SHADER_TESH:
		case SHADER_GSH:
		case SHADER_PSH:
		case SHADER_CSH:
		case SHADER_RGSH:
		case SHADER_AHSH:
		case SHADER_CHSH:
		case SHADER_MSSH:
		case SHADER_INSH:
		case SHADER_CASH:
		case SHADER_GLSL:
		case SHADER_SPV:
			CID = SHADER;
			break;
		}
		return CID;
	}

	util::string file::eid_to_string(extension aType) {
		const char* ReturnString = "";
		for (size_t i = 0; i < sizeof(BuiltInTypes) / sizeof(built_in_type); i++) {
			if (BuiltInTypes[i].ID == aType) {
				return BuiltInTypes[i].Extension[0].ptr();
			}
		}
		return ReturnString;
	}

	file* file::open(const char* aFilePath) {
		file* File = nullptr;
		util::string Path = aFilePath;
		Path.reverse();
		util::string Extension = Path.split_at('.');
		Extension.reverse();
		file::extension ExtID = string_to_eid(Extension);
		file::caste TypeID = eid_to_cid(ExtID);
		switch (TypeID) {
		default:
			File = nullptr;
			break;
		case DYNALIB:
			File = new dynalib(aFilePath);
			break;
		case FONT:
			File = new font(aFilePath);
			break;
		case IMAGE:
			File = new image(aFilePath);
			break;
		case SHADER:
			File = new shader(aFilePath);
			break;
		case MODEL:
			File = new model(aFilePath);
			break;
		}
		return File;
	}

	file::~file() {
		free(this->HostData);
		this->HostSize = 0;
		this->HostData = NULL;
	}

	util::string file::get_path() {
		return this->Path;
	}

	util::string file::get_dir() {
		return this->Directory;
	}

	util::string file::get_name() {
		return this->Name;
	}

	util::string file::get_ext() {
		return this->Extension;
	}

	void* file::get_data(size_t& ReturnSize) {
		ReturnSize = this->HostSize;
		return this->HostData;
	}

	file::file() {
		this->Path = "";
		this->Directory = "";
		this->Name = "";
		this->Extension = "";
		this->ID = EID_UNKNOWN;

		this->HostSize = 0;
		this->HostData = NULL;
	}

	file::file(const char* aFilePath) {
		this->setup(aFilePath);
		//this->load_raw(aFilePath);
	}

	file::file(util::string aFilePath) {
		this->setup(aFilePath);
		//this->load_raw(aFilePath.ptr());
	}

	bool file::setup(util::string aFilePath) {
		this->Path = aFilePath;
		util::string TempString = aFilePath;
		TempString.reverse();
		this->Extension = TempString.split_at('.');
		this->Extension.reverse();
		this->Name = TempString.split_at("/");
		this->Name.reverse();
		this->Directory = TempString;
		this->Directory.reverse();
		this->ID = string_to_eid(this->Extension);
		return false;
	}

	bool file::read(const char* aFilePath) {
		FILE* Handle = fopen(aFilePath, "rb");
		
		if (Handle == NULL) return true;
		long FileSize = 0;
		fseek(Handle, 0L, SEEK_END);
		FileSize = ftell(Handle);
		fseek(Handle, 0L, SEEK_SET);

		this->HostData = malloc((FileSize + 1) * sizeof(char));
		if (this->HostData == NULL) return true;

		memset(this->HostData, 0xAA, (FileSize + 1) * sizeof(char));

		this->HostSize = FileSize;
		fread(this->HostData, sizeof(char), this->HostSize, Handle);

		char* String = (char*)this->HostData;
		String[this->HostSize] = '\0';

		fclose(Handle);
		return false;
	}

}















//FIBITMAP*			ImageLoader = nullptr;
//FT_Library			FontLoader;
//Assimp::Importer*	ModelLoader = nullptr;
//
//file::extension get_extension_type(std::string ext);
//file::category get_category_type(std::string ext);
//
//file::file() {
//	isValid			= false;
//	Cat				= UNKNOWN;
//	Ext				= EXT_UNK;
//}
//
//file::~file() {
//
//}
//
//file::file(std::string FilePath) {
//	isValid			= false;
//	Cat				= UNKNOWN;
//	Ext				= EXT_UNK;
//
//	isValid = analyze_path(FilePath);
//}
//
//file & file::operator=(std::string FilePath) {
//	this->isValid			= false;
//	this->Cat				= UNKNOWN;
//	this->Ext				= EXT_UNK;
//
//	this->isValid = analyze_path(FilePath);
//
//	return *this;
//}
//
//void file::set_path(std::string FilePath) {
//	isValid		= false;
//	Cat			= UNKNOWN;
//	Ext			= EXT_UNK;
//
//	Directory.clear();
//	Name.clear();
//	Extension.clear();
//
//	isValid = analyze_path(FilePath);
//
//}
//
//std::string file::get_path() {
//	std::string path;
//	for (int i = 0; i < Directory.size(); i++) {
//		path += Directory[i] + "/";
//	}
//	path += Name + "." + Extension;
//	return path;
//}
//
//std::string file::get_sauce() {
//
//	std::string source;
//	std::string FilePath;
//
//	for (int i = 0; i < Directory.size(); i++) {
//		FilePath += Directory[i] + "/";
//	}
//	FilePath += Name + "." + Extension;
//
//	std::ifstream SauceFile;
//	std::stringstream SauceBuffer;
//	SauceFile.open(FilePath);
//
//	if (!SauceFile.fail()) {
//		SauceBuffer << SauceFile.rdbuf();
//		source = SauceBuffer.str();
//	}
//	SauceFile.close();
//
//	return source;
//}
//
//file::image_data file::get_image() {
//
//	image_data ImageData;
//	
//	ImageData.Buffer = NULL;
//	ImageData.MemSize = 0;
//	ImageData.BitCount = 0;
//	ImageData.Width = 0;
//	ImageData.Height = 0;
//	ImageData.Format = image_data_format::IDF_UNK;
//	ImageData.Type = image_data_type::IDT_UNK;
//
//	// Generate File path.
//	std::string FilePath;
//	for (int i = 0; i < Directory.size(); i++) {
//		FilePath += Directory[i] + "/";
//	}
//	FilePath += Name + "." + Extension;
//
//	FIBITMAP* fiBitmap;
//	FREE_IMAGE_FORMAT fiFormat;
//	FREE_IMAGE_TYPE fiType;
//
//	fiFormat = FreeImage_GetFileType(FilePath.c_str(), 0);
//	if (fiFormat == FIF_UNKNOWN) {
//		fiFormat = FreeImage_GetFIFFromFilename(FilePath.c_str());
//	}
//
//	fiBitmap = FreeImage_Load(fiFormat, FilePath.c_str());
//	fiType = FreeImage_GetImageType(fiBitmap);
//
//	ImageData.Width = FreeImage_GetWidth(fiBitmap);
//	ImageData.Height = FreeImage_GetHeight(fiBitmap);
//	ImageData.MemSize = FreeImage_GetMemorySize(fiBitmap);
//	ImageData.BitCount = FreeImage_GetBPP(fiBitmap);
//
//	switch (fiType) {
//	default:
//		ImageData.Format = image_data_format::IDF_UNK;
//		ImageData.Type = image_data_type::IDT_UNK;
//		break;
//	case FIT_BITMAP:
//		switch (ImageData.BitCount) {
//		default:
//			ImageData.Format = image_data_format::IDF_UNK;
//			ImageData.Type = image_data_type::IDT_UNK;
//			break;
//		case 1:
//			ImageData.Format = image_data_format::IDF_RED;
//			ImageData.Type = image_data_type::IDT_BOOL;
//			break;
//		case 4:
//			ImageData.Format = image_data_format::IDF_BGRA;
//			ImageData.Type = image_data_type::IDT_BOOL;
//			break;
//		case 8:
//			ImageData.Format = image_data_format::IDF_RED;
//			ImageData.Type = image_data_type::IDT_UINT08;
//			break;
//		case 16:
//			ImageData.Format = image_data_format::IDF_RG;
//			ImageData.Type = image_data_type::IDT_UINT08;
//			break;
//		case 24:
//			ImageData.Format = image_data_format::IDF_BGR;
//			ImageData.Type = image_data_type::IDT_UINT08;
//			break;
//		case 32:
//			ImageData.Format = image_data_format::IDF_BGRA;
//			ImageData.Type = image_data_type::IDT_UINT08;
//			break;
//		}
//		break;
//	case FIT_UINT16:
//		ImageData.Format = image_data_format::IDF_RED;
//		ImageData.Type = image_data_type::IDT_UINT16;
//		break;
//	case FIT_INT16:
//		ImageData.Format = image_data_format::IDF_RED;
//		ImageData.Type = image_data_type::IDT_SINT16;
//		break;
//	case FIT_UINT32:
//		ImageData.Format = image_data_format::IDF_RED;
//		ImageData.Type = image_data_type::IDT_UINT32;
//		break;
//	case FIT_INT32:
//		ImageData.Format = image_data_format::IDF_RED;
//		ImageData.Type = image_data_type::IDT_SINT32;
//		break;
//	case FIT_FLOAT:
//		ImageData.Format = image_data_format::IDF_RED;
//		ImageData.Type = image_data_type::IDT_FLOAT32;
//		break;
//	case FIT_DOUBLE:
//		ImageData.Format = image_data_format::IDF_RED;
//		ImageData.Type = image_data_type::IDT_FLOAT64;
//		break;
//	case FIT_COMPLEX:
//		ImageData.Format = image_data_format::IDF_RG;
//		ImageData.Type = image_data_type::IDT_FLOAT64;
//		break;
//	case FIT_RGB16:
//		ImageData.Format = image_data_format::IDF_BGR;
//		ImageData.Type = image_data_type::IDT_UINT16;
//		break;
//	case FIT_RGBA16:
//		ImageData.Format = image_data_format::IDF_BGRA;
//		ImageData.Type = image_data_type::IDT_UINT16;
//		break;
//	case FIT_RGBF:
//		ImageData.Format = image_data_format::IDF_BGR;
//		ImageData.Type = image_data_type::IDT_FLOAT32;
//		break;
//	case FIT_RGBAF:
//		ImageData.Format = image_data_format::IDF_BGRA;
//		ImageData.Type = image_data_type::IDT_FLOAT32;
//		break;
//	}
//
//	if ((ImageData.MemSize > 0) && (ImageData.Format != image_data_format::IDF_UNK) && (ImageData.Type != image_data_type::IDT_UNK)) {
//		ImageData.Buffer = malloc(ImageData.MemSize);
//		if (ImageData.Buffer != NULL) {
//			memcpy(ImageData.Buffer, FreeImage_GetBits(fiBitmap), ImageData.MemSize);
//		}
//	}
//
//	if (fiBitmap != NULL) {
//		FreeImage_Unload(fiBitmap);
//		fiBitmap = NULL;
//	}
//
//	return ImageData;
//}
//
//std::vector<file::font_data> file::get_font() {
//
//	std::vector<font_data> temp;
//	font_data CharacterGlyph;
//
//	// Generate File path.
//	std::string FilePath;
//	for (int i = 0; i < Directory.size(); i++) {
//		FilePath += Directory[i] + "/";
//	}
//	FilePath += Name + "." + Extension;
//
//	FT_Face Face;
//
//	if (FT_New_Face(FontLoader, FilePath.c_str(), 0, &Face)) {
//		isValid = false;
//	}
//
//	if (isValid) {
//
//		FT_Set_Pixel_Sizes(Face, 0, 128);
//
//		for (unsigned char i = 0; i < 128; i++) {
//			if (FT_Load_Char(Face, i, FT_LOAD_RENDER)) {
//				continue;
//			}
//
//			CharacterGlyph.Index = i;									// Character Index
//			CharacterGlyph.Width = Face->glyph->bitmap.width;			// Number of pixels in bitmap row.
//			CharacterGlyph.Height = Face->glyph->bitmap.rows;			// Number of rows in pixels. (aka Height)
//			CharacterGlyph.BearingX = Face->glyph->bitmap_left;			// Units: Pixels
//			CharacterGlyph.BearingY = Face->glyph->bitmap_top;			// Units: Pixels
//			CharacterGlyph.AdvanceX = (Face->glyph->advance.x >> 6);	// Units: Pixels
//			CharacterGlyph.AdvanceY = (Face->glyph->advance.y >> 6);	// Units: Pixels
//			
//			CharacterGlyph.Buffer = malloc(Face->glyph->bitmap.width * Face->glyph->bitmap.rows *sizeof(unsigned char));
//			memcpy(CharacterGlyph.Buffer, (void*)Face->glyph->bitmap.buffer, Face->glyph->bitmap.width * Face->glyph->bitmap.rows *sizeof(unsigned char));
//
//			temp.push_back(CharacterGlyph);
//		}
//		FT_Done_Face(Face);
//	}
//
//	return temp;
//}
//
//bool file::initialize_system() {
//	FreeImage_Initialise();
//
//	if (FT_Init_FreeType(&FontLoader)) {
//		return true;
//	}
//
//	return false;
//}
//
//bool file::terminate_system() {
//	FreeImage_DeInitialise();
//
//	FT_Done_FreeType(FontLoader);
//
//	return false;
//}
//
//bool file::analyze_path(std::string path) {
//	// NULL
//	// Astolfo
//	// Astolfo.png
//	// assets/textures/Astolfo
//	// assets/textures/Astolfo.png
//
//	if (path.length() != 0) {
//		// Check if in same directory.
//		bool isLocalDir = true;
//		std::string Item;
//		bool ExtensionDetected = false;
//
//		for (int i = 0; i < path.length(); i++) {
//			if ((path[i] == '/') || (path[i] == '\\')) {
//				isLocalDir = false;
//				break;
//			}
//		}
//
//		if (!isLocalDir) {
//			// File is in another directory.
//			for (int i = 0; i < path.length(); i++) {
//				switch (path[i]) {
//				case '/':
//					Directory.push_back(Item);
//					Item.clear();
//					break;
//				case '\\':
//					Directory.push_back(Item);
//					Item.clear();
//					break;
//				default:
//					Item.push_back(path[i]);
//					break;
//				}
//			}
//		}
//		else {
//			// File is in same directory.
//			Item = path;
//		}
//
//		// Get extension if it exists.
//		for (int i = 0; i < Item.length(); i++) {
//			// No names with '.'
//			if (!ExtensionDetected) {
//				Name.push_back(Item[i]);
//				if (Item[i] == '.') {
//					Name.pop_back();
//					ExtensionDetected = true;
//				}
//			}
//			else {
//				Extension.push_back(Item[i]);
//			}
//		}
//
//		// Validate extension
//		if (ExtensionDetected) {
//			// Find Extension Number
//			Cat = get_category_type(Extension);
//			Ext = get_extension_type(Extension);
//			if ((Cat == file::category::UNKNOWN) || (Ext == file::extension::EXT_UNK)) {
//				return false;
//			}
//			else {
//				return true;
//			}
//		}
//		else {
//			// Throw error or something, not a valid extension.
//			return false;
//		}
//	}
//	else {
//		return false;
//	}
//
//	//std::string Item;
//	//for (int i = 0; i < path.length(); i++) {
//	//	switch (path[i]) {
//	//	case '/':
//	//		Directory.push_back(Item);
//	//		Item.clear();
//	//		break;
//	//	case '\\':
//	//		Directory.push_back(Item);
//	//		Item.clear();
//	//		break;
//	//	default:
//	//		Item.push_back(path[i]);
//	//		break;
//	//	}
//	//}
//
//	//// Example.vsh
//	//bool ExtensionDetected = false;
//	//for (int i = 0; i < Item.length(); i++) {
//	//	// No names with '.'
//	//	if (!ExtensionDetected) {
//	//		Name.push_back(Item[i]);
//	//		if (Item[i] == '.') {
//	//			Name.pop_back();
//	//			ExtensionDetected = true;
//	//		}
//	//	}
//	//	else {
//	//		Extension.push_back(Item[i]);
//	//	}
//	//}
//}
//
//file::extension get_extension_type(std::string ext) {
//	if (ext == "bmp") {
//		return file::extension::EXT_BMP;
//	}
//	else if (ext == "ico") {
//		return file::extension::EXT_ICO;
//	}
//	else if ((ext == "jpg") || (ext == "jpeg")) {
//		return file::extension::EXT_JPEG;
//	}
//	else if (ext == "jng") {
//		return file::extension::EXT_JNG;
//	}
//	else if (ext == "koala") {
//		return file::extension::EXT_KOALA;
//	}
//	else if ((ext == "iff") || (ext == "lbm")) {
//		return file::extension::EXT_LBM;
//	}
//	else if (ext == "mng") {
//		return file::extension::EXT_MNG;
//	}
//	else if (ext == "pbm") {
//		return file::extension::EXT_PBM;
//	}
//	else if (ext == "pbmraw") {
//		return file::extension::EXT_PBMRAW;
//	}
//	else if (ext == "pcd") {
//		return file::extension::EXT_PCD;
//	}
//	else if (ext == "pcx") {
//		return file::extension::EXT_PCX;
//	}
//	else if (ext == "pcx") {
//		return file::extension::EXT_PCX;
//	}
//	else if (ext == "pgm") {
//		return file::extension::EXT_PGM;
//	}
//	else if (ext == "pgmraw") {
//		return file::extension::EXT_PGMRAW;
//	}
//	else if (ext == "png") {
//		return file::extension::EXT_PNG;
//	}
//	else if (ext == "ppm") {
//		return file::extension::EXT_PPM;
//	}
//	else if (ext == "ppmraw") {
//		return file::extension::EXT_PPMRAW;
//	}
//	else if (ext == "ras") {
//		return file::extension::EXT_RAS;
//	}
//	else if ((ext == "tga") || (ext == "targa")) {
//		return file::extension::EXT_TARGA;
//	}
//	else if ((ext == "tif") || (ext == "tiff")) {
//		return file::extension::EXT_TIFF;
//	}
//	else if (ext == "wbmp") {
//		return file::extension::EXT_WBMP;
//	}
//	else if (ext == "psd") {
//		return file::extension::EXT_PSD;
//	}
//	else if (ext == "cut") {
//		return file::extension::EXT_CUT;
//	}
//	else if (ext == "xbm") {
//		return file::extension::EXT_XBM;
//	}
//	else if (ext == "xpm") {
//		return file::extension::EXT_XPM;
//	}
//	else if (ext == "dds") {
//		return file::extension::EXT_DDS;
//	}
//	else if (ext == "gif") {
//		return file::extension::EXT_GIF;
//	}
//	else if (ext == "hdr") {
//		return file::extension::EXT_HDR;
//	}
//	else if (ext == "g3") {
//		return file::extension::EXT_FAXG3;
//	}
//	else if (ext == "sgi") {
//		return file::extension::EXT_SGI;
//	}
//	else if (ext == "exr") {
//		return file::extension::EXT_EXR;
//	}
//	else if ((ext == "j2k") || (ext == "j2c")) {
//		return file::extension::EXT_EXR;
//	}
//	else if (ext == "jp2") {
//		return file::extension::EXT_JP2;
//	}
//	//else if (ext == "pfm") {
//	//	return file::extension_type::EXT_PFM;
//	//}
//	else if ((ext == "pct") || (ext == "pict") || (ext == "pic")) {
//		return file::extension::EXT_PICT;
//	}
//	else if (ext == "webp") {
//		return file::extension::EXT_WEBP;
//	}
//	else if ((ext == "jxr") || (ext == "wdp") || (ext == "hdp")) {
//		return file::extension::EXT_JXR;
//	}
//	else if (ext == "ttf") {
//		return file::extension::EXT_TTF;
//	}
//	else if (ext == "ttc") {
//		return file::extension::EXT_TTC;
//	}
//	else if (ext == "otf") {
//		return file::extension::EXT_OTF;
//	}
//	else if (ext == "pfm") {
//		return file::extension::EXT_PFM;
//	}
//	else if (ext == "vsh") {
//		return file::extension::EXT_VSH;
//	}
//	else if (ext == "gsh") {
//		return file::extension::EXT_GSH;
//	}
//	else if (ext == "fsh") {
//		return file::extension::EXT_FSH;
//	}
//	else if (ext == "cl") {
//		return file::extension::EXT_CL;
//	}
//	else if (ext == "lua") {
//		return file::extension::EXT_LUA;
//	}
//	else {
//		return file::extension::EXT_UNK;
//		//return file::extension_type::EXT_RAW;
//	}
//}
//
//file::category get_category_type(std::string ext) {
//	file::extension temp = get_extension_type(ext);
//	if ((temp >= file::extension::EXT_BMP) && (temp <= file::extension::EXT_JXR)) {
//		return file::category::IMAGE_DATA;
//	}
//	else if ((temp >= file::extension::EXT_TTF) && (temp <= file::extension::EXT_PFM)) {
//		return file::category::FONT_DATA;
//	}
//	else if ((temp >= file::extension::EXT_VSH) && (temp <= file::extension::EXT_FSH)) {
//		return file::category::OGL_SHADER;
//	}
//	else if (temp == file::extension::EXT_CL) {
//		return file::category::OCL_KERNEL;
//	}
//	else if (temp == file::extension::EXT_LUA) {
//		return file::category::LUA_SCRIPT;
//	}
//	else {
//		return file::category::UNKNOWN;
//	}
//}
