#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "Debug.hpp"
#include "Application.hpp"

namespace mist {
	void ValidateFramebufferProperties(FramebufferProperties& properties) {
		for (size_t i = 0; i < properties.attachments.size(); i++) {
			FramebufferTextureProperties& textureProperties = properties.attachments[i];
			if (VulkanHelper::IsColorFormatSupported(VulkanHelper::GetVkFormat(textureProperties.textureFormat))) {
				continue;
			}
			if (VulkanHelper::IsDepthStencilFormatSupported(VulkanHelper::GetVkFormat(textureProperties.textureFormat))) {
				continue;
			}
	
			// If format is not supported, choose a fallback format
			if (VulkanHelper::IsDepthStencilFormat(textureProperties.textureFormat)) {
				textureProperties.textureFormat = VulkanHelper::GetTextureFormat(VulkanHelper::FindSupportedDepthStencilFormat(VulkanHelper::GetVkFormat(textureProperties.textureFormat), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT));
			} else {
				textureProperties.textureFormat = VulkanHelper::GetTextureFormat(VulkanHelper::FindSupportedColorFormat(VulkanHelper::GetVkFormat(textureProperties.textureFormat), VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT));
			}
	
			MIST_WARN(std::string("Format not supported. Using a fallback format: ") + TextureFormatToString(textureProperties.textureFormat));
		}
	}

	std::shared_ptr<RenderData> RenderData::Create(FramebufferProperties& properties) {
		switch (Application::Get().GetRenderAPI()->GetAPI()) {
		case RenderAPI::API::Vulkan:
		{
			ValidateFramebufferProperties(properties);
			VulkanContext& context = VulkanContext::GetContext();
			std::shared_ptr<VulkanRenderData> data = context.CreateNewRenderData();
			
			if (properties.type == FramebufferType::SWAPCHAIN) {
				MIST_ASSERT(VulkanHelper::IsColorFormat(properties.attachments[0].textureFormat), "First framebuffer attachment is not a color format, cant create swapchain");
				context.CreateSwapchain(data->GetRenderDataID(), properties);
			}

			data->CreateRenderData(properties);
			// Since window may have a different initial size on load this makes sure the window is the size that was requested
			Application::Get().GetWindow()->SetSize(properties.width, properties.height);

			return std::static_pointer_cast<RenderData>(data);
		}
		case RenderAPI::API::None:
			MIST_ASSERT(false, "Running headless");
			return nullptr;
		}
	}
}