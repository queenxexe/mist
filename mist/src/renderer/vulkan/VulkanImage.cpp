#include "renderer/vulkan/VulkanImage.hpp"
#include <stb_image.h>
#include "data/Importer.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "Debug.hpp"
#include "VulkanContext.hpp"
#include "VulkanDebug.hpp"

namespace mist {
	VulkanImage::VulkanImage(const std::string& path, const TextureFormat desiredFormat, const TilingMode tiling) {
		data = Importer::ImportImage(path);
		data.tilingMode = tiling;
		data.format = desiredFormat;
		InitImage();
	}

	VulkanImage::~VulkanImage() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDestroyImage(context.GetDevice(), image, context.GetAllocationCallbacks());
		vkDestroyImageView(context.GetDevice(), view, context.GetAllocationCallbacks());
		vkDestroySampler(context.GetDevice(), sampler, context.GetAllocationCallbacks());
		stbi_image_free(data.pixels);
	}

	void VulkanImage::InitImage() {
		VulkanContext& context = VulkanContext::GetContext();
		VkFormat chosenFormat = VulkanHelper::GetVkFormat(data.format);
		VkSamplerAddressMode tiling = VulkanHelper::GetVkTiling(data.tilingMode);
		
		VkImageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format =  chosenFormat;
		info.extent = { (uint32_t)data.width, (uint32_t)data.height, 1 };
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		info.flags = 0;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		CheckVkResult(vkCreateImage(context.GetDevice(), &info, context.GetAllocationCallbacks(), &image));

		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(context.GetDevice(), image, &requirements);
		uint32_t memoryTypeIndex = context.FindMemoryType(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		VkMemoryAllocateInfo allocateInfo{};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = requirements.size;
		allocateInfo.memoryTypeIndex = memoryTypeIndex;
		CheckVkResult(vkAllocateMemory(context.GetDevice(), &allocateInfo, context.GetAllocationCallbacks(), &memory));
		CheckVkResult(vkBindImageMemory(context.GetDevice(), image, memory, 0));

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = chosenFormat;
		viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
		CheckVkResult(vkCreateImageView(context.GetDevice(), &viewInfo, context.GetAllocationCallbacks(), &view));

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.addressModeU = tiling;
		samplerInfo.addressModeV = tiling;
		samplerInfo.addressModeW = tiling;
		CheckVkResult(vkCreateSampler(context.GetDevice(), &samplerInfo, context.GetAllocationCallbacks(), &sampler));
	}
}