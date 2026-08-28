#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "renderer/Framebuffer.hpp"
#include "renderer/RenderAPI.hpp"

namespace mist {
	class VulkanHelper {
	public:
		static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags features);
		static VkFormat FindSupportedColorFormat(VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags formatFlags);
		static VkFormat FindSupportedDepthStencilFormat(VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags formatFlags);
		static bool IsColorFormatSupported(const VkFormat& format);
		static bool IsDepthStencilFormatSupported(const VkFormat& format);
		static bool IsDepthStencilFormat(const FramebufferTextureFormat& format);
		static bool IsDepthStencilFormat(const VkFormat& format);
		static bool IsDepthFormat(const FramebufferTextureFormat& format);
		static bool IsDepthFormat(const VkFormat& format);
		static bool IsColorFormat(const FramebufferTextureFormat& format);
		static VkImageLayout GetVkAttachmentDescriptionLayout(const FramebufferType type, const FramebufferTextureFormat& format);
		static VkImageLayout GetVkAttachmentDescriptionFinalLayout(const FramebufferType type, const size_t attachmentIndex, const FramebufferTextureFormat& format);
		static VkFormat GetVkFormat(const FramebufferTextureFormat& format);
		static FramebufferTextureFormat GetFramebufferTextureFormat(const VkFormat& format);
		static VkPresentModeKHR GetPresentMode(RenderAPI::VSYNC mode);
		static glm::mat4 GetFlippedProjectionMatrix(glm::mat4 projectionMatrix);
		static glm::mat4 GetFlippedViewProjectionMatrix(const mist::Camera& camera);
	};
}