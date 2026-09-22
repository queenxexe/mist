#pragma once
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <optional>
#include <vector>
#include <unordered_map>
#include "renderer/Framebuffer.hpp"
#include "renderer/vulkan/VulkanRenderData.hpp"

namespace mist {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool Valid() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct FrameData {
		VkSemaphore acquireImageSempahore;
		VkFence inFlightFence;

		void Cleanup();
	};

	class VulkanContext {
	public:
		static VulkanContext& GetContext() {
			static VulkanContext instance;
			return instance;
		}

		void Initialize();
        void Cleanup();

		const QueueFamilyIndices FindQueueFamilies() const;
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags flags);

		void BeginSingleTimeCommands();
		void EndSingleTimeCommands();
		
		void CreateSwapchain(const SwapchainProperties& properties);
		void RecreateSwapchain();
		void BeginFrame();
		void EndFrame();
		void BeginRenderPass(const uint8_t renderDataID);
		void EndRenderPass();

		Ref<VulkanRenderData> CreateNewRenderData();
		Ref<VulkanRenderData> GetRenderData(const uint8_t renderDataId) { return renderDatas[renderDataId]; }
		inline const size_t GetRenderDataCount() const { return renderDatas.size(); }

		inline const VkInstance GetInstance() const { return instance; }
		inline const VkSurfaceKHR GetSurface() const { return surface; }
		inline const VkDevice GetDevice() const { return device; }
		inline const VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
		inline const VkQueue GetGraphicsQueue() const { return graphicsQueue; }
		inline const VkQueue GetPresentQueue() const { return presentQueue; }
        inline const VkDebugUtilsMessengerEXT GetDebugMessenger() const { return debugMessenger; }
        inline const VmaAllocator GetAllocator() const { return allocator; }
		inline const VkCommandPool GetCommandPool() const { return commandPool; }
		inline const VkCommandBuffer GetTempCommandBuffer() const { return tempCommandBuffer; }
		inline const VkAllocationCallbacks* GetAllocationCallbacks() const { return allocationCallbacks; }
		inline const VkSwapchainKHR GetSwapchain() const { return swapchain; }
		inline const uint32_t GetCurrentFrameIndex() const { return currentFrame; }
		inline const VkCommandBuffer GetCommandBuffer(uint32_t index) const { return commandBuffers[index]; }
		inline const VkCommandBuffer GetCurrentFrameCommandBuffer() const { return commandBuffers[currentFrame]; }
		inline const uint32_t GetSwapchainImageCount() const { return static_cast<uint32_t>(swapchainImageViews.size()); }
		inline const VkImageView GetSwapchainImageView(const uint8_t index) const { return swapchainImageViews[index]; }

		const int MAX_FRAMES_IN_FLIGHT = 3;
	private:
        VulkanContext() {}
        ~VulkanContext() {}

        VulkanContext(const VulkanContext&) = delete;
        VulkanContext& operator=(const VulkanContext&) = delete;

        void CreateInstance();
        void CreateSurface();
        void CreatePhysicalDevice();
        void CreateDevice();
        void CreateAllocator();
		void CreateFrameDatas();
		void CreateCommandPool();
		void AllocateCommandBuffers();
		void CreateDescriptorPool();

		uint8_t GetNewRenderDataID();
		uint32_t currentFrame = 0;
		uint32_t imageIndex = 0;

		VkInstance instance = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		VkAllocationCallbacks* allocationCallbacks = nullptr;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VmaAllocator allocator = nullptr;
		
		SwapchainProperties swapchainProperties;
		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkSemaphore> submitSemaphores;
		std::vector<FrameData> frameDatas;

		VkCommandPool commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers;
		VkCommandBuffer tempCommandBuffer = VK_NULL_HANDLE;
		VkFence tempCommandBufferFence = VK_NULL_HANDLE;

		uint8_t renderDataCounter;
		std::unordered_map<uint8_t, Ref<VulkanRenderData>> renderDatas;
	};
}