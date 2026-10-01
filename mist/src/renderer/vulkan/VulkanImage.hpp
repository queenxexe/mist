#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include "data/Image.hpp"

namespace mist {
    class VulkanImage : public Image {
    public:
        VulkanImage(const std::string& imagePath, const TextureFormat format, const TilingMode tiling);
        virtual ~VulkanImage() override;
        
        virtual void Cleanup() override;

        VulkanImage(VulkanImage&& other) noexcept;
        VulkanImage& operator=(VulkanImage&& other) noexcept;
    
        void InitImage();
        void TransitionLayout(const VkCommandBuffer& cmd, const VkImageLayout& newLayout);
        void Upload();
        
        inline const VkImage& GetImage() const { return image; }
        inline const VkImageView& GetImageView() const { return view; }
        inline const VkSampler& GetImageSampler() const { return sampler; }
    private:
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };
}