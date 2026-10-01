#include "renderer/RenderTypes.hpp"

namespace mist {
    const char* TextureFormatToString(const TextureFormat format) {
		switch (format) {
			case TextureFormat::None:				return "None";
			case TextureFormat::RGBA8:				return "RGBA8";
			case TextureFormat::BGRA8:				return "BGRA8";
			case TextureFormat::RGB8:				return "RGB8";
			case TextureFormat::BGR8:				return "BGR8";
			case TextureFormat::RGBA16F:			return "RGBA16F";
			case TextureFormat::RGBA32F:			return "RGBA32F";
			case TextureFormat::RGB565:				return "RGB565";
			case TextureFormat::RGBA4:				return "RGBA4";
			case TextureFormat::RG8:				return "RG8";
			case TextureFormat::RG16F:				return "RG16F";
			case TextureFormat::R32F:				return "R32F";
			case TextureFormat::R11F_G11F_B10F:		return "R11F_G11F_B10F";
			case TextureFormat::RGB9_E5:			return "RGB9_E5";
			case TextureFormat::R8:					return "R8";
			case TextureFormat::SR8:				return "SR8";
			case TextureFormat::SRGB8_ALPHA8:		return "SRGB8_ALPHA8";
			case TextureFormat::SBGRA8:				return "SBGRA8";
			case TextureFormat::RGB10_A2:			return "RGB10_A2";
			case TextureFormat::R16:				return "R16";
			case TextureFormat::BC1_RGB:			return "BC1_RGB";
			case TextureFormat::BC1_RGBA:			return "BC1_RGBA";
			case TextureFormat::BC2:				return "BC2";
			case TextureFormat::BC3:				return "BC3";
			case TextureFormat::BC4:				return "BC4";
			case TextureFormat::BC5:				return "BC5";
			case TextureFormat::BC6H:				return "BC6H";
			case TextureFormat::BC7:				return "BC7";
			case TextureFormat::ETC2_RGB:			return "ETC2_RGB";
			case TextureFormat::ETC2_RGBA1:			return "ETC2_RGBA1";
			case TextureFormat::ETC2_RGBA8:			return "ETC2_RGBA8";
			case TextureFormat::EAC_R11:			return "EAC_R11";
			case TextureFormat::EAC_RG11:			return "EAC_RG11";
			case TextureFormat::ASTC_4x4:			return "ASTC_4x4";
			case TextureFormat::ASTC_5x4:			return "ASTC_5x4";
			case TextureFormat::ASTC_5x5:			return "ASTC_5x5";
			case TextureFormat::ASTC_6x5:			return "ASTC_6x5";
			case TextureFormat::ASTC_6x6:			return "ASTC_6x6";
			case TextureFormat::ASTC_8x5:			return "ASTC_8x5";
			case TextureFormat::ASTC_8x6:			return "ASTC_8x6";
			case TextureFormat::ASTC_8x8:			return "ASTC_8x8";
			case TextureFormat::ASTC_10x5:			return "ASTC_10x5";
			case TextureFormat::ASTC_10x6:			return "ASTC_10x6";
			case TextureFormat::ASTC_10x8:			return "ASTC_10x8";
			case TextureFormat::ASTC_10x10:			return "ASTC_10x10";
			case TextureFormat::ASTC_12x10:			return "ASTC_12x10";
			case TextureFormat::ASTC_12x12:			return "ASTC_12x12";
			case TextureFormat::DEPTH16:			return "DEPTH16";
			case TextureFormat::DEPTH24X8:			return "DEPTH24X8";
			case TextureFormat::DEPTH32:			return "DEPTH32";
			case TextureFormat::DEPTH16_STENCIL8:	return "DEPTH16_STENCIL8";
			case TextureFormat::DEPTH24_STENCIL8:	return "DEPTH24_STENCIL8";
			case TextureFormat::DEPTH32_STENCIL8:	return "DEPTH32_STENCIL8";
			case TextureFormat::STENCIL8:			return "STENCIL8";
		}

		return "Missing format";
	}
}