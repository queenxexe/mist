#include "renderer/vulkan/VulkanImage.hpp"
#include <stb_image.h>
#include "data/Importer.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"
#include "Debug.hpp"
#include "VulkanContext.hpp"
#include "VulkanDebug.hpp"

namespace mist {
	VulkanImage::VulkanImage(const std::string& path, const TextureFormat desiredFormat, const TilingMode tiling) {
		data = Importer::ImportImage(path);
		data.tilingMode = tiling;
		data.format = desiredFormat;
		InitImage();
		Upload();
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
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		CheckVkResult(vkCreateSampler(context.GetDevice(), &samplerInfo, context.GetAllocationCallbacks(), &sampler));
	}

	void VulkanImage::TransitionLayout(const VkCommandBuffer& cmd, const VkImageLayout& newLayout) {
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = layout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;
		if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			MIST_ERROR("Unsupported image layout transition");
			return;
		}

		vkCmdPipelineBarrier(cmd, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		layout = newLayout;
	}

    void VulkanImage::Upload() {
		VulkanContext& context = VulkanContext::GetContext();
		context.BeginSingleTimeCommands();
		VkCommandBuffer cmd = context.GetTempCommandBuffer();

		VkDeviceSize size = static_cast<VkDeviceSize>(data.width) * static_cast<VkDeviceSize>(data.height) * VulkanHelper::GetByteSizeFromFormat(data.format);
		VulkanBuffer staging;
		VmaAllocationInfo info {};
		staging.Create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, VMA_ALLOCATION_CREATE_MAPPED_BIT, info);
		memcpy(info.pMappedData, data.pixels, size);
		vmaFlushAllocation(context.GetAllocator(), staging.alloc, 0, size);

		TransitionLayout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { static_cast<uint32_t>(data.width), static_cast<uint32_t>(data.height), 1 };
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		vkCmdCopyBufferToImage(cmd, staging.buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
		TransitionLayout(cmd, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		context.EndSingleTimeCommands();
		staging.Clear();
	}
}