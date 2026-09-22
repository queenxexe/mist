#pragma once
#include <cstdint>
#include "Core.hpp"

namespace mist {
	enum class TextureFormat {
		None = 0,
		// Color formats
		RGBA8,
		BGRA8,
		RGB8,
		BGR8,
		RGBA16F,
		RGBA32F,
		RGB565,
		RGBA4,
		RG8,
		RG16F,
		R32F,
		R11F_G11F_B10F,
		RGB9_E5,
		R8,
		SR8,
		SRGB8_ALPHA8,
		SBGRA8,
		RGB10_A2,
		R16,
		// Compressed color formats
		BC1_RGB,
		BC1_RGBA,
		BC2,
		BC3,
		BC4,
		BC5,
		BC6H,
		BC7,
		ETC2_RGB,
		ETC2_RGBA1,
		ETC2_RGBA8,
		EAC_R11,
		EAC_RG11,
		ASTC_4x4,
		ASTC_5x4,
		ASTC_5x5,
		ASTC_6x5,
		ASTC_6x6,
		ASTC_8x5,
		ASTC_8x6,
		ASTC_8x8,
		ASTC_10x5,
		ASTC_10x6,
		ASTC_10x8,
		ASTC_10x10,
		ASTC_12x10,
		ASTC_12x12,
		// Depth/Stencil formats
		DEPTH16,
		DEPTH24X8,
		DEPTH32,
		DEPTH16_STENCIL8,
		DEPTH24_STENCIL8,
		DEPTH32_STENCIL8,
		STENCIL8
	};

	extern "C" const char* TextureFormatToString(const TextureFormat format);
	
	enum TilingMode {
		REPEAT,
		CLAMP
	};

	struct ImageData {
		int width;
		int height;
		int channels;
		float* pixels;
		TilingMode tilingMode;
		TextureFormat format;
	};

	struct Image {
	public:
		virtual ~Image() {};
		
		ImageData GetImageData() { return data; }
		
		// While RGBA8 is good for typical images, use RGBA16F or RGBA32F for skyboxes
		// REPEAT is a good default but for skyboxes use CLAMP
		static Ref<Image> Create(const std::string& imagePath, const TextureFormat desiredFormat = TextureFormat::RGBA8, const TilingMode tiling = TilingMode::REPEAT);
	protected:
		ImageData data;
	};
}