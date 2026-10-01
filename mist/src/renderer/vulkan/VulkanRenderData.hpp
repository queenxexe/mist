#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanPipeline.hpp"
#include "renderer/vulkan/VulkanDescriptorAllocator.hpp"
#include "renderer/vulkan/VulkanImguiContext.hpp"

// TODO: Add additional FramebufferType of PING PONG which would allow for creation of two sets of framebuffers that will be switched between

namespace mist {
	struct FramebufferAttachment {
		VkImage image;
		VkImageView view;
		VmaAllocation imageAlloc;
		bool isDepth;

		void Cleanup();
	};

	class VulkanRenderData : public RenderData {
	public:
		VulkanRenderData(const uint8_t ID);
		~VulkanRenderData() {}

		virtual void Resize(const uint32_t width, const uint32_t height) override;

		void CreateRenderData(FramebufferProperties& properties);
		void Cleanup();	// This is for complete destruction
		void Reset();	// This is for framebuffer recreation

		inline VkImageView GetFirstFramebufferImageView() { return framebufferAttachments[0][0].view; }
		VkImageLayout GetFirstFramebufferImageLayout();
		
		VulkanPipeline pipeline;
		VulkanDescriptorAllocator vda;
		VulkanImguiContext imguiContext;
		
		VkRenderPass renderPass = VK_NULL_HANDLE;
		VkViewport viewport;
		VkRect2D scissor;
		uint32_t colorAttachmentCount = 0;
		std::vector<std::vector<FramebufferAttachment>> framebufferAttachments;
		std::vector<VkFramebuffer> framebuffers;
	private:
		void CreateSwapchainFramebuffers(const FramebufferProperties& properties);
		void CreateFramebuffers(const FramebufferProperties& properties);
		void ClearAndResizeFramebufferAttachments(const uint32_t count);
		void ClearAndResizeFramebuffers(const uint32_t count);
		void CreateAttachmentImage(const FramebufferProperties& properties, const TextureFormat& attachmentFormat, const size_t imageIndex, const size_t attachmentIndex);
		void SetViewportAndScissor(const uint32_t width, const uint32_t height);
	};
}