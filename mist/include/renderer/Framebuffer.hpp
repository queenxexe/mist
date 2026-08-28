#pragma once
#include "Core.hpp"
#include <vector>
#include <cstdint>

namespace mist {
	enum class FramebufferTextureFormat {
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

	enum class FramebufferType {
		SWAPCHAIN,	// Will generate a double or triple buffered framebuffers with the swapchain images
		SINGLE		// Single framebuffer
	};

	struct FramebufferTextureProperties {
		FramebufferTextureProperties() = default;
		FramebufferTextureProperties(const FramebufferTextureFormat& format) : textureFormat(format) {}

		FramebufferTextureFormat textureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferProperties {
		FramebufferType type;
		uint32_t width = 1, height = 1;
		std::vector<FramebufferTextureProperties> attachments;
		uint32_t samples = 1;
	};

	struct SwapchainProperties {
		uint32_t width = 1, height = 1;
		FramebufferTextureFormat colorFormat;
	};

	extern "C" const char* FramebufferTextureFormatToString(const FramebufferTextureFormat format);

	class RenderData {
	public:
		RenderData(const uint8_t ID) : ID(ID) {}
		virtual ~RenderData() {}

		virtual void Resize(const uint32_t width, const uint32_t height) = 0;
		
		inline const FramebufferProperties& GetProperties() const { return framebufferProperties; }
		inline const uint8_t GetRenderDataID() { return ID; }

		static Ref<RenderData> Create(FramebufferProperties& properties);
	protected:
		uint8_t ID;
		FramebufferProperties framebufferProperties;
	};
}