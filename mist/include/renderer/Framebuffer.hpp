#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <stdint.h>
#include "renderer/RenderTypes.hpp"

#define INVALID_RENDER_DATA_ID UINT8_MAX

namespace mist {
	enum class FramebufferType {
		SWAPCHAIN,	// Will generate a double or triple buffered framebuffers with the swapchain images
		SINGLE		// Single framebuffer
	};

	struct FramebufferTextureProperties {
		FramebufferTextureProperties() = default;
		FramebufferTextureProperties(const TextureFormat& format) : textureFormat(format) {}

		TextureFormat textureFormat = TextureFormat::None;
	};

	struct FramebufferProperties {
		FramebufferType type;
		uint32_t width = 1, height = 1;
		std::vector<FramebufferTextureProperties> attachments;
		uint32_t samples = 1;
	};

	class RenderData {
	public:
		RenderData(const uint8_t ID) : ID(ID) {}
		virtual ~RenderData() {}

		virtual void Resize(const uint32_t width, const uint32_t height) = 0;
		
		inline const FramebufferProperties& GetProperties() const { return framebufferProperties; }
		inline const uint8_t GetRenderDataID() { return ID; }

		static std::shared_ptr<RenderData> Create(FramebufferProperties& properties);
	protected:
		uint8_t ID;
		FramebufferProperties framebufferProperties;
	};
}