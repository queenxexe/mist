#pragma once
#include "data/Image.hpp"
#include <vulkan/vulkan.h>

namespace mist {
    class VulkanImage : public Image {
    public:
        VulkanImage(const std::string& imagePath, const TextureFormat format, const TilingMode tiling);
        ~VulkanImage();

        VulkanImage(VulkanImage&& other) noexcept;
        VulkanImage& operator=(VulkanImage&& other) noexcept;
    
        void InitImage();
    private:
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
        VkSampler sampler;
    };
}