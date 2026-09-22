#pragma once
#include <vulkan/vulkan.h>

namespace mist {
    class VulkanImguiContext {
    public:
		VkDescriptorPool& GetPool();
        
        void Cleanup();
    private:
		VkDescriptorPool imguiPool = VK_NULL_HANDLE;
    };
}