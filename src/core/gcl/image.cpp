#include <geodesuka/core/gcl/image.h>

// Standard C Libs
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>

#include <assert.h>

#include <vector>

#include <geodesuka/core/math.h>

#include <geodesuka/core/gcl/variable.h>

//// Image Loading
//#define FREEIMAGE_LIB
//#include <FreeImage.h>

// So gross.
// Group these later based on spec.
// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#texel-block-size

/*
Size table;

8
16
24
32
48
64
96
128
192
256
*/

namespace geodesuka::core::gcl {

	using namespace util;

	/*
	static FREE_IMAGE_FORMAT extid_to_fifid(io::file::extension ExtensionID) {
		switch (ExtensionID) {
		default:								return FIF_UNKNOWN;
		case io::file::extension::IMAGE_BMP:	return FIF_BMP;
		case io::file::extension::IMAGE_ICO:	return FIF_ICO;
		case io::file::extension::IMAGE_JPEG:	return FIF_JPEG;
		case io::file::extension::IMAGE_JNG:	return FIF_JNG;
		case io::file::extension::IMAGE_KOALA:	return FIF_KOALA;
		case io::file::extension::IMAGE_LBM:	return FIF_LBM;
		case io::file::extension::IMAGE_IFF:	return FIF_IFF;
		case io::file::extension::IMAGE_MNG:	return FIF_MNG;
		case io::file::extension::IMAGE_PBM:	return FIF_PBM;
		case io::file::extension::IMAGE_PBMRAW: return FIF_PBMRAW;
		case io::file::extension::IMAGE_PCD:	return FIF_PCD;
		case io::file::extension::IMAGE_PCX:	return FIF_PCX;
		case io::file::extension::IMAGE_PGM:	return FIF_PGM;
		case io::file::extension::IMAGE_PGMRAW: return FIF_PGMRAW;
		case io::file::extension::IMAGE_PNG:	return FIF_PNG;
		case io::file::extension::IMAGE_PPM:	return FIF_PPM;
		case io::file::extension::IMAGE_PPMRAW: return FIF_PPMRAW;
		case io::file::extension::IMAGE_RAS:	return FIF_RAS;
		case io::file::extension::IMAGE_TARGA:	return FIF_TARGA;
		case io::file::extension::IMAGE_TIFF:	return FIF_TIFF;
		case io::file::extension::IMAGE_WBMP:	return FIF_WBMP;
		case io::file::extension::IMAGE_PSD:	return FIF_PSD;
		case io::file::extension::IMAGE_CUT:	return FIF_CUT;
		case io::file::extension::IMAGE_XBM:	return FIF_XBM;
		case io::file::extension::IMAGE_XPM:	return FIF_XPM;
		case io::file::extension::IMAGE_DDS:	return FIF_DDS;
		case io::file::extension::IMAGE_GIF:	return FIF_GIF;
		case io::file::extension::IMAGE_HDR:	return FIF_HDR;
		case io::file::extension::IMAGE_FAXG3:	return FIF_FAXG3;
		case io::file::extension::IMAGE_SGI:	return FIF_SGI;
		case io::file::extension::IMAGE_EXR:	return FIF_EXR;
		case io::file::extension::IMAGE_J2K:	return FIF_J2K;
		case io::file::extension::IMAGE_JP2:	return FIF_JP2;
		case io::file::extension::IMAGE_PFM:	return FIF_PFM;
		case io::file::extension::IMAGE_PICT:	return FIF_PICT;
		case io::file::extension::IMAGE_RAW:	return FIF_RAW;
		case io::file::extension::IMAGE_WEBP:	return FIF_WEBP;
		case io::file::extension::IMAGE_JXR:	return FIF_JXR;
		}
	}
	*/

	image::create_info::create_info() {
		this->Sample = image::sample::COUNT_1;
		this->Tiling = image::tiling::OPTIMAL;
		this->Memory = 0;
		this->Usage = image::usage::TRANSFER_DST | image::usage::TRANSFER_SRC;
	}

	image::create_info::create_info(int aSample, int aTiling, int aMemory, int aUsage) {
		this->Sample = aSample;
		this->Tiling = aTiling;
		this->Memory = aMemory;
		this->Usage = aUsage;
	}

	image::format image::t2f(type::id aID) {
		switch (aID) {
		default:					return image::format::UNDEFINED;
			// Scalar Types
		case type::id::UCHAR:		return image::format::R8_UINT;
		case type::id::USHORT:		return image::format::R16_UINT;
		case type::id::UINT:		return image::format::R32_UINT;
		case type::id::CHAR:		return image::format::R8_SINT;
		case type::id::SHORT:		return image::format::R16_SINT;
		case type::id::INT:			return image::format::R32_SINT;
		case type::id::FLOAT:		return image::format::R32_SFLOAT;
		case type::id::DOUBLE:		return image::format::R64_SFLOAT;
			// Vector Types.
		case type::id::UCHAR2:		return image::format::R8G8_UINT;
		case type::id::UCHAR3:		return image::format::R8G8B8_UINT;
		case type::id::UCHAR4:		return image::format::R8G8B8A8_UINT;
		case type::id::USHORT2:		return image::format::R16G16_UINT;
		case type::id::USHORT3:		return image::format::R16G16B16_UINT;
		case type::id::USHORT4:		return image::format::R16G16B16A16_UINT;
		case type::id::UINT2:		return image::format::R32G32_UINT;
		case type::id::UINT3:		return image::format::R32G32B32_UINT;
		case type::id::UINT4:		return image::format::R32G32B32A32_UINT;
		case type::id::CHAR2:		return image::format::R8G8_SINT;
		case type::id::CHAR3:		return image::format::R8G8B8_SINT;
		case type::id::CHAR4:		return image::format::R8G8B8A8_SINT;
		case type::id::SHORT2:		return image::format::R16G16_SINT;
		case type::id::SHORT3:		return image::format::R16G16B16_SINT;
		case type::id::SHORT4:		return image::format::R16G16B16A16_SINT;
		case type::id::INT2:		return image::format::R32G32_SINT;
		case type::id::INT3:		return image::format::R32G32B32_SINT;
		case type::id::INT4:		return image::format::R32G32B32A32_SINT;
		case type::id::FLOAT2:		return image::format::R32G32_SFLOAT;
		case type::id::FLOAT3:		return image::format::R32G32B32_SFLOAT;
		case type::id::FLOAT4:		return image::format::R32G32B32A32_SFLOAT;
			//case type::id::DOUBLE2:		return image::format::R64G64_SFLOAT;
			//case type::id::DOUBLE3:		return image::format::R64G64B64_SFLOAT;
			//case type::id::DOUBLE4:		return image::format::R64G64B64A64_SFLOAT;
			// Matrix Types (Must take multiple slots.)
			//case type::id::FLOAT2X2:	return image::format::UNDEFINED;
			//case type::id::FLOAT2X3:	return image::format::UNDEFINED;
			//case type::id::FLOAT2X4:	return image::format::UNDEFINED;
			//case type::id::FLOAT3X2:	return image::format::UNDEFINED;
			//case type::id::FLOAT3X3:	return image::format::UNDEFINED;
			//case type::id::FLOAT3X4:	return image::format::UNDEFINED;
			//case type::id::FLOAT4X2:	return image::format::UNDEFINED;
			//case type::id::FLOAT4X3:	return image::format::UNDEFINED;
			//case type::id::FLOAT4X4:	return image::format::UNDEFINED;
		}
	}

	size_t image::bytesperpixel(int aFormat) {
		return (image::bitsperpixel(aFormat) / 8);
	}

	size_t image::bitsperpixel(int aFormat) {

		switch (aFormat) {
		default: return 0;
		case VK_FORMAT_R4G4_UNORM_PACK8: return 8;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return 16;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return 16;
		case VK_FORMAT_R5G6B5_UNORM_PACK16: return 16;
		case VK_FORMAT_B5G6R5_UNORM_PACK16: return 16;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return 16;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return 16;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return 16;
		case VK_FORMAT_R8_UNORM: return 8;
		case VK_FORMAT_R8_SNORM: return 8;
		case VK_FORMAT_R8_USCALED: return 8;
		case VK_FORMAT_R8_SSCALED: return 8;
		case VK_FORMAT_R8_UINT: return 8;
		case VK_FORMAT_R8_SINT: return 8;
		case VK_FORMAT_R8_SRGB: return 8;
		case VK_FORMAT_R8G8_UNORM: return 16;
		case VK_FORMAT_R8G8_SNORM: return 16;
		case VK_FORMAT_R8G8_USCALED: return 16;
		case VK_FORMAT_R8G8_SSCALED: return 16;
		case VK_FORMAT_R8G8_UINT: return 16;
		case VK_FORMAT_R8G8_SINT: return 16;
		case VK_FORMAT_R8G8_SRGB: return 16;
		case VK_FORMAT_R8G8B8_UNORM: return 24;
		case VK_FORMAT_R8G8B8_SNORM: return 24;
		case VK_FORMAT_R8G8B8_USCALED: return 24;
		case VK_FORMAT_R8G8B8_SSCALED: return 24;
		case VK_FORMAT_R8G8B8_UINT: return 24;
		case VK_FORMAT_R8G8B8_SINT: return 24;
		case VK_FORMAT_R8G8B8_SRGB: return 24;
		case VK_FORMAT_B8G8R8_UNORM: return 24;
		case VK_FORMAT_B8G8R8_SNORM: return 24;
		case VK_FORMAT_B8G8R8_USCALED: return 24;
		case VK_FORMAT_B8G8R8_SSCALED: return 24;
		case VK_FORMAT_B8G8R8_UINT: return 24;
		case VK_FORMAT_B8G8R8_SINT: return 24;
		case VK_FORMAT_B8G8R8_SRGB: return 24;
		case VK_FORMAT_R8G8B8A8_UNORM: return 32;
		case VK_FORMAT_R8G8B8A8_SNORM: return 32;
		case VK_FORMAT_R8G8B8A8_USCALED: return 32;
		case VK_FORMAT_R8G8B8A8_SSCALED: return 32;
		case VK_FORMAT_R8G8B8A8_UINT: return 32;
		case VK_FORMAT_R8G8B8A8_SINT: return 32;
		case VK_FORMAT_R8G8B8A8_SRGB: return 32;
		case VK_FORMAT_B8G8R8A8_UNORM: return 32;
		case VK_FORMAT_B8G8R8A8_SNORM: return 32;
		case VK_FORMAT_B8G8R8A8_USCALED: return 32;
		case VK_FORMAT_B8G8R8A8_SSCALED: return 32;
		case VK_FORMAT_B8G8R8A8_UINT: return 32;
		case VK_FORMAT_B8G8R8A8_SINT: return 32;
		case VK_FORMAT_B8G8R8A8_SRGB: return 32;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32: return 32;
		case VK_FORMAT_R16_UNORM: return 16;
		case VK_FORMAT_R16_SNORM: return 16;
		case VK_FORMAT_R16_USCALED: return 16;
		case VK_FORMAT_R16_SSCALED: return 16;
		case VK_FORMAT_R16_UINT: return 16;
		case VK_FORMAT_R16_SINT: return 16;
		case VK_FORMAT_R16_SFLOAT: return 16;
		case VK_FORMAT_R16G16_UNORM: return 16;
		case VK_FORMAT_R16G16_SNORM: return 16;
		case VK_FORMAT_R16G16_USCALED: return 16;
		case VK_FORMAT_R16G16_SSCALED: return 16;
		case VK_FORMAT_R16G16_UINT: return 16;
		case VK_FORMAT_R16G16_SINT: return 16;
		case VK_FORMAT_R16G16_SFLOAT: return 16;
		case VK_FORMAT_R16G16B16_UNORM: return 48;
		case VK_FORMAT_R16G16B16_SNORM: return 48;
		case VK_FORMAT_R16G16B16_USCALED: return 48;
		case VK_FORMAT_R16G16B16_SSCALED: return 48;
		case VK_FORMAT_R16G16B16_UINT: return 48;
		case VK_FORMAT_R16G16B16_SINT: return 48;
		case VK_FORMAT_R16G16B16_SFLOAT: return 48;
		case VK_FORMAT_R16G16B16A16_UNORM: return 64;
		case VK_FORMAT_R16G16B16A16_SNORM: return 64;
		case VK_FORMAT_R16G16B16A16_USCALED: return 64;
		case VK_FORMAT_R16G16B16A16_SSCALED: return 64;
		case VK_FORMAT_R16G16B16A16_UINT: return 64;
		case VK_FORMAT_R16G16B16A16_SINT: return 64;
		case VK_FORMAT_R16G16B16A16_SFLOAT: return 64;
		case VK_FORMAT_R32_UINT: return 32;
		case VK_FORMAT_R32_SINT: return 32;
		case VK_FORMAT_R32_SFLOAT: return 32;
		case VK_FORMAT_R32G32_UINT: return 64;
		case VK_FORMAT_R32G32_SINT: return 64;
		case VK_FORMAT_R32G32_SFLOAT: return 64;
		case VK_FORMAT_R32G32B32_UINT: return 96;
		case VK_FORMAT_R32G32B32_SINT: return 96;
		case VK_FORMAT_R32G32B32_SFLOAT: return 96;
		case VK_FORMAT_R32G32B32A32_UINT: return 128;
		case VK_FORMAT_R32G32B32A32_SINT: return 128;
		case VK_FORMAT_R32G32B32A32_SFLOAT: return 128;
		case VK_FORMAT_R64_UINT: return 64;
		case VK_FORMAT_R64_SINT: return 64;
		case VK_FORMAT_R64_SFLOAT: return 64;
		case VK_FORMAT_R64G64_UINT: return 128;
		case VK_FORMAT_R64G64_SINT: return 128;
		case VK_FORMAT_R64G64_SFLOAT: return 128;
		case VK_FORMAT_R64G64B64_UINT: return 192;
		case VK_FORMAT_R64G64B64_SINT: return 192;
		case VK_FORMAT_R64G64B64_SFLOAT: return 192;
		case VK_FORMAT_R64G64B64A64_UINT: return 256;
		case VK_FORMAT_R64G64B64A64_SINT: return 256;
		case VK_FORMAT_R64G64B64A64_SFLOAT: return 256;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return 32;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return 32;
		case VK_FORMAT_D16_UNORM: return 16;
		case VK_FORMAT_X8_D24_UNORM_PACK32: return 32;
		case VK_FORMAT_D32_SFLOAT: return 32;
		case VK_FORMAT_S8_UINT: return 8;
		case VK_FORMAT_D16_UNORM_S8_UINT: return 24;
		case VK_FORMAT_D24_UNORM_S8_UINT: return 32;
		case VK_FORMAT_D32_SFLOAT_S8_UINT: return 40;
			// I might comment this section out.
			/*
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK                              : return 0;
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK                               : return 0;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_BC2_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC2_SRGB_BLOCK                                   : return 0;
		case VK_FORMAT_BC3_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC3_SRGB_BLOCK                                   : return 0;
		case VK_FORMAT_BC4_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC4_SNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC5_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC5_SNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC6H_UFLOAT_BLOCK                                : return 0;
		case VK_FORMAT_BC6H_SFLOAT_BLOCK                                : return 0;
		case VK_FORMAT_BC7_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC7_SRGB_BLOCK                                   : return 0;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK                          : return 0;
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK                           : return 0;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK                        : return 0;
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK                         : return 0;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK                        : return 0;
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK                         : return 0;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK                              : return 0;
		case VK_FORMAT_EAC_R11_SNORM_BLOCK                              : return 0;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK                            : return 0;
		case VK_FORMAT_G8B8G8R8_422_UNORM                               : return 0;
		case VK_FORMAT_B8G8R8G8_422_UNORM                               : return 0;
		case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM                        : return 0;
		case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM                         : return 0;
		case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM                        : return 0;
		case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM                         : return 0;
		case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM                        : return 0;
		case VK_FORMAT_R10X6_UNORM_PACK16                               : return 0;
		case VK_FORMAT_R10X6G10X6_UNORM_2PACK16                         : return 0;
		case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16               : return 0;
		case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16       : return 0;
		case VK_FORMAT_R12X4_UNORM_PACK16                               : return 0;
		case VK_FORMAT_R12X4G12X4_UNORM_2PACK16                         : return 0;
		case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16               : return 0;
		case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G16B16G16R16_422_UNORM                           : return 0;
		case VK_FORMAT_B16G16R16G16_422_UNORM                           : return 0;
		case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM                     : return 0;
		case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM                      : return 0;
		case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM                     : return 0;
		case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM                      : return 0;
		case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM                     : return 0;
		case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT                       : return 0;
		case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT                       : return 0;
		case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT                       : return 0;
		case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT                      : return 0;
		case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT                      : return 0;
		case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT                      : return 0;
		case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT                     : return 0;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT    : return 0;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT    : return 0;
		case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT                  : return 0;
		case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT                        : return 0;
		case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT                        : return 0;
			*/
		}
	}

	VkImageAspectFlags image::aspect_flag(int aFormat) {
		VkImageAspectFlags AspectFlag = 0;
		switch (aFormat) {
		default: 
			AspectFlag = 0;
			break;
		case VK_FORMAT_R4G4_UNORM_PACK8:
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16: 
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16: 
		case VK_FORMAT_R5G6B5_UNORM_PACK16: 
		case VK_FORMAT_B5G6R5_UNORM_PACK16: 
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16: 
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16: 
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16: 
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_USCALED:
		case VK_FORMAT_R8_SSCALED:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_SRGB:
		case VK_FORMAT_R8G8_UNORM: 
		case VK_FORMAT_R8G8_SNORM: 
		case VK_FORMAT_R8G8_USCALED: 
		case VK_FORMAT_R8G8_SSCALED: 
		case VK_FORMAT_R8G8_UINT: 
		case VK_FORMAT_R8G8_SINT: 
		case VK_FORMAT_R8G8_SRGB: 
		case VK_FORMAT_R8G8B8_UNORM: 
		case VK_FORMAT_R8G8B8_SNORM: 
		case VK_FORMAT_R8G8B8_USCALED: 
		case VK_FORMAT_R8G8B8_SSCALED: 
		case VK_FORMAT_R8G8B8_UINT: 
		case VK_FORMAT_R8G8B8_SINT: 
		case VK_FORMAT_R8G8B8_SRGB: 
		case VK_FORMAT_B8G8R8_UNORM: 
		case VK_FORMAT_B8G8R8_SNORM: 
		case VK_FORMAT_B8G8R8_USCALED: 
		case VK_FORMAT_B8G8R8_SSCALED: 
		case VK_FORMAT_B8G8R8_UINT: 
		case VK_FORMAT_B8G8R8_SINT: 
		case VK_FORMAT_B8G8R8_SRGB: 
		case VK_FORMAT_R8G8B8A8_UNORM: 
		case VK_FORMAT_R8G8B8A8_SNORM: 
		case VK_FORMAT_R8G8B8A8_USCALED: 
		case VK_FORMAT_R8G8B8A8_SSCALED: 
		case VK_FORMAT_R8G8B8A8_UINT: 
		case VK_FORMAT_R8G8B8A8_SINT: 
		case VK_FORMAT_R8G8B8A8_SRGB: 
		case VK_FORMAT_B8G8R8A8_UNORM: 
		case VK_FORMAT_B8G8R8A8_SNORM: 
		case VK_FORMAT_B8G8R8A8_USCALED: 
		case VK_FORMAT_B8G8R8A8_SSCALED: 
		case VK_FORMAT_B8G8R8A8_UINT: 
		case VK_FORMAT_B8G8R8A8_SINT: 
		case VK_FORMAT_B8G8R8A8_SRGB: 
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32: 
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32: 
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32: 
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: 
		case VK_FORMAT_A8B8G8R8_UINT_PACK32: 
		case VK_FORMAT_A8B8G8R8_SINT_PACK32: 
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32: 
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32: 
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32: 
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32: 
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: 
		case VK_FORMAT_A2R10G10B10_UINT_PACK32: 
		case VK_FORMAT_A2R10G10B10_SINT_PACK32: 
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32: 
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32: 
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32: 
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: 
		case VK_FORMAT_A2B10G10R10_UINT_PACK32: 
		case VK_FORMAT_A2B10G10R10_SINT_PACK32: 
		case VK_FORMAT_R16_UNORM: 
		case VK_FORMAT_R16_SNORM: 
		case VK_FORMAT_R16_USCALED: 
		case VK_FORMAT_R16_SSCALED: 
		case VK_FORMAT_R16_UINT: 
		case VK_FORMAT_R16_SINT: 
		case VK_FORMAT_R16_SFLOAT: 
		case VK_FORMAT_R16G16_UNORM: 
		case VK_FORMAT_R16G16_SNORM: 
		case VK_FORMAT_R16G16_USCALED: 
		case VK_FORMAT_R16G16_SSCALED: 
		case VK_FORMAT_R16G16_UINT: 
		case VK_FORMAT_R16G16_SINT: 
		case VK_FORMAT_R16G16_SFLOAT: 
		case VK_FORMAT_R16G16B16_UNORM: 
		case VK_FORMAT_R16G16B16_SNORM: 
		case VK_FORMAT_R16G16B16_USCALED: 
		case VK_FORMAT_R16G16B16_SSCALED: 
		case VK_FORMAT_R16G16B16_UINT: 
		case VK_FORMAT_R16G16B16_SINT: 
		case VK_FORMAT_R16G16B16_SFLOAT: 
		case VK_FORMAT_R16G16B16A16_UNORM: 
		case VK_FORMAT_R16G16B16A16_SNORM: 
		case VK_FORMAT_R16G16B16A16_USCALED: 
		case VK_FORMAT_R16G16B16A16_SSCALED: 
		case VK_FORMAT_R16G16B16A16_UINT: 
		case VK_FORMAT_R16G16B16A16_SINT: 
		case VK_FORMAT_R16G16B16A16_SFLOAT: 
		case VK_FORMAT_R32_UINT: 
		case VK_FORMAT_R32_SINT: 
		case VK_FORMAT_R32_SFLOAT: 
		case VK_FORMAT_R32G32_UINT: 
		case VK_FORMAT_R32G32_SINT: 
		case VK_FORMAT_R32G32_SFLOAT: 
		case VK_FORMAT_R32G32B32_UINT: 
		case VK_FORMAT_R32G32B32_SINT: 
		case VK_FORMAT_R32G32B32_SFLOAT: 
		case VK_FORMAT_R32G32B32A32_UINT: 
		case VK_FORMAT_R32G32B32A32_SINT: 
		case VK_FORMAT_R32G32B32A32_SFLOAT: 
		case VK_FORMAT_R64_UINT: 
		case VK_FORMAT_R64_SINT: 
		case VK_FORMAT_R64_SFLOAT: 
		case VK_FORMAT_R64G64_UINT: 
		case VK_FORMAT_R64G64_SINT: 
		case VK_FORMAT_R64G64_SFLOAT: 
		case VK_FORMAT_R64G64B64_UINT: 
		case VK_FORMAT_R64G64B64_SINT: 
		case VK_FORMAT_R64G64B64_SFLOAT: 
		case VK_FORMAT_R64G64B64A64_UINT: 
		case VK_FORMAT_R64G64B64A64_SINT: 
		case VK_FORMAT_R64G64B64A64_SFLOAT: 
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32: 
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
			AspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case VK_FORMAT_D16_UNORM: 
		case VK_FORMAT_X8_D24_UNORM_PACK32: 
		case VK_FORMAT_D32_SFLOAT: 
			AspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		case VK_FORMAT_D16_UNORM_S8_UINT: 
		case VK_FORMAT_D24_UNORM_S8_UINT: 
		case VK_FORMAT_D32_SFLOAT_S8_UINT: 
		case VK_FORMAT_S8_UINT:
			AspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			break;
		}
		return AspectFlag;
	}

	image::image() {
		
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, uint aX, uint aY, uint aZ, uint aT, void* aTextureData) {
		VkResult Result = VK_SUCCESS;

		this->zero_out();

		// Image Handle Info
		this->Context					= aContext;
		this->Handle 					= VK_NULL_HANDLE;

		this->MemoryType 				= aCreateInfo.Memory;
		this->MemoryHandle 				= VK_NULL_HANDLE;

		VkImageCreateInfo CreateInfo {};
		this->CreateInfo.sType						= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		this->CreateInfo.pNext						= NULL;
		this->CreateInfo.flags						= 0;
		if ((aY == 1) && (aZ == 1)) {
			// 1D Image
			this->CreateInfo.imageType 					= VK_IMAGE_TYPE_1D;
		} else if (aZ == 1) {
			// 2D Image
			this->CreateInfo.imageType 					= VK_IMAGE_TYPE_2D;
		} else {
			// 3D Image
			this->CreateInfo.imageType 					= VK_IMAGE_TYPE_3D;
		}
		this->CreateInfo.format						= (VkFormat)aFormat;
		this->CreateInfo.extent						= { aX, aY, aZ };
		this->CreateInfo.mipLevels					= std::floor(std::log2(std::max(std::max(aX, aY), aZ))) + 1;
		this->CreateInfo.arrayLayers				= aT;
		this->CreateInfo.samples					= (VkSampleCountFlagBits)aCreateInfo.Sample;
		this->CreateInfo.tiling						= (VkImageTiling)aCreateInfo.Tiling;
		this->CreateInfo.usage						= (VkImageUsageFlags)aCreateInfo.Usage;
		this->CreateInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
		this->CreateInfo.queueFamilyIndexCount		= 0;
		this->CreateInfo.pQueueFamilyIndices		= NULL;
		this->CreateInfo.initialLayout				= VK_IMAGE_LAYOUT_UNDEFINED;

		Result = vkCreateImage(aContext->handle(), &CreateInfo, NULL, &this->Handle);
		if (Result != VK_SUCCESS) {
			// TODO: Error handling
			throw std::runtime_error("Failed to create image.");
		}

		// Get memory requirements of the image object.
		VkMemoryRequirements MemoryRequirements = this->Context->get_image_memory_requirements(this->Handle);

		// Find the memory index for the heap that best suits the memory requirements, and desired memory properties.
		this->MemoryHandle = this->Context->allocate_memory(MemoryRequirements, this->MemoryType);

		// Bind the image object to the memory object.
		Result = vkBindImageMemory(this->Context->handle(), this->Handle, this->MemoryHandle, 0);

		// Generate Mipmaps

	}

	// Copy Constructor.
	image::image(image& aInput) : 
	image(
		aInput.Context, 
		create_info(aInput.CreateInfo.samples, aInput.CreateInfo.tiling, this->MemoryType, aInput.CreateInfo.usage), 
		(format)aInput.CreateInfo.format, 
		aInput.CreateInfo.extent.width, aInput.CreateInfo.extent.height, aInput.CreateInfo.extent.depth, aInput.CreateInfo.arrayLayers, 
		NULL
	) {
		std::vector<VkImageCopy> RegionList;
		for (uint32_t i = 0; i < aInput.CreateInfo.mipLevels; i++) {
			VkImageCopy Region;
			Region.srcSubresource.aspectMask				= VK_IMAGE_ASPECT_COLOR_BIT;
			Region.srcSubresource.mipLevel					= i;
			Region.srcSubresource.baseArrayLayer			= 0;
			Region.srcSubresource.layerCount				= aInput.CreateInfo.arrayLayers;
			Region.srcOffset								= { 0, 0, 0 };
			Region.srcSubresource.aspectMask				= VK_IMAGE_ASPECT_COLOR_BIT;
			Region.srcSubresource.mipLevel					= i;
			Region.srcSubresource.baseArrayLayer			= 0;
			Region.srcSubresource.layerCount				= aInput.CreateInfo.arrayLayers;
			Region.dstOffset								= { 0, 0, 0 };
			Region.extent									= { aInput.CreateInfo.extent.width, aInput.CreateInfo.extent.height, aInput.CreateInfo.extent.depth };
			RegionList.push_back(Region);
		}

		// Copy the image.
		command_list CommandList = this->copy(aInput, RegionList);

		// Execute transfer operations.
		this->Context->execute_and_wait(device::operation::TRANSFER, CommandList);

	}

	// Move Constructor.
	image::image(image&& aInput) noexcept {
		this->Context = aInput.Context;
		this->CreateInfo = aInput.CreateInfo;
		this->Handle = aInput.Handle;

		this->MemoryType = aInput.MemoryType;
		this->MemoryHandle = aInput.MemoryHandle;

		this->Extent = aInput.Extent;
		this->Layout = aInput.Layout;
	}

	// Destructor
	image::~image() {
		this->clear();
	}

	// Copy Assignment.
	image& image::operator=(image& aRhs) {

	}

	// Move Assignment.
	image& image::operator=(image&& aRhs) noexcept {
		this->clear();
	}

	// Device Operation Support: T.
	command_list image::copy(buffer& aSourceData, VkBufferImageCopy aRegion) {
		std::vector<VkBufferImageCopy> RegionList = { aRegion };
		return this->copy(aSourceData, RegionList);
	}

	command_list image::copy(buffer& aSourceData, std::vector<VkBufferImageCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		command_list CommandList = this->Context->create_command_list(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
		VkCommandBufferBeginInfo BeginInfo {};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = 0;
		BeginInfo.pInheritanceInfo = NULL;
		Result = vkBeginCommandBuffer(CommandList[0], &BeginInfo);
		vkCmdCopyBufferToImage(CommandList[0], aSourceData.handle(), this->Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aRegionList.size(), aRegionList.data());
		Result = vkEndCommandBuffer(CommandList[0]);		
		return CommandList;
	}

	command_list image::copy(image& aSourceData, VkImageCopy aRegion) {
		std::vector<VkImageCopy> RegionList = { aRegion };
		return this->copy(aSourceData, RegionList);
	}

	command_list image::copy(image& aSourceData, std::vector<VkImageCopy> aRegionList) {
		VkResult Result = VK_SUCCESS;
		command_list CommandList = this->Context->create_command_list(device::operation::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
		VkCommandBufferBeginInfo BeginInfo {};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = 0;
		BeginInfo.pInheritanceInfo = NULL;
		Result = vkBeginCommandBuffer(CommandList[0], &BeginInfo);
		vkCmdCopyImage(CommandList[0], aSourceData.handle(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, this->Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, aRegionList.size(), aRegionList.data());
		Result = vkEndCommandBuffer(CommandList[0]);		
		return CommandList;
	}

	// Device Operation: T, G, C, D, E.
	command_list image::transition(
		VkAccessFlags aSrcAccessMask, VkAccessFlags aDstAccessMask,
		VkPipelineStageFlags aSrcStage, VkPipelineStageFlags aDstStage,
		VkImageLayout aNewLayout
	) {

	}

	command_list image::transition(
		VkAccessFlags aSrcAccessMask, VkAccessFlags aDstAccessMask,
		VkPipelineStageFlags aSrcStage, VkPipelineStageFlags aDstStage,
		uint32_t aMipLevel, uint32_t aMipLevelCount,
		uint32_t aArrayLayer, uint32_t aArrayLayerCount,
		VkImageLayout aNewLayout
	) {

		VkImageMemoryBarrier Barrier {};
		//vkCmdPipelineBarrier()
	}

	void image::zero_out() {
		this->Context			= nullptr;
		this->CreateInfo		= {};
		this->Handle			= VK_NULL_HANDLE;
		this->MemoryType		= 0;
		this->MemoryHandle		= VK_NULL_HANDLE;
		this->Extent.clear();
		this->Layout.clear();
	}

	void image::clear() {
		this->Context->free_memory(this->MemoryHandle);
		vkDestroyImage(this->Context->handle(), this->Handle, NULL);
		this->zero_out();
	}

}
