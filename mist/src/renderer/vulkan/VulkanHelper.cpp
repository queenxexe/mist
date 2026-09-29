#include "renderer/vulkan/VulkanHelper.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
	VkFormat VulkanHelper::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags features) {
		VulkanContext& context = VulkanContext::GetContext();
		
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), originalFormat, &props);

		// Checks if original was valid
		if ((tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) || (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)) {
			return originalFormat;
		}

		// Tries alternative formats
		for (VkFormat format : candidates) {
			VkFormatProperties properties;
			vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), format, &properties);

			if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
				return format;
			} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		MIST_ERROR("Failed to find supported format or alternative format");
		return VK_FORMAT_UNDEFINED;
	}

	VkFormat VulkanHelper::FindSupportedColorFormat(VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags formatFlags) {
		return FindSupportedFormat({ 
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_FORMAT_B8G8R8A8_UNORM,
			VK_FORMAT_R8G8B8_UNORM,
			VK_FORMAT_B8G8R8_UNORM,
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_FORMAT_R32G32B32A32_SFLOAT,
			VK_FORMAT_R5G6B5_UNORM_PACK16,
			VK_FORMAT_R4G4B4A4_UNORM_PACK16,
			VK_FORMAT_R8G8_UNORM,
			VK_FORMAT_R16G16_SFLOAT,
			VK_FORMAT_R32_SFLOAT,
			VK_FORMAT_B10G11R11_UFLOAT_PACK32,
			VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
			VK_FORMAT_R8_UNORM,
			VK_FORMAT_R8_SRGB,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FORMAT_B8G8R8A8_SRGB,
			VK_FORMAT_A2B10G10R10_UNORM_PACK32,
			VK_FORMAT_R16_UNORM
		}, originalFormat, tiling, formatFlags);
	}

	VkFormat VulkanHelper::FindSupportedDepthStencilFormat(VkFormat originalFormat, VkImageTiling tiling, VkFormatFeatureFlags formatFlags) {
		return FindSupportedFormat({ 
			VK_FORMAT_D16_UNORM,
			VK_FORMAT_X8_D24_UNORM_PACK32,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D32_SFLOAT_S8_UINT
		}, originalFormat, tiling, formatFlags);
	}

	bool VulkanHelper::IsColorFormatSupported(const VkFormat& format) {
		VkFormatProperties props;
		VulkanContext& context = VulkanContext::GetContext();
		vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), format, &props);

		return (props.optimalTilingFeatures & VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT) != 0;
	}

	bool VulkanHelper::IsDepthStencilFormatSupported(const VkFormat& format) {
		VkFormatProperties props;
		VulkanContext& context = VulkanContext::GetContext();
		vkGetPhysicalDeviceFormatProperties(context.GetPhysicalDevice(), format, &props);

		return (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0;
	}

	bool VulkanHelper::IsDepthStencilFormat(const VkFormat& format) {
		switch (format) {
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default:
			return false;
		}
	}

	bool VulkanHelper::IsDepthStencilFormat(const TextureFormat& format) {
		switch (format) {
			case TextureFormat::DEPTH16_STENCIL8:
			case TextureFormat::DEPTH24_STENCIL8:
			case TextureFormat::DEPTH32_STENCIL8:
				return true;
			default:
				return false;
		}
	};
	
	bool VulkanHelper::IsDepthFormat(const VkFormat& format) {
		switch (format) {
		case VK_FORMAT_D16_UNORM:
		case VK_FORMAT_X8_D24_UNORM_PACK32:
		case VK_FORMAT_D32_SFLOAT:
		case VK_FORMAT_D16_UNORM_S8_UINT:
		case VK_FORMAT_D24_UNORM_S8_UINT:
		case VK_FORMAT_D32_SFLOAT_S8_UINT:
			return true;
		default:
			return false;
		}
	}

	bool VulkanHelper::IsColorFormat(const TextureFormat& format) {
		switch (format) {
			case TextureFormat::RGBA8:
			case TextureFormat::BGRA8:
			case TextureFormat::RGB8:
			case TextureFormat::BGR8:
			case TextureFormat::RGBA16F:
			case TextureFormat::RGBA32F:
			case TextureFormat::RGB565:
			case TextureFormat::RGBA4:
			case TextureFormat::RG8:
			case TextureFormat::RG16F:
			case TextureFormat::R32F:
			case TextureFormat::R11F_G11F_B10F:
			case TextureFormat::RGB9_E5:
			case TextureFormat::R8:
			case TextureFormat::SR8:
			case TextureFormat::SRGB8_ALPHA8:
			case TextureFormat::SBGRA8:
			case TextureFormat::RGB10_A2:
			case TextureFormat::R16:
				return true;
			default:
				return false;
		}
	}

	bool VulkanHelper::IsDepthFormat(const TextureFormat& format) {
		switch (format) {
			case TextureFormat::DEPTH16:
			case TextureFormat::DEPTH24X8:
			case TextureFormat::DEPTH32:
			case TextureFormat::DEPTH16_STENCIL8:
			case TextureFormat::DEPTH24_STENCIL8:
			case TextureFormat::DEPTH32_STENCIL8:
				return true;
			default:
				return false;
		}
	};

	VkImageLayout VulkanHelper::GetVkAttachmentDescriptionLayout(const FramebufferType type, const TextureFormat& format) {
		if (VulkanHelper::IsDepthFormat(format)) {
			if (type == FramebufferType::SWAPCHAIN)	
				return VulkanHelper::IsDepthStencilFormat(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			
			return VulkanHelper::IsDepthStencilFormat(format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
		}

		if (type == FramebufferType::SWAPCHAIN)
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VkImageLayout VulkanHelper::GetVkAttachmentDescriptionFinalLayout(const FramebufferType type, const size_t attachmentIndex, const TextureFormat& format) {
		if (VulkanHelper::IsDepthFormat(format)) {
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		if (type == FramebufferType::SWAPCHAIN && attachmentIndex == 0)
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	VkFormat VulkanHelper::GetVkFormat(const TextureFormat& format) {
		switch (format) {
		// Color formats
		case TextureFormat::RGBA8:				return VK_FORMAT_R8G8B8A8_UNORM;
		case TextureFormat::BGRA8:				return VK_FORMAT_B8G8R8A8_UNORM;
		case TextureFormat::RGB8:				return VK_FORMAT_R8G8B8_UNORM;
		case TextureFormat::BGR8:				return VK_FORMAT_B8G8R8_UNORM;
		case TextureFormat::RGBA16F:			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case TextureFormat::RGBA32F:			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case TextureFormat::RGB565:				return VK_FORMAT_R5G6B5_UNORM_PACK16;
		case TextureFormat::RGBA4:				return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
		case TextureFormat::RG8:				return VK_FORMAT_R8G8_UNORM;
		case TextureFormat::RG16F:				return VK_FORMAT_R16G16_SFLOAT;
		case TextureFormat::R32F:				return VK_FORMAT_R32_SFLOAT;
		case TextureFormat::R11F_G11F_B10F:		return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case TextureFormat::RGB9_E5:			return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		case TextureFormat::R8:					return VK_FORMAT_R8_UNORM;
		case TextureFormat::SR8:				return VK_FORMAT_R8_SRGB;
		case TextureFormat::SRGB8_ALPHA8:		return VK_FORMAT_R8G8B8A8_SRGB;
		case TextureFormat::SBGRA8:				return VK_FORMAT_B8G8R8A8_SRGB;
		case TextureFormat::RGB10_A2:			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case TextureFormat::R16:				return VK_FORMAT_R16_UNORM;
		// Compressed color formats
		case TextureFormat::BC1_RGB:			return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
		case TextureFormat::BC1_RGBA:			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case TextureFormat::BC2:				return VK_FORMAT_BC2_UNORM_BLOCK;
		case TextureFormat::BC3:				return VK_FORMAT_BC3_UNORM_BLOCK;
		case TextureFormat::BC4:				return VK_FORMAT_BC4_UNORM_BLOCK;
		case TextureFormat::BC5:				return VK_FORMAT_BC5_UNORM_BLOCK;
		case TextureFormat::BC6H:				return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case TextureFormat::BC7:				return VK_FORMAT_BC7_UNORM_BLOCK;
		case TextureFormat::ETC2_RGB:			return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
		case TextureFormat::ETC2_RGBA1:			return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
		case TextureFormat::ETC2_RGBA8:			return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
		case TextureFormat::EAC_R11:			return VK_FORMAT_EAC_R11_UNORM_BLOCK;
		case TextureFormat::EAC_RG11:			return VK_FORMAT_EAC_R11G11_UNORM_BLOCK;
		case TextureFormat::ASTC_4x4:			return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
		case TextureFormat::ASTC_5x4:			return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
		case TextureFormat::ASTC_5x5:			return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
		case TextureFormat::ASTC_6x5:			return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
		case TextureFormat::ASTC_6x6:			return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
		case TextureFormat::ASTC_8x5:			return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
		case TextureFormat::ASTC_8x6:			return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
		case TextureFormat::ASTC_8x8:			return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
		case TextureFormat::ASTC_10x5:			return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
		case TextureFormat::ASTC_10x6:			return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
		case TextureFormat::ASTC_10x8:			return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
		case TextureFormat::ASTC_10x10:			return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
		case TextureFormat::ASTC_12x10:			return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
		case TextureFormat::ASTC_12x12:			return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
		// Depth/Stencil formats
		case TextureFormat::DEPTH16:			return VK_FORMAT_D16_UNORM;
		case TextureFormat::DEPTH24X8:			return VK_FORMAT_X8_D24_UNORM_PACK32;
		case TextureFormat::DEPTH32:			return VK_FORMAT_D32_SFLOAT;
		case TextureFormat::DEPTH16_STENCIL8:	return VK_FORMAT_D16_UNORM_S8_UINT;
		case TextureFormat::DEPTH24_STENCIL8:	return VK_FORMAT_D24_UNORM_S8_UINT;
		case TextureFormat::DEPTH32_STENCIL8:	return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case TextureFormat::STENCIL8:			return VK_FORMAT_S8_UINT;
		default:								return VK_FORMAT_UNDEFINED;
		}
	}

	TextureFormat VulkanHelper::GetTextureFormat(const VkFormat& format) {
		switch (format) {
		// Color formats
		case VK_FORMAT_R8G8B8A8_UNORM:					return TextureFormat::RGBA8;
		case VK_FORMAT_B8G8R8A8_UNORM:					return TextureFormat::BGRA8;
		case VK_FORMAT_R8G8B8_UNORM:					return TextureFormat::RGB8;
		case VK_FORMAT_B8G8R8_UNORM:					return TextureFormat::BGR8;
		case VK_FORMAT_R16G16B16A16_SFLOAT:				return TextureFormat::RGBA16F;
		case VK_FORMAT_R32G32B32A32_SFLOAT:				return TextureFormat::RGBA32F;
		case VK_FORMAT_R5G6B5_UNORM_PACK16:				return TextureFormat::RGB565;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16:			return TextureFormat::RGBA4;
		case VK_FORMAT_R8G8_UNORM:						return TextureFormat::RG8;
		case VK_FORMAT_R16G16_SFLOAT:					return TextureFormat::RG16F;
		case VK_FORMAT_R32_SFLOAT:						return TextureFormat::R32F;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32:			return TextureFormat::R11F_G11F_B10F;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:			return TextureFormat::RGB9_E5;
		case VK_FORMAT_R8_UNORM:						return TextureFormat::R8;
		case VK_FORMAT_R8_SRGB:							return TextureFormat::SR8;
		case VK_FORMAT_R8G8B8A8_SRGB:					return TextureFormat::SRGB8_ALPHA8;
		case VK_FORMAT_B8G8R8A8_SRGB:					return TextureFormat::SBGRA8;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32:		return TextureFormat::RGB10_A2;
		case VK_FORMAT_R16_UNORM:						return TextureFormat::R16;
		// Compressed color formats
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK:				return TextureFormat::BC1_RGB;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:			return TextureFormat::BC1_RGBA;
		case VK_FORMAT_BC2_UNORM_BLOCK:					return TextureFormat::BC2;
		case VK_FORMAT_BC3_UNORM_BLOCK:					return TextureFormat::BC3;
		case VK_FORMAT_BC4_UNORM_BLOCK:					return TextureFormat::BC4;
		case VK_FORMAT_BC5_UNORM_BLOCK:					return TextureFormat::BC5;
		case VK_FORMAT_BC6H_UFLOAT_BLOCK:				return TextureFormat::BC6H;
		case VK_FORMAT_BC7_UNORM_BLOCK:					return TextureFormat::BC7;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:			return TextureFormat::ETC2_RGB;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:		return TextureFormat::ETC2_RGBA1;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:		return TextureFormat::ETC2_RGBA8;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK:				return TextureFormat::EAC_R11;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:			return TextureFormat::EAC_RG11;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:			return TextureFormat::ASTC_4x4;
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:			return TextureFormat::ASTC_5x4;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:			return TextureFormat::ASTC_5x5;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:			return TextureFormat::ASTC_6x5;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:			return TextureFormat::ASTC_6x6;
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:			return TextureFormat::ASTC_8x5;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:			return TextureFormat::ASTC_8x6;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:			return TextureFormat::ASTC_8x8;
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:			return TextureFormat::ASTC_10x5;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:			return TextureFormat::ASTC_10x6;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:			return TextureFormat::ASTC_10x8;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:			return TextureFormat::ASTC_10x10;
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:			return TextureFormat::ASTC_12x10;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:			return TextureFormat::ASTC_12x12;
		// Depth/Stencil formats
		case VK_FORMAT_D16_UNORM:						return TextureFormat::DEPTH16;
		case VK_FORMAT_X8_D24_UNORM_PACK32:				return TextureFormat::DEPTH24X8;
		case VK_FORMAT_D32_SFLOAT:						return TextureFormat::DEPTH32;
		case VK_FORMAT_D16_UNORM_S8_UINT:				return TextureFormat::DEPTH16_STENCIL8;
		case VK_FORMAT_D24_UNORM_S8_UINT:				return TextureFormat::DEPTH24_STENCIL8;
		case VK_FORMAT_D32_SFLOAT_S8_UINT:				return TextureFormat::DEPTH32_STENCIL8;
		case VK_FORMAT_S8_UINT:							return TextureFormat::STENCIL8;
		default:										return TextureFormat::None;
		}
	}

	VkSamplerAddressMode VulkanHelper::GetVkTiling(const TilingMode& tiling) {
		switch (tiling) {
		case TilingMode::REPEAT:	return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case TilingMode::CLAMP:		return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		default:					return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}
	}

	VkPresentModeKHR VulkanHelper::GetPresentMode(RenderAPI::VSYNC mode) {
		switch (mode) {
		case RenderAPI::VSYNC::Off:				return VK_PRESENT_MODE_IMMEDIATE_KHR;
		case RenderAPI::VSYNC::On:				return VK_PRESENT_MODE_FIFO_KHR;
		case RenderAPI::VSYNC::TripleBuffer:	return VK_PRESENT_MODE_MAILBOX_KHR;
		}
	}

	glm::mat4 VulkanHelper::GetFlippedProjectionMatrix(const glm::mat4 projectionMatrix) {
		glm::mat4 flipped = projectionMatrix;
		flipped[1][1] *= -1;
		return flipped;
	}

	glm::mat4 VulkanHelper::GetFlippedViewProjectionMatrix(const mist::Camera& camera) {
		glm::mat4 flipped = GetFlippedProjectionMatrix(camera.GetProjectionMatrix());
		return flipped * camera.GetViewMatrix();
	}

	// Gets the byte size for each format
	// Note: Compressed color formats are not included as would require specific handling
	uint32_t VulkanHelper::GetByteSizeFromFormat(const TextureFormat& format) {
		switch (format) {
		// Color formats
		case TextureFormat::RGBA8:				return 4;
		case TextureFormat::BGRA8:				return 4;
		case TextureFormat::RGB8:				return 3;
		case TextureFormat::BGR8:				return 3;
		case TextureFormat::RGBA16F:			return 8;
		case TextureFormat::RGBA32F:			return 16;
		case TextureFormat::RGB565:				return 2;
		case TextureFormat::RGBA4:				return 2;
		case TextureFormat::RG8:				return 2;
		case TextureFormat::RG16F:				return 4;
		case TextureFormat::R32F:				return 4;
		case TextureFormat::R11F_G11F_B10F:		return 4;
		case TextureFormat::RGB9_E5:			return 4;
		case TextureFormat::R8:					return 1;
		case TextureFormat::SR8:				return 1;
		case TextureFormat::SRGB8_ALPHA8:		return 4;
		case TextureFormat::SBGRA8:				return 4;
		case TextureFormat::RGB10_A2:			return 4;
		case TextureFormat::R16:				return 2;
		// Depth/Stencil formats
		case TextureFormat::DEPTH16:			return 2;
		case TextureFormat::DEPTH24X8:			return 4;
		case TextureFormat::DEPTH32:			return 4;
		case TextureFormat::DEPTH16_STENCIL8:	return 3;
		case TextureFormat::DEPTH24_STENCIL8:	return 4;
		case TextureFormat::DEPTH32_STENCIL8:	return 8;
		case TextureFormat::STENCIL8:			return 1;
		default:								
			MIST_WARN("Unknown format or attempted to passed a compressed format. Will default to 4");
			return 4;	
		}
	}

	VkCullModeFlags VulkanHelper::GetVkCullFlagsFromCullMode(const CullMode& mode) {
		switch (mode) {
			case CullMode::CULL_BACK: return VK_CULL_MODE_BACK_BIT;
			case CullMode::CULL_FRONT: return VK_CULL_MODE_FRONT_BIT;
			case CullMode::CULL_OFF: return VK_CULL_MODE_NONE;
		}
	}
}