#include "renderer/vulkan/VulkanImguiContext.hpp"
#include <imgui_impl_vulkan.h>
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "Debug.hpp"

namespace mist {
    VkDescriptorPool& VulkanImguiContext::GetPool() {
		if (imguiPool != VK_NULL_HANDLE)
			return imguiPool;

		VulkanContext& context = VulkanContext::GetContext();
		VkDescriptorPoolSize poolSizes[] = {
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
			{ VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE }
        };
        VkDescriptorPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        info.maxSets = IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE + IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE;
        for (VkDescriptorPoolSize& poolSize : poolSizes)
            info.maxSets += poolSize.descriptorCount;
		
        info.poolSizeCount = IM_ARRAYSIZE(poolSizes);
        info.pPoolSizes = poolSizes;
        CheckVkResult(vkCreateDescriptorPool(context.GetDevice(), &info, context.GetAllocationCallbacks(), &imguiPool));
        MIST_INFO("Created imgui pool");
        return imguiPool;
	}

    void VulkanImguiContext::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

        if (imguiPool != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(context.GetDevice(), imguiPool, context.GetAllocationCallbacks());
    }
}