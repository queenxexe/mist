#include "renderer/vulkan/VulkanMaterial.hpp"
#include <map>
#include "Debug.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanImage.hpp"
#include "VulkanDebug.hpp"
#include "Application.hpp"
#include "ResourceManagerInternal.hpp"

namespace mist {
	VulkanMaterial::VulkanMaterial(const ShaderRef& shader) : Material(shader) {
		VulkanContext& context = VulkanContext::GetContext();
		ResourceManager* rm = Application::Get().GetResourceManager();
		std::shared_ptr<VulkanShader> vkShader = std::dynamic_pointer_cast<VulkanShader>(ResourceManagerInternal::GetShader(rm, shader.id));
		
		const auto& layouts = vkShader->GetDescriptorSetLayouts();
		uint8_t count = static_cast<uint8_t>(context.GetRenderDataCount());

		for (uint8_t i = 0; i < count; ++i) {
			std::shared_ptr<VulkanRenderData> renderData = context.GetRenderData(i);
			
			VulkanMaterialRenderData& data = materialRenderData[i];
			data.descriptorSets.resize(layouts.size());

			for (uint32_t set = 0; set < layouts.size(); ++set) {
				data.descriptorSets[set] = renderData->vda.Allocate(layouts[set]);
			}
		}
	}

	VulkanMaterial::~VulkanMaterial() {
		Cleanup();
	}

	void VulkanMaterial::Cleanup() {
		materialRenderData.clear();
	}

	void VulkanMaterial::Bind(const uint8_t renderDataID) {
		VulkanContext& context = VulkanContext::GetContext();
		std::shared_ptr<VulkanRenderData> vkRenderData = context.GetRenderData(renderDataID);

		VulkanMaterialRenderData& data = materialRenderData[renderDataID];
		if (data.descriptorDirty)
			UpdateDescriptors(renderDataID);

		VkCommandBuffer cmd = context.GetCurrentFrameCommandBuffer();
		std::shared_ptr<Shader> shaderData = ResourceManagerInternal::GetShader(Application::Get().GetResourceManager(), shader.id);
		VkPipelineLayout pipelineLayout = vkRenderData->pipeline.GetGraphicsPipelineLayout(shaderData->GetName());
		for (uint32_t set = 0; set < data.descriptorSets.size(); ++set) {
			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, set, 1, &data.descriptorSets[set], 0, nullptr);
		}
	}

	void VulkanMaterial::SetTexture(const uint8_t renderDataID, const std::string& name, const std::shared_ptr<Image>& texture) {
		VulkanMaterialRenderData& data = materialRenderData[renderDataID];
		data.textures[name] = texture;
		data.descriptorDirty = true;
	}

	void VulkanMaterial::SetUniformData(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) {
		VulkanMaterialRenderData& data = materialRenderData[renderDataID];
		UniformBuffer& buffer = data.uniformBuffers[name];
		if (buffer.SetData(static_cast<uint32_t>(size), value))
			data.descriptorDirty = true;
	}

	void VulkanMaterial::SetPushConstant(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) {
		std::shared_ptr<Shader> shaderData = ResourceManagerInternal::GetShader(Application::Get().GetResourceManager(), shader.id);
		shaderData->SetPushConstant(renderDataID, name, size, value);
	}

	void VulkanMaterial::UpdateDescriptors(const uint8_t renderDataID) {
		std::shared_ptr<Shader> shaderData = ResourceManagerInternal::GetShader(Application::Get().GetResourceManager(), shader.id);
		std::shared_ptr<VulkanShader> vkShader = std::dynamic_pointer_cast<VulkanShader>(shaderData);
		VulkanMaterialRenderData& data = materialRenderData[renderDataID];
		
		std::vector<VkWriteDescriptorSet> writes;
		std::vector<VkDescriptorBufferInfo> bufferInfos;
		std::vector<VkDescriptorImageInfo> textureInfos;
		writes.reserve(vkShader->GetUboResources().size());
		bufferInfos.reserve(vkShader->GetUboResources().size());
		textureInfos.reserve(vkShader->GetTextureResources().size());

		for (const auto& [name, resources] : vkShader->GetUboResources()) {
			auto it = data.uniformBuffers.find(name);
			if (it == data.uniformBuffers.end())
				continue;

			UniformBuffer& buffer = it->second;
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = buffer.GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = buffer.GetSize();
			bufferInfos.push_back(bufferInfo);

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = data.descriptorSets[resources.set];
			write.dstBinding = resources.binding;
			write.dstArrayElement = 0;
			write.descriptorType = resources.type;
			write.descriptorCount = 1;
			write.pBufferInfo = &bufferInfos.back();
			writes.push_back(write);
		}

		for (const auto& [name, resources] : vkShader->GetTextureResources()) {
			auto it = data.textures.find(name);
			if (it == data.textures.end())
				continue;

			std::shared_ptr<VulkanImage> vkImage = std::dynamic_pointer_cast<VulkanImage>(it->second);

			VkDescriptorImageInfo textureInfo{};
			textureInfo.sampler = vkImage->GetImageSampler();
			textureInfo.imageView = vkImage->GetImageView();
			textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			textureInfos.push_back(textureInfo);

			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstSet = data.descriptorSets[resources.set];
			write.dstBinding = resources.binding;
			write.dstArrayElement = 0;
			write.descriptorType = resources.type;
			write.descriptorCount = 1;
			write.pImageInfo = &textureInfos.back();
			writes.push_back(write);
		}

		VulkanContext& context = VulkanContext::GetContext();
		vkUpdateDescriptorSets(context.GetDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
		data.descriptorDirty = false;
	}
}