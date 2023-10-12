#pragma once
#ifndef GEODESUKA_CORE_GCL_IMAGE_H
#define GEODESUKA_CORE_GCL_IMAGE_H

#include "../../config.h"

#include "../io/file.h"

#include "variable.h"

#include "device.h"
#include "context.h"

#include "buffer.h"

namespace geodesuka::core::object {
	class system_window;
}

namespace geodesuka::core::gcl {

	class image : public io::file {
	public:

		friend class geodesuka::engine;
		friend class buffer;
		friend class object::system_window;

		enum sample {
      		COUNT_1  										= VK_SAMPLE_COUNT_1_BIT,
      		COUNT_2  										= VK_SAMPLE_COUNT_2_BIT,
      		COUNT_4  										= VK_SAMPLE_COUNT_4_BIT,
      		COUNT_8  										= VK_SAMPLE_COUNT_8_BIT,
      		COUNT_16 										= VK_SAMPLE_COUNT_16_BIT,
      		COUNT_32 										= VK_SAMPLE_COUNT_32_BIT,
      		COUNT_64 										= VK_SAMPLE_COUNT_64_BIT,
		};

		enum tiling {
      		OPTIMAL                 						= VK_IMAGE_TILING_OPTIMAL,
      		LINEAR                  						= VK_IMAGE_TILING_LINEAR,
      		DRM_FORMAT_MODIFIER_EXT 						= VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT,
		};

		enum usage : int {
			TRANSFER_SRC 									= VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			TRANSFER_DST 									= VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			SAMPLED 										= VK_IMAGE_USAGE_SAMPLED_BIT,
			STORAGE 										= VK_IMAGE_USAGE_STORAGE_BIT,
			COLOR_ATTACHMENT 								= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			DEPTH_STENCIL_ATTACHMENT 						= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			TRANSIENT_ATTACHMENT 							= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
			INPUT_ATTACHMENT 								= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VIDEO_DECODE_DST 								= VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR,
			VIDEO_DECODE_SRC 								= VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR,
			VIDEO_DECODE_DPB 								= VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR,
			FRAGMENT_DENSITY_MAP 							= VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT,
			FRAGMENT_SHADING_RATE_ATTACHMENT 				= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR,
		};

		enum format {			
			UNDEFINED                                  		= VK_FORMAT_UNDEFINED,
			R4G4_UNORM_PACK8                           		= VK_FORMAT_R4G4_UNORM_PACK8,
			R4G4B4A4_UNORM_PACK16                      		= VK_FORMAT_R4G4B4A4_UNORM_PACK16,
			B4G4R4A4_UNORM_PACK16                      		= VK_FORMAT_B4G4R4A4_UNORM_PACK16,
			R5G6B5_UNORM_PACK16                        		= VK_FORMAT_R5G6B5_UNORM_PACK16,
			B5G6R5_UNORM_PACK16                        		= VK_FORMAT_B5G6R5_UNORM_PACK16,
			R5G5B5A1_UNORM_PACK16                      		= VK_FORMAT_R5G5B5A1_UNORM_PACK16,
			B5G5R5A1_UNORM_PACK16                      		= VK_FORMAT_B5G5R5A1_UNORM_PACK16,
			A1R5G5B5_UNORM_PACK16                      		= VK_FORMAT_A1R5G5B5_UNORM_PACK16,
			R8_UNORM                                   		= VK_FORMAT_R8_UNORM,
			R8_SNORM                                   		= VK_FORMAT_R8_SNORM,
			R8_USCALED                                 		= VK_FORMAT_R8_USCALED,
			R8_SSCALED                                 		= VK_FORMAT_R8_SSCALED,
			R8_UINT                                    		= VK_FORMAT_R8_UINT,
			R8_SINT                                    		= VK_FORMAT_R8_SINT,
			R8_SRGB                                    		= VK_FORMAT_R8_SRGB,
			R8G8_UNORM                                 		= VK_FORMAT_R8G8_UNORM,
			R8G8_SNORM                                 		= VK_FORMAT_R8G8_SNORM,
			R8G8_USCALED                               		= VK_FORMAT_R8G8_USCALED,
			R8G8_SSCALED                               		= VK_FORMAT_R8G8_SSCALED,
			R8G8_UINT                                  		= VK_FORMAT_R8G8_UINT,
			R8G8_SINT                                  		= VK_FORMAT_R8G8_SINT,
			R8G8_SRGB                                  		= VK_FORMAT_R8G8_SRGB,
			R8G8B8_UNORM                               		= VK_FORMAT_R8G8B8_UNORM,
			R8G8B8_SNORM                               		= VK_FORMAT_R8G8B8_SNORM,
			R8G8B8_USCALED                             		= VK_FORMAT_R8G8B8_USCALED,
			R8G8B8_SSCALED                             		= VK_FORMAT_R8G8B8_SSCALED,
			R8G8B8_UINT                                		= VK_FORMAT_R8G8B8_UINT,
			R8G8B8_SINT                                		= VK_FORMAT_R8G8B8_SINT,
			R8G8B8_SRGB                                		= VK_FORMAT_R8G8B8_SRGB,
			B8G8R8_UNORM                               		= VK_FORMAT_B8G8R8_UNORM,
			B8G8R8_SNORM                               		= VK_FORMAT_B8G8R8_SNORM,
			B8G8R8_USCALED                             		= VK_FORMAT_B8G8R8_USCALED,
			B8G8R8_SSCALED                             		= VK_FORMAT_B8G8R8_SSCALED,
			B8G8R8_UINT                                		= VK_FORMAT_B8G8R8_UINT,
			B8G8R8_SINT                                		= VK_FORMAT_B8G8R8_SINT,
			B8G8R8_SRGB                                		= VK_FORMAT_B8G8R8_SRGB,
			R8G8B8A8_UNORM                             		= VK_FORMAT_R8G8B8A8_UNORM,
			R8G8B8A8_SNORM                             		= VK_FORMAT_R8G8B8A8_SNORM,
			R8G8B8A8_USCALED                           		= VK_FORMAT_R8G8B8A8_USCALED,
			R8G8B8A8_SSCALED                           		= VK_FORMAT_R8G8B8A8_SSCALED,
			R8G8B8A8_UINT                              		= VK_FORMAT_R8G8B8A8_UINT,
			R8G8B8A8_SINT                              		= VK_FORMAT_R8G8B8A8_SINT,
			R8G8B8A8_SRGB                              		= VK_FORMAT_R8G8B8A8_SRGB,
			B8G8R8A8_UNORM                             		= VK_FORMAT_B8G8R8A8_UNORM,
			B8G8R8A8_SNORM                             		= VK_FORMAT_B8G8R8A8_SNORM,
			B8G8R8A8_USCALED                           		= VK_FORMAT_B8G8R8A8_USCALED,
			B8G8R8A8_SSCALED                           		= VK_FORMAT_B8G8R8A8_SSCALED,
			B8G8R8A8_UINT                              		= VK_FORMAT_B8G8R8A8_UINT,
			B8G8R8A8_SINT                              		= VK_FORMAT_B8G8R8A8_SINT,
			B8G8R8A8_SRGB                              		= VK_FORMAT_B8G8R8A8_SRGB,
			A8B8G8R8_UNORM_PACK32                      		= VK_FORMAT_A8B8G8R8_UNORM_PACK32,
			A8B8G8R8_SNORM_PACK32                      		= VK_FORMAT_A8B8G8R8_SNORM_PACK32,
			A8B8G8R8_USCALED_PACK32                    		= VK_FORMAT_A8B8G8R8_USCALED_PACK32,
			A8B8G8R8_SSCALED_PACK32                    		= VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
			A8B8G8R8_UINT_PACK32                       		= VK_FORMAT_A8B8G8R8_UINT_PACK32,
			A8B8G8R8_SINT_PACK32                       		= VK_FORMAT_A8B8G8R8_SINT_PACK32,
			A8B8G8R8_SRGB_PACK32                       		= VK_FORMAT_A8B8G8R8_SRGB_PACK32,
			A2R10G10B10_UNORM_PACK32                   		= VK_FORMAT_A2R10G10B10_UNORM_PACK32,
			A2R10G10B10_SNORM_PACK32                   		= VK_FORMAT_A2R10G10B10_SNORM_PACK32,
			A2R10G10B10_USCALED_PACK32                 		= VK_FORMAT_A2R10G10B10_USCALED_PACK32,
			A2R10G10B10_SSCALED_PACK32                 		= VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
			A2R10G10B10_UINT_PACK32                    		= VK_FORMAT_A2R10G10B10_UINT_PACK32,
			A2R10G10B10_SINT_PACK32                    		= VK_FORMAT_A2R10G10B10_SINT_PACK32,
			A2B10G10R10_UNORM_PACK32                   		= VK_FORMAT_A2B10G10R10_UNORM_PACK32,
			A2B10G10R10_SNORM_PACK32                   		= VK_FORMAT_A2B10G10R10_SNORM_PACK32,
			A2B10G10R10_USCALED_PACK32                 		= VK_FORMAT_A2B10G10R10_USCALED_PACK32,
			A2B10G10R10_SSCALED_PACK32                 		= VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
			A2B10G10R10_UINT_PACK32                    		= VK_FORMAT_A2B10G10R10_UINT_PACK32,
			A2B10G10R10_SINT_PACK32                    		= VK_FORMAT_A2B10G10R10_SINT_PACK32,
			R16_UNORM                                  		= VK_FORMAT_R16_UNORM,
			R16_SNORM                                  		= VK_FORMAT_R16_SNORM,
			R16_USCALED                                		= VK_FORMAT_R16_USCALED,
			R16_SSCALED                                		= VK_FORMAT_R16_SSCALED,
			R16_UINT                                   		= VK_FORMAT_R16_UINT,
			R16_SINT                                   		= VK_FORMAT_R16_SINT,
			R16_SFLOAT                                 		= VK_FORMAT_R16_SFLOAT,
			R16G16_UNORM                               		= VK_FORMAT_R16G16_UNORM,
			R16G16_SNORM                               		= VK_FORMAT_R16G16_SNORM,
			R16G16_USCALED                             		= VK_FORMAT_R16G16_USCALED,
			R16G16_SSCALED                             		= VK_FORMAT_R16G16_SSCALED,
			R16G16_UINT                                		= VK_FORMAT_R16G16_UINT,
			R16G16_SINT                                		= VK_FORMAT_R16G16_SINT,
			R16G16_SFLOAT                              		= VK_FORMAT_R16G16_SFLOAT,
			R16G16B16_UNORM                            		= VK_FORMAT_R16G16B16_UNORM,
			R16G16B16_SNORM                            		= VK_FORMAT_R16G16B16_SNORM,
			R16G16B16_USCALED                          		= VK_FORMAT_R16G16B16_USCALED,
			R16G16B16_SSCALED                          		= VK_FORMAT_R16G16B16_SSCALED,
			R16G16B16_UINT                             		= VK_FORMAT_R16G16B16_UINT,
			R16G16B16_SINT                             		= VK_FORMAT_R16G16B16_SINT,
			R16G16B16_SFLOAT                           		= VK_FORMAT_R16G16B16_SFLOAT,
			R16G16B16A16_UNORM                         		= VK_FORMAT_R16G16B16A16_UNORM,
			R16G16B16A16_SNORM                         		= VK_FORMAT_R16G16B16A16_SNORM,
			R16G16B16A16_USCALED                       		= VK_FORMAT_R16G16B16A16_USCALED,
			R16G16B16A16_SSCALED                       		= VK_FORMAT_R16G16B16A16_SSCALED,
			R16G16B16A16_UINT                          		= VK_FORMAT_R16G16B16A16_UINT,
			R16G16B16A16_SINT                          		= VK_FORMAT_R16G16B16A16_SINT,
			R16G16B16A16_SFLOAT                        		= VK_FORMAT_R16G16B16A16_SFLOAT,
			R32_UINT                                   		= VK_FORMAT_R32_UINT,
			R32_SINT                                   		= VK_FORMAT_R32_SINT,
			R32_SFLOAT                                 		= VK_FORMAT_R32_SFLOAT,
			R32G32_UINT                                		= VK_FORMAT_R32G32_UINT,
			R32G32_SINT                                		= VK_FORMAT_R32G32_SINT,
			R32G32_SFLOAT                              		= VK_FORMAT_R32G32_SFLOAT,
			R32G32B32_UINT                             		= VK_FORMAT_R32G32B32_UINT,
			R32G32B32_SINT                             		= VK_FORMAT_R32G32B32_SINT,
			R32G32B32_SFLOAT                           		= VK_FORMAT_R32G32B32_SFLOAT,
			R32G32B32A32_UINT                          		= VK_FORMAT_R32G32B32A32_UINT,
			R32G32B32A32_SINT                          		= VK_FORMAT_R32G32B32A32_SINT,
			R32G32B32A32_SFLOAT                        		= VK_FORMAT_R32G32B32A32_SFLOAT,
			R64_UINT                                   		= VK_FORMAT_R64_UINT,
			R64_SINT                                   		= VK_FORMAT_R64_SINT,
			R64_SFLOAT                                 		= VK_FORMAT_R64_SFLOAT,
			R64G64_UINT                                		= VK_FORMAT_R64G64_UINT,
			R64G64_SINT                                		= VK_FORMAT_R64G64_SINT,
			R64G64_SFLOAT                              		= VK_FORMAT_R64G64_SFLOAT,
			R64G64B64_UINT                             		= VK_FORMAT_R64G64B64_UINT,
			R64G64B64_SINT                             		= VK_FORMAT_R64G64B64_SINT,
			R64G64B64_SFLOAT                           		= VK_FORMAT_R64G64B64_SFLOAT,
			R64G64B64A64_UINT                          		= VK_FORMAT_R64G64B64A64_UINT,
			R64G64B64A64_SINT                          		= VK_FORMAT_R64G64B64A64_SINT,
			R64G64B64A64_SFLOAT                        		= VK_FORMAT_R64G64B64A64_SFLOAT,
			B10G11R11_UFLOAT_PACK32                    		= VK_FORMAT_B10G11R11_UFLOAT_PACK32,
			E5B9G9R9_UFLOAT_PACK32                     		= VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
			D16_UNORM                                  		= VK_FORMAT_D16_UNORM,
			X8_D24_UNORM_PACK32                        		= VK_FORMAT_X8_D24_UNORM_PACK32,
			D32_SFLOAT                                 		= VK_FORMAT_D32_SFLOAT,
			S8_UINT                                    		= VK_FORMAT_S8_UINT,
			D16_UNORM_S8_UINT                          		= VK_FORMAT_D16_UNORM_S8_UINT,
			D24_UNORM_S8_UINT                          		= VK_FORMAT_D24_UNORM_S8_UINT,
			D32_SFLOAT_S8_UINT                         		= VK_FORMAT_D32_SFLOAT_S8_UINT,
			BC1_RGB_UNORM_BLOCK                        		= VK_FORMAT_BC1_RGB_UNORM_BLOCK,
			BC1_RGB_SRGB_BLOCK                         		= VK_FORMAT_BC1_RGB_SRGB_BLOCK,
			BC1_RGBA_UNORM_BLOCK                       		= VK_FORMAT_BC1_RGBA_UNORM_BLOCK,
			BC1_RGBA_SRGB_BLOCK                        		= VK_FORMAT_BC1_RGBA_SRGB_BLOCK,
			BC2_UNORM_BLOCK                            		= VK_FORMAT_BC2_UNORM_BLOCK,
			BC2_SRGB_BLOCK                             		= VK_FORMAT_BC2_SRGB_BLOCK,
			BC3_UNORM_BLOCK                            		= VK_FORMAT_BC3_UNORM_BLOCK,
			BC3_SRGB_BLOCK                             		= VK_FORMAT_BC3_SRGB_BLOCK,
			BC4_UNORM_BLOCK                            		= VK_FORMAT_BC4_UNORM_BLOCK,
			BC4_SNORM_BLOCK                            		= VK_FORMAT_BC4_SNORM_BLOCK,
			BC5_UNORM_BLOCK                            		= VK_FORMAT_BC5_UNORM_BLOCK,
			BC5_SNORM_BLOCK                            		= VK_FORMAT_BC5_SNORM_BLOCK,
			BC6H_UFLOAT_BLOCK                          		= VK_FORMAT_BC6H_UFLOAT_BLOCK,
			BC6H_SFLOAT_BLOCK                          		= VK_FORMAT_BC6H_SFLOAT_BLOCK,
			BC7_UNORM_BLOCK                            		= VK_FORMAT_BC7_UNORM_BLOCK,
			BC7_SRGB_BLOCK                             		= VK_FORMAT_BC7_SRGB_BLOCK,
			ETC2_R8G8B8_UNORM_BLOCK                    		= VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
			ETC2_R8G8B8_SRGB_BLOCK                     		= VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
			ETC2_R8G8B8A1_UNORM_BLOCK                  		= VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
			ETC2_R8G8B8A1_SRGB_BLOCK                   		= VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
			ETC2_R8G8B8A8_UNORM_BLOCK                  		= VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
			ETC2_R8G8B8A8_SRGB_BLOCK                   		= VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
			EAC_R11_UNORM_BLOCK                        		= VK_FORMAT_EAC_R11_UNORM_BLOCK,
			EAC_R11_SNORM_BLOCK                        		= VK_FORMAT_EAC_R11_SNORM_BLOCK,
			EAC_R11G11_UNORM_BLOCK                     		= VK_FORMAT_EAC_R11G11_UNORM_BLOCK,
			EAC_R11G11_SNORM_BLOCK                     		= VK_FORMAT_EAC_R11G11_SNORM_BLOCK,
			ASTC_4x4_UNORM_BLOCK                       		= VK_FORMAT_ASTC_4x4_UNORM_BLOCK,
			ASTC_4x4_SRGB_BLOCK                        		= VK_FORMAT_ASTC_4x4_SRGB_BLOCK,
			ASTC_5x4_UNORM_BLOCK                       		= VK_FORMAT_ASTC_5x4_UNORM_BLOCK,
			ASTC_5x4_SRGB_BLOCK                        		= VK_FORMAT_ASTC_5x4_SRGB_BLOCK,
			ASTC_5x5_UNORM_BLOCK                       		= VK_FORMAT_ASTC_5x5_UNORM_BLOCK,
			ASTC_5x5_SRGB_BLOCK                        		= VK_FORMAT_ASTC_5x5_SRGB_BLOCK,
			ASTC_6x5_UNORM_BLOCK                       		= VK_FORMAT_ASTC_6x5_UNORM_BLOCK,
			ASTC_6x5_SRGB_BLOCK                        		= VK_FORMAT_ASTC_6x5_SRGB_BLOCK,
			ASTC_6x6_UNORM_BLOCK                       		= VK_FORMAT_ASTC_6x6_UNORM_BLOCK,
			ASTC_6x6_SRGB_BLOCK                        		= VK_FORMAT_ASTC_6x6_SRGB_BLOCK,
			ASTC_8x5_UNORM_BLOCK                       		= VK_FORMAT_ASTC_8x5_UNORM_BLOCK,
			ASTC_8x5_SRGB_BLOCK                        		= VK_FORMAT_ASTC_8x5_SRGB_BLOCK,
			ASTC_8x6_UNORM_BLOCK                       		= VK_FORMAT_ASTC_8x6_UNORM_BLOCK,
			ASTC_8x6_SRGB_BLOCK                        		= VK_FORMAT_ASTC_8x6_SRGB_BLOCK,
			ASTC_8x8_UNORM_BLOCK                       		= VK_FORMAT_ASTC_8x8_UNORM_BLOCK,
			ASTC_8x8_SRGB_BLOCK                        		= VK_FORMAT_ASTC_8x8_SRGB_BLOCK,
			ASTC_10x5_UNORM_BLOCK                      		= VK_FORMAT_ASTC_10x5_UNORM_BLOCK,
			ASTC_10x5_SRGB_BLOCK                       		= VK_FORMAT_ASTC_10x5_SRGB_BLOCK,
			ASTC_10x6_UNORM_BLOCK                      		= VK_FORMAT_ASTC_10x6_UNORM_BLOCK,
			ASTC_10x6_SRGB_BLOCK                       		= VK_FORMAT_ASTC_10x6_SRGB_BLOCK,
			ASTC_10x8_UNORM_BLOCK                      		= VK_FORMAT_ASTC_10x8_UNORM_BLOCK,
			ASTC_10x8_SRGB_BLOCK                       		= VK_FORMAT_ASTC_10x8_SRGB_BLOCK,
			ASTC_10x10_UNORM_BLOCK                     		= VK_FORMAT_ASTC_10x10_UNORM_BLOCK,
			ASTC_10x10_SRGB_BLOCK                      		= VK_FORMAT_ASTC_10x10_SRGB_BLOCK,
			ASTC_12x10_UNORM_BLOCK                     		= VK_FORMAT_ASTC_12x10_UNORM_BLOCK,
			ASTC_12x10_SRGB_BLOCK                      		= VK_FORMAT_ASTC_12x10_SRGB_BLOCK,
			ASTC_12x12_UNORM_BLOCK                     		= VK_FORMAT_ASTC_12x12_UNORM_BLOCK,
			ASTC_12x12_SRGB_BLOCK                      		= VK_FORMAT_ASTC_12x12_SRGB_BLOCK,
			G8B8G8R8_422_UNORM                         		= VK_FORMAT_G8B8G8R8_422_UNORM,
			B8G8R8G8_422_UNORM                         		= VK_FORMAT_B8G8R8G8_422_UNORM,
			G8_B8_R8_3PLANE_420_UNORM                  		= VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM,
			G8_B8R8_2PLANE_420_UNORM                   		= VK_FORMAT_G8_B8R8_2PLANE_420_UNORM,
			G8_B8_R8_3PLANE_422_UNORM                  		= VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM,
			G8_B8R8_2PLANE_422_UNORM                   		= VK_FORMAT_G8_B8R8_2PLANE_422_UNORM,
			G8_B8_R8_3PLANE_444_UNORM                  		= VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM,
			R10X6_UNORM_PACK16                         		= VK_FORMAT_R10X6_UNORM_PACK16,
			R10X6G10X6_UNORM_2PACK16                   		= VK_FORMAT_R10X6G10X6_UNORM_2PACK16,
			R10X6G10X6B10X6A10X6_UNORM_4PACK16         		= VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16,
			G10X6B10X6G10X6R10X6_422_UNORM_4PACK16     		= VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16,
			B10X6G10X6R10X6G10X6_422_UNORM_4PACK16     		= VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16,
			G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 		= VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16,
			G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16  		= VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16,
			G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 		= VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16,
			G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16  		= VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16,
			G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 		= VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16,
			R12X4_UNORM_PACK16                         		= VK_FORMAT_R12X4_UNORM_PACK16,
			R12X4G12X4_UNORM_2PACK16                   		= VK_FORMAT_R12X4G12X4_UNORM_2PACK16,
			R12X4G12X4B12X4A12X4_UNORM_4PACK16         		= VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16,
			G12X4B12X4G12X4R12X4_422_UNORM_4PACK16     		= VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16,
			B12X4G12X4R12X4G12X4_422_UNORM_4PACK16     		= VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16,
			G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 		= VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16,
			G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16  		= VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16,
			G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 		= VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16,
			G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16  		= VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16,
			G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 		= VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16,
			G16B16G16R16_422_UNORM                     		= VK_FORMAT_G16B16G16R16_422_UNORM,
			B16G16R16G16_422_UNORM                     		= VK_FORMAT_B16G16R16G16_422_UNORM,
			G16_B16_R16_3PLANE_420_UNORM               		= VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM,
			G16_B16R16_2PLANE_420_UNORM                		= VK_FORMAT_G16_B16R16_2PLANE_420_UNORM,
			G16_B16_R16_3PLANE_422_UNORM               		= VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM,
			G16_B16R16_2PLANE_422_UNORM                		= VK_FORMAT_G16_B16R16_2PLANE_422_UNORM,
			G16_B16_R16_3PLANE_444_UNORM               		= VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM,
			G8_B8R8_2PLANE_444_UNORM                   		= VK_FORMAT_G8_B8R8_2PLANE_444_UNORM,
			G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16  		= VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16,
			G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16  		= VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16,
			G16_B16R16_2PLANE_444_UNORM                		= VK_FORMAT_G16_B16R16_2PLANE_444_UNORM,
			A4R4G4B4_UNORM_PACK16                      		= VK_FORMAT_A4R4G4B4_UNORM_PACK16,
			A4B4G4R4_UNORM_PACK16                      		= VK_FORMAT_A4B4G4R4_UNORM_PACK16,
			ASTC_4x4_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK,
			ASTC_5x4_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK,
			ASTC_5x5_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK,
			ASTC_6x5_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK,
			ASTC_6x6_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK,
			ASTC_8x5_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK,
			ASTC_8x6_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK,
			ASTC_8x8_SFLOAT_BLOCK                      		= VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK,
			ASTC_10x5_SFLOAT_BLOCK                     		= VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK,
			ASTC_10x6_SFLOAT_BLOCK                     		= VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK,
			ASTC_10x8_SFLOAT_BLOCK                     		= VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK,
			ASTC_10x10_SFLOAT_BLOCK                    		= VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK,
			ASTC_12x10_SFLOAT_BLOCK                    		= VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK,
			ASTC_12x12_SFLOAT_BLOCK                    		= VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK,
			PVRTC1_2BPP_UNORM_BLOCK_IMG                		= VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
			PVRTC1_4BPP_UNORM_BLOCK_IMG                		= VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
			PVRTC2_2BPP_UNORM_BLOCK_IMG                		= VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
			PVRTC2_4BPP_UNORM_BLOCK_IMG                		= VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
			PVRTC1_2BPP_SRGB_BLOCK_IMG                 		= VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
			PVRTC1_4BPP_SRGB_BLOCK_IMG                 		= VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
			PVRTC2_2BPP_SRGB_BLOCK_IMG                 		= VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG,
			PVRTC2_4BPP_SRGB_BLOCK_IMG                 		= VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG,
			R16G16_S10_5_NV                            		= VK_FORMAT_R16G16_S10_5_NV,
		};

		struct create_info {
			int Sample;
			int Tiling;
			int Memory;
			int Usage;
			create_info();
			create_info(int aSample, int aTiling, int aMemory, int aUsage);
		};

		// Will yield the number of bits per pixel.
		static format t2f(type::id aID);
		static size_t bytesperpixel(int aFormat);
		static size_t bitsperpixel(int aFormat);
		static VkImageAspectFlags aspect_flag(int aFormat);

		// -------------------- Host Memory -------------------- //

		// Host memory images.
		image();
		image(const util::string& aFilePath);
		image(const char* aFilePath);

		image(context* aContext, create_info aCreateInfo, format aFormat, uint aX, uint aY, uint aZ, uint aT, void* aTextureData);

		// Copy Constructor.
		image(image& aInput);
		// Move Constructor.
		image(image&& aInput) noexcept;
		// Destructor
		~image();

		// Copy Assignment.
		image& operator=(image& aRhs);
		// Move Assignment.
		image& operator=(image&& aRhs) noexcept;

		// Device Operation Support: T.
		command_list copy(buffer& aSourceData, VkBufferImageCopy aRegion);
		command_list copy(buffer& aSourceData, std::vector<VkBufferImageCopy> aRegionList);
		command_list copy(image& aSourceData, VkImageCopy aRegion);
		command_list copy(image& aSourceData, std::vector<VkImageCopy> aRegionList);

		// Device Operation Support: T, G, C, D, E.
		command_list transition(
			VkAccessFlags aSrcAccessMask, VkAccessFlags aDstAccessMask,
			VkPipelineStageFlags aSrcStage, VkPipelineStageFlags aDstStage,
			VkImageLayout aNewLayout
		);
		command_list transition(
			VkAccessFlags aSrcAccessMask, VkAccessFlags aDstAccessMask,
			VkPipelineStageFlags aSrcStage, VkPipelineStageFlags aDstStage,
			uint32_t aMipLevel, uint32_t aMipLevelCount,
			uint32_t aArrayLayer, uint32_t aArrayLayerCount,
			VkImageLayout aNewLayout
		);

		// Device Operation Support: G.
		command_list generate_mipmaps(VkFilter aFilter);

		VkResult write(void* aSourceData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize);
		VkResult write(void* aSourceData, std::vector<VkBufferImageCopy> aRegionList);

		VkResult read(void* aDestinationData, size_t aSourceOffset, size_t aDestinationOffset, size_t aRegionSize);
		VkResult read(void* aDestinationData, std::vector<VkBufferImageCopy> aRegionList);

		VkAttachmentDescription description(VkAttachmentLoadOp aLoadOp, VkAttachmentStoreOp aStoreOp);
		VkAttachmentDescription description(VkAttachmentLoadOp aLoadOp, VkAttachmentStoreOp aStoreOp, VkImageLayout aFinalLayout);

		// Insure that all MipLevels and ArrayLayers have the same image layout before using a description.
		// Insure that all mip levels, and array layers, have the same layout before using a description.
		VkAttachmentDescription description(
			VkAttachmentLoadOp aLoadOp, VkAttachmentStoreOp aStoreOp,
			VkAttachmentLoadOp aStencilLoadOp, VkAttachmentStoreOp aStencilStoreOp,
			VkImageLayout aInitialLayout, VkImageLayout aFinalLayout
		);

		// Generates image views from texture instance. (YOU ARE RESPONSIBLE FOR DESTROYING VIEWS)
		VkImageView view();
		//VkImageView view(vk_image_viewType aType, VkImageSubresourceRange aRange);
		//VkImageView view(vk_image_viewType aType, VkComponentMapping aComponentMapping, VkImageSubresourceRange aRange);
		
		// Write to an image.
		VkResult write(size_t aMemorySize, void* aData);

		// Read to an image.
		VkResult read(size_t aMemorySize, void* aData);

		// Total memory size of the image. (Does not include mip levels)
		size_t get_memory_size() const;

		VkImage handle();

		void clear();

	private:

		// static bool initialize();
		// static void terminate();

		uint					Layers;
		int						Format;
		math::vec3<uint>		Resolution;

		// Device Memory
		context*				Context;
		size_t					DeviceSize;
		VkImageCreateInfo		CreateInfo;
		VkImage					Handle;
		VkMemoryAllocateInfo	AllocateInfo;
		VkDeviceMemory			MemoryHandle;
		VkImageLayout**			Layout; 
		VkExtent3D*				MipExtent;

		bool load_host_image(const char* aFilePath);

		bool create_host_image(uint32_t aArrayLayers, int aFormat, uint32_t aWidth, uint32_t aHeight, uint32_t aDepth, void* aTextureData);

		void clear_host_memory();

		// Calculates number of possible mip levels.
		uint32_t mip_level_count(VkImageType aImageType, VkExtent3D aExtent);

		// Creates Device Image.
		VkResult create_device_image(context* aContext, create_info aCreateInfo, uint aArrayLayers, int aFormat, math::vec3<uint> aResolution);
		VkResult create_device_image(context* aDeviceContext, VkImageCreateInfo aCreateInfo, int aMemoryType);
		VkResult get_limits(VkImageFormatProperties * aReturn, device* aDevice, create_info aCreateInfo, int aFormat, math::vec3<uint> aResolution);
		VkImageCreateInfo pack_create_info(VkImageFormatProperties aImageLimits, create_info aCreateInfo, uint aArrayLayers, int aFormat, math::vec3<uint> aResolution);
		VkMemoryAllocateInfo pack_allocate_info(context* aContext, device* aDevice, int aMemoryType);
		VkResult generate_miplevels(uint aMipLevel, uint aArrayLayers, VkImageLayout aInitialLayout, VkImageType aImageType, VkExtent3D aExtent);

		// Destroys device memory image.
		void clear_device_memory();

		// Clears garbage memory.
		void zero_out();

		// Used by system_window
		VkImageView create_system_window_frame(context* aContext, VkFormat aFormat, VkImageUsageFlags aUsageFlags, VkImage aImageHandle, math::vec3<uint> aImageResolution);

	};

}

#endif // !GEODESUKA_CORE_GCL_IMAGE_H
