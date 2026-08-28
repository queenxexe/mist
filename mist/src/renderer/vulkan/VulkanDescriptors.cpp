#include "VulkanDescriptors.hpp"
#include "renderer/vulkan/VulkanDebug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "Debug.hpp"
#include <imgui_impl_vulkan.h>

namespace mist {
	void VulkanDescriptor::CreateDescriptorPool() {
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

	VkDescriptorSetLayout VulkanDescriptor::CreateDescriptorSetLayout(const VulkanShader* shader) {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		VulkanContext& context = VulkanContext::GetContext();

		for (const auto& res : shader->GetUboResources()) {
			uniformBufferNames[{shader->GetName(), res.second.binding}] = res.first;

			MIST_ASSERT(res.second.count > 0, "Invalid descriptor set layout count");
			MIST_ASSERT(res.second.flags != 0, "Invalid descriptor set layout flags");
			
			VkDescriptorSetLayoutBinding layoutBinding{};
			layoutBinding.binding = res.second.binding;
			layoutBinding.descriptorType = res.second.type;
			layoutBinding.descriptorCount = res.second.count;
			layoutBinding.stageFlags = res.second.flags;
			layoutBinding.pImmutableSamplers = nullptr;
			layoutBindings.push_back(layoutBinding);
		}

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		layoutInfo.pBindings = layoutBindings.data();
		
		VkDescriptorSetLayout layout;
        CheckVkResult(vkCreateDescriptorSetLayout(context.GetDevice(), &layoutInfo, context.GetAllocationCallbacks(), &layout));
		MIST_INFO(std::string("Created new descriptor set layout for: ") + shader->GetName());

		descriptorSetLayoutBindings[shader->GetName()] = std::move(layoutBindings);
		descriptorSetLayouts[shader->GetName()] = std::move(layout);
		return layout;
	}

	VkDescriptorSet& VulkanDescriptor::CreateDescriptorSet(const uint8_t frameIndex, const MeshRenderer& meshRenderer) {
		VkDescriptorSet descriptorSet;
		VulkanContext& context = VulkanContext::GetContext();

		DescriptorSetKey key{};
		key.layout = GetDescriptorSetLayout(meshRenderer.shaderName);
		key.vertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(meshRenderer.vBuffer)->GetBuffer();
		key.indexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(meshRenderer.iBuffer)->GetBuffer();

		// Try to place in existing pool
		for (size_t i = 0; i < pools.size(); ++i) {
			VkDescriptorSetAllocateInfo allocationInfo {};
			allocationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocationInfo.descriptorPool = pools[i];
			allocationInfo.descriptorSetCount = 1;
			allocationInfo.pSetLayouts = &GetDescriptorSetLayout(meshRenderer.shaderName);

			VkResult result = vkAllocateDescriptorSets(context.GetDevice(), &allocationInfo, &descriptorSet);

			switch(result) {
			case VK_SUCCESS:
				MIST_INFO("Allocated descriptor set to existing pool");
				key.poolIndex = i;
				descriptorSets[key] = descriptorSet;
				UpdateDescriptorSetsWithUniformBuffers(frameIndex, meshRenderer);
				return descriptorSets[key];
			case VK_ERROR_FRAGMENTED_POOL:
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				continue;
			default:
				CheckVkResult(result);
			};
		}

		CreateDescriptorPool();

		key.poolIndex = pools.size() - 1;
		
		VkDescriptorSetAllocateInfo alloctionInfo {};
		alloctionInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloctionInfo.descriptorPool = GetDescriptorPool((int)pools.size() - 1);
		alloctionInfo.descriptorSetCount = 1;
		alloctionInfo.pSetLayouts = &GetDescriptorSetLayout(meshRenderer.shaderName);

		CheckVkResult(vkAllocateDescriptorSets(context.GetDevice(), &alloctionInfo, &descriptorSet));
		MIST_INFO("Allocated descriptor set to new pool");

		descriptorSets[key] = descriptorSet;
		UpdateDescriptorSetsWithUniformBuffers(frameIndex, meshRenderer);
		return descriptorSets[key];
	}

	void VulkanDescriptor::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();
		for (std::pair<std::string, VkDescriptorSetLayout> layout : descriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(context.GetDevice(), layout.second, context.GetAllocationCallbacks());
		}
		descriptorSetLayouts.clear();
		
		for (auto& set : descriptorSets) {
			vkFreeDescriptorSets(context.GetDevice(), pools[set.first.poolIndex], 1, &set.second);
		}
		descriptorSets.clear();

		for (std::pair<const std::pair<uint8_t, std::string>, mist::UniformBuffer>& ubo : uniformBuffers) {
			ubo.second.Clear();
		}
		uniformBuffers.clear();
		uniformBufferNames.clear();

		for (VkDescriptorPool& pool : pools) {
			vkDestroyDescriptorPool(context.GetDevice(), pool, context.GetAllocationCallbacks());
		}
		pools.clear();

		if (imguiPool != VK_NULL_HANDLE)
			vkDestroyDescriptorPool(context.GetDevice(), imguiPool, context.GetAllocationCallbacks());

		MIST_INFO("Destroyed descriptors and uniform buffers");
	}

	VkDescriptorPool& VulkanDescriptor::GetImGuiDescriptorPool() {
		if (imguiPool != VK_NULL_HANDLE)
			return imguiPool;

		VulkanContext& context = VulkanContext::GetContext();
		VkDescriptorPoolSize poolSizes[] = {
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
        };
        VkDescriptorPoolCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        info.maxSets = 0;
        for (VkDescriptorPoolSize& poolSize : poolSizes)
            info.maxSets += poolSize.descriptorCount;
		
        info.poolSizeCount = (uint32_t)IM_COUNTOF(poolSizes);
        info.pPoolSizes = poolSizes;
        CheckVkResult(vkCreateDescriptorPool(context.GetDevice(), &info, context.GetAllocationCallbacks(), &imguiPool));
		return imguiPool;
	}

	VkDescriptorSetLayout& VulkanDescriptor::GetDescriptorSetLayout(const std::string& name) {
		MIST_ASSERT(Exists(name), "Descriptor set layout not found"); 
		return descriptorSetLayouts[name];
	}

	VkDescriptorSet& VulkanDescriptor::GetDescriptorSet(const MeshRenderer& meshRenderer) {
		DescriptorSetKey key{};
		key.layout = GetDescriptorSetLayout(meshRenderer.shaderName);
		key.vertexBuffer = std::static_pointer_cast<VulkanVertexBuffer>(meshRenderer.vBuffer)->GetBuffer();
		key.indexBuffer = std::static_pointer_cast<VulkanIndexBuffer>(meshRenderer.iBuffer)->GetBuffer();

		auto it = descriptorSets.find(key);
		if (it != descriptorSets.end())
			return it->second;

		VulkanContext& context = VulkanContext::GetContext();
		return CreateDescriptorSet(context.GetCurrentFrameIndex(), meshRenderer);
	}

	void VulkanDescriptor::UpdateDescriptorSetsWithUniformBuffers(const uint8_t frameIndex, const MeshRenderer& meshRenderer) {
		VulkanContext& context = VulkanContext::GetContext();
		for (const VkDescriptorSetLayoutBinding& binding : descriptorSetLayoutBindings[meshRenderer.shaderName]) {
			UniformBuffer& buffer = uniformBuffers[{ frameIndex, uniformBufferNames[{meshRenderer.shaderName, binding.binding}] }];
			VkDescriptorSet descriptor = GetDescriptorSet(meshRenderer);	// Get here incase we require to create the descriptor set incase we skip over due to the uniform buffer not being ready yet

			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = buffer.GetSize();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = descriptor;
			descriptorWrite.dstBinding = binding.binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(context.GetDevice(), 1, &descriptorWrite, 0, nullptr);
		}
	}
}