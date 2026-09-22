#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace mist {
	class VulkanDescriptorAllocator {
	public:
		VulkanDescriptorAllocator();
		~VulkanDescriptorAllocator();

		VulkanDescriptorAllocator(const VulkanDescriptorAllocator&) = delete;
		VulkanDescriptorAllocator& operator=(const VulkanDescriptorAllocator) = delete;

		VkDescriptorSet Allocate(VkDescriptorSetLayout layout);
		void Cleanup();
	private:
		void CreatePool();
	
		std::vector<VkDescriptorPool> pools;
	};
}