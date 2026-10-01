#pragma once
#include <Math.hpp>

namespace mist {
	enum class CullMode {
		CULL_BACK,
		CULL_FRONT,
		CULL_OFF
	};

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

	// These are default types used for reflection of shaders

	struct CameraData {
		glm::mat4 u_ViewProjectionMatrix;
	};

	// vec3 in shader is actually 16bytes rather than 12 which means padding is required
	// or it will insert the red value of color into the direction and mess with both
	struct DirectionalLightData {
		glm::vec3 u_LightDir;
		float pad1;
		glm::vec3 u_LightColor;
		float pad2;
	};
}