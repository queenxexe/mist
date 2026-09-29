#pragma once
#include <string>
#include <unordered_map>
#include "renderer/Material.hpp"
#include "renderer/vulkan/VulkanShader.hpp"
#include "renderer/vulkan/VulkanBuffer.hpp"

namespace mist {
	struct VulkanMaterialRenderData {
		std::vector<VkDescriptorSet> descriptorSets;
		std::unordered_map<std::string, UniformBuffer> uniformBuffers;
		std::unordered_map<std::string, Ref<Image>> textures;
		bool descriptorDirty = true;
	};

	class VulkanMaterial : public Material {
	public:
		VulkanMaterial(const uint32_t materialID, const Ref<Shader>& shader);
		virtual ~VulkanMaterial() override;
	
		VulkanMaterial(const VulkanMaterial& other) = delete;
		VulkanMaterial& operator=(const VulkanMaterial& other) = delete;

		virtual void Cleanup() override;

		virtual void Bind(const uint8_t renderDataID) override;

		virtual void SetTexture(const uint8_t renderDataID, const std::string& name, const Ref<Image>& texture) override;
		virtual void SetUniformData(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) override;
		virtual void SetPushConstant(const uint8_t renderDataID, const std::string& name, size_t size, const void* value) override;
	private:
		void UpdateDescriptors(const uint8_t renderDataID);

		std::unordered_map<std::string, MaterialParameter> parameters;
		std::unordered_map<uint8_t, VulkanMaterialRenderData> materialRenderData;
	};
}