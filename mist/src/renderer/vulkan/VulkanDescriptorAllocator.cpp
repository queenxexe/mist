#include "renderer/vulkan/VulkanDescriptorAllocator.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"

namespace mist {
	VulkanDescriptorAllocator::VulkanDescriptorAllocator() {}

	VulkanDescriptorAllocator::~VulkanDescriptorAllocator() {}

	void VulkanDescriptorAllocator::CreatePool() {
		// Initial pool sizes
		std::vector<VkDescriptorPoolSize> poolSize = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
		};

		VulkanContext& context = VulkanContext::GetContext();
		VkDescriptorPoolCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		info.poolSizeCount = static_cast<uint32_t>(poolSize.size());
		info.pPoolSizes = poolSize.data();
		info.maxSets = 100;
		info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

		VkDescriptorPool pool;
		CheckVkResult(vkCreateDescriptorPool(context.GetDevice(), &info, context.GetAllocationCallbacks(), &pool));
		pools.push_back(pool);
		
		MIST_INFO("Created new descriptor pool");
	}

	VkDescriptorSet VulkanDescriptorAllocator::Allocate(VkDescriptorSetLayout layout) {
		VulkanContext& context = VulkanContext::GetContext();

		for (VkDescriptorPool& pool : pools) {
			VkDescriptorSetAllocateInfo allocationInfo{};
			allocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocationInfo.descriptorPool = pool;
			allocationInfo.descriptorSetCount = 1;
			allocationInfo.pSetLayouts = &layout;

			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			VkResult result = vkAllocateDescriptorSets(context.GetDevice(), &allocationInfo, &descriptorSet);

			switch (result) {
			case VK_SUCCESS:
				return descriptorSet;
			case VK_ERROR_FRAGMENTED_POOL:
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				continue;
			default:
				CheckVkResult(result);
			}
		}

		CreatePool();

		VkDescriptorSetAllocateInfo allocationInfo{};
		allocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocationInfo.descriptorPool = pools.back();
		allocationInfo.descriptorSetCount = 1;
		allocationInfo.pSetLayouts = &layout;

		VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
		CheckVkResult(vkAllocateDescriptorSets(context.GetDevice(), &allocationInfo, &descriptorSet));
		return descriptorSet;
	}

	void VulkanDescriptorAllocator::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		
		for (VkDescriptorPool& pool : pools) {
			vkDestroyDescriptorPool(context.GetDevice(), pool, context.GetAllocationCallbacks());
		}
		pools.clear();
		
		MIST_INFO("Destroyed descriptor pools");
	}
}