#include "renderer/vulkan/VulkanRenderData.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Application.hpp"
#include "Debug.hpp"

namespace mist {
	VkFormat ChooseSwapchainDepthFormat(const FramebufferProperties& properties) {
		bool validFormat = false;
		VkFormat preferredFormat;
		for (uint32_t i = 0; i < properties.attachments.size(); i++) {
			if (VulkanHelper::IsDepthFormat(properties.attachments[i].textureFormat)) {
				preferredFormat = VulkanHelper::GetVkFormat(properties.attachments[i].textureFormat);
				validFormat = true;
			}
		}
		
		if (!validFormat) {
			MIST_INFO("No valid depth buffer selected, skipping depth buffer creation");
			return VK_FORMAT_UNDEFINED;
		}

		return VulkanHelper::FindSupportedDepthStencilFormat(preferredFormat, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkAttachmentDescription CreateAttachmentDescription(const FramebufferType type, const size_t index, const FramebufferTextureProperties& textureProperties) {
		VkAttachmentDescription attachment {};
		attachment.format = VulkanHelper::GetVkFormat(textureProperties.textureFormat);
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VulkanHelper::GetVkAttachmentDescriptionFinalLayout(type, index, textureProperties.textureFormat);

		return attachment;
	}

	void CreateRenderpass(const VkDevice device, const VkAllocationCallbacks* allocationCallbacks, const FramebufferProperties& properties, uint32_t& colorAttachmentCount, VkRenderPass& renderPass) {
		if (renderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(device, renderPass, allocationCallbacks);
		
		std::vector<VkAttachmentDescription> attachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		
		VkAttachmentReference depthAttachmentRef {};
		bool hasDepthAttachment = false;

		for (size_t i = 0; i < properties.attachments.size(); ++i) {
			VkAttachmentDescription attachment = CreateAttachmentDescription(properties.type, i, properties.attachments[i]);
			attachments.push_back(attachment);

			VkAttachmentReference ref {};
			ref.attachment = static_cast<uint32_t>(i);
			ref.layout = VulkanHelper::GetVkAttachmentDescriptionLayout(properties.type, properties.attachments[i].textureFormat);
			
			if (VulkanHelper::IsDepthFormat(properties.attachments[i].textureFormat)) {
				depthAttachmentRef.attachment = ref.attachment;
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				hasDepthAttachment = true;
			} else {
				ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				colorAttachmentRefs.push_back(ref);
			}
		}

		colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());

		VkSubpassDescription subpassInfo {};
		subpassInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassInfo.colorAttachmentCount = colorAttachmentCount;
		subpassInfo.pColorAttachments = colorAttachmentRefs.data();
		subpassInfo.pDepthStencilAttachment = hasDepthAttachment ? &depthAttachmentRef : nullptr;

		VkSubpassDependency dependency {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderpassInfo {};
		renderpassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderpassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderpassInfo.pAttachments = attachments.data();
		renderpassInfo.subpassCount = 1;
		renderpassInfo.pSubpasses = &subpassInfo;
		renderpassInfo.dependencyCount = 1;
		renderpassInfo.pDependencies = &dependency;

		CheckVkResult(vkCreateRenderPass(device, &renderpassInfo, allocationCallbacks, &renderPass));
	}

	void VulkanRenderData::ClearAndResizeFramebufferAttachments(const uint32_t count) {
		for (std::vector<FramebufferAttachment>& attachments : framebufferAttachments) {
			for (FramebufferAttachment& attachment : attachments) {
				attachment.Cleanup();
			}
		}
		framebufferAttachments.clear();
		framebufferAttachments.resize(count);
	}

	void VulkanRenderData::ClearAndResizeFramebuffers(const uint32_t count) {
		VulkanContext& context = VulkanContext::GetContext();
		
		for (VkFramebuffer& framebuffer : framebuffers)
			vkDestroyFramebuffer(context.GetDevice(), framebuffer, context.GetAllocationCallbacks());
		framebuffers.clear();
		framebuffers.resize(count);
	}

	void FramebufferAttachment::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		
		if (view != VK_NULL_HANDLE)
			vkDestroyImageView(context.GetDevice(), view, context.GetAllocationCallbacks());
		
		if (image != VK_NULL_HANDLE)
			vmaDestroyImage(context.GetAllocator(), image, imageAlloc);
	}
	
	VulkanRenderData::VulkanRenderData(const uint8_t ID) : RenderData(ID) {}
	
	void VulkanRenderData::Resize(const uint32_t width, const uint32_t height) {
		VulkanContext& context = VulkanContext::GetContext();
		vkDeviceWaitIdle(context.GetDevice());
		framebufferProperties.width = width;
		framebufferProperties.height = height;
		CreateRenderData(framebufferProperties);
	}

	void VulkanRenderData::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

		imguiContext.Cleanup();
		vda.Cleanup();
		pipeline.Cleanup();

		for (VkFramebuffer& framebuffer : framebuffers)
			vkDestroyFramebuffer(context.GetDevice(), framebuffer, context.GetAllocationCallbacks());

		for (std::vector<FramebufferAttachment>& attachments : framebufferAttachments)
			for (FramebufferAttachment& attachment : attachments)
				attachment.Cleanup();

		if (renderPass != VK_NULL_HANDLE)
			vkDestroyRenderPass(context.GetDevice(), renderPass, context.GetAllocationCallbacks());
	}

	VkImageLayout VulkanRenderData::GetFirstFramebufferImageLayout() {
		return VulkanHelper::GetVkAttachmentDescriptionLayout(framebufferProperties.type, framebufferProperties.attachments[0].textureFormat);
	}

	void VulkanRenderData::CreateRenderData(FramebufferProperties& properties) {
		VulkanContext& context = VulkanContext::GetContext();
		framebufferProperties = properties;
		SetViewportAndScissor(properties.width, properties.height);

		switch(properties.type) {
		case FramebufferType::SWAPCHAIN:
			CreateSwapchainFramebuffers(properties);
			break;
		case FramebufferType::SINGLE:
			CreateFramebuffers(properties);
			break;
		}
	}

	void VulkanRenderData::CreateAttachmentImage(const FramebufferProperties& properties, const TextureFormat& attachmentFormat, const size_t imageIndex, const size_t attachmentIndex) {
		VulkanContext& context = VulkanContext::GetContext();
		bool isDepthStencilFormat = VulkanHelper::IsDepthStencilFormat(attachmentFormat);
		bool isDepthFormat = VulkanHelper::IsDepthFormat(attachmentFormat);

		VkImageCreateInfo imageInfo {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = properties.width;
		imageInfo.extent.height = properties.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = VulkanHelper::GetVkFormat(attachmentFormat);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = isDepthFormat ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0;

		VmaAllocationCreateInfo imageAllocInfo {};
		imageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		framebufferAttachments[imageIndex][attachmentIndex] = {};
		CheckVkResult(vmaCreateImage(context.GetAllocator(), &imageInfo, &imageAllocInfo, &framebufferAttachments[imageIndex][attachmentIndex].image, &framebufferAttachments[imageIndex][attachmentIndex].imageAlloc, nullptr));

		VkImageViewCreateInfo imageViewInfo {};
		imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewInfo.image = framebufferAttachments[imageIndex][attachmentIndex].image;
		imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewInfo.format = VulkanHelper::GetVkFormat(attachmentFormat);
		imageViewInfo.subresourceRange.baseMipLevel = 0;
		imageViewInfo.subresourceRange.levelCount = 1;
		imageViewInfo.subresourceRange.baseArrayLayer = 0;
		imageViewInfo.subresourceRange.layerCount = 1;

		if (isDepthStencilFormat) {
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		} else if (isDepthFormat) {
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		} else {
			imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		CheckVkResult(vkCreateImageView(context.GetDevice(), &imageViewInfo, context.GetAllocationCallbacks(), &framebufferAttachments[imageIndex][attachmentIndex].view));

		framebufferAttachments[imageIndex][attachmentIndex].isDepth = isDepthFormat;
	}

	void VulkanRenderData::CreateSwapchainFramebuffers(const FramebufferProperties& properties) {
		MIST_ASSERT(properties.type == FramebufferType::SWAPCHAIN, "Trying to create offscreen framebuffer as a swapchain framebuffer");
		VulkanContext& context = VulkanContext::GetContext();

		int framebufferColorIndex;
		for (uint32_t i = 0; i < properties.attachments.size(); ++i) {
			if (VulkanHelper::IsColorFormat(properties.attachments[i].textureFormat)) {
				framebufferColorIndex = i;
				break;
			}
		}

		ClearAndResizeFramebufferAttachments(context.GetSwapchainImageCount());

		for (size_t swapchainImageIndex = 0; swapchainImageIndex < context.GetSwapchainImageCount(); ++swapchainImageIndex) {
			framebufferAttachments[swapchainImageIndex].resize(properties.attachments.size() - 1);
			size_t attachmentIndex = 0;
			for (size_t i = 0; i < properties.attachments.size(); ++i) {
				// Skip as already used for swapchain
				if (framebufferColorIndex == i)
					continue;

				CreateAttachmentImage(properties, properties.attachments[i].textureFormat, swapchainImageIndex, attachmentIndex);
				attachmentIndex++;
			}
		}

		CreateRenderpass(context.GetDevice(), context.GetAllocationCallbacks(), properties, colorAttachmentCount, renderPass);

		ClearAndResizeFramebuffers(context.GetSwapchainImageCount());
		for (size_t i = 0; i < context.GetSwapchainImageCount(); ++i) {
			std::vector<VkImageView> attachments { context.GetSwapchainImageView(i) };

			for (FramebufferAttachment& attachment : framebufferAttachments[i]) {
				attachments.push_back(attachment.view);
			}

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = properties.width;
			framebufferInfo.height = properties.height;
			framebufferInfo.layers = 1;
			
			CheckVkResult(vkCreateFramebuffer(context.GetDevice(), &framebufferInfo, context.GetAllocationCallbacks(), &framebuffers[i]));
		}
	}

	void VulkanRenderData::CreateFramebuffers(const FramebufferProperties& properties) {
		MIST_ASSERT(properties.type != FramebufferType::SWAPCHAIN, "Trying to create swapchain buffer as an offscreen framebuffer");
		VulkanContext& context = VulkanContext::GetContext();
		uint32_t framebufferCount = 1;
		
		ClearAndResizeFramebufferAttachments(framebufferCount);
		for (size_t imageIndex = 0; imageIndex < framebufferCount; ++imageIndex) {
			framebufferAttachments[imageIndex].resize(properties.attachments.size());
			size_t attachmentIndex = 0;
			for (size_t i = 0; i < properties.attachments.size(); ++i) {
				CreateAttachmentImage(properties, properties.attachments[i].textureFormat, imageIndex, attachmentIndex);
				attachmentIndex++;
			}
		}

		CreateRenderpass(context.GetDevice(), context.GetAllocationCallbacks(), properties, colorAttachmentCount, renderPass);

		ClearAndResizeFramebuffers(framebufferCount);		
		for (size_t i = 0; i < framebufferCount; ++i) {
			std::vector<VkImageView> attachments;

			for (FramebufferAttachment& attachment : framebufferAttachments[i]) {
				attachments.push_back(attachment.view);
			}

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = properties.width;
			framebufferInfo.height = properties.height;
			framebufferInfo.layers = 1;
			
			CheckVkResult(vkCreateFramebuffer(context.GetDevice(), &framebufferInfo, context.GetAllocationCallbacks(), &framebuffers[i]));
		}
	}

	void VulkanRenderData::SetViewportAndScissor(const uint32_t width, const uint32_t height) {
		viewport = {
			.x = 0,
			.y = 0,
			.width = static_cast<float>(width),
			.height = static_cast<float>(height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};

		scissor = {
			.offset = { 0, 0 },
			.extent = { width, height }
		};
	}
}