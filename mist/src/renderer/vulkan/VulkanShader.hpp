#pragma once
#include <vulkan/vulkan.h>
#include <glslang/Public/ShaderLang.h>
#include <spirv_cross/spirv_cross.hpp>
#include "renderer/Shader.hpp"

namespace mist {
	struct InputShaderResource {
		uint32_t binding;
		uint32_t location;
		uint32_t offset;
		uint32_t stride;
		VkFormat format;
		VkVertexInputRate inputRate;
		VkShaderStageFlags flags;
	};

	struct UBOShaderResource {
        VkDescriptorType type;
        uint32_t binding;
        uint32_t offset;
        uint32_t size;
        uint32_t count;
		uint32_t set;
        VkShaderStageFlags flags;
    };

	struct PushConstantResource {
		uint32_t offset;
		uint32_t size;
		VkShaderStageFlags flags;
	};

	struct ShaderDescriptorResource {
		VkDescriptorType type;
		uint32_t binding;
		uint32_t count;
		uint32_t set;
		VkShaderStageFlags flags;
	};

	struct VulkanShaderStage {
		EShLanguage language;
		VkShaderStageFlagBits stage;
		VkShaderModule module;
	};

	class VulkanShader : public Shader {
	public:
		VulkanShader(const std::string& path);
		VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		virtual ~VulkanShader();

		VulkanShader(const VulkanShader& other) = delete;
		VulkanShader& operator=(const VulkanShader& other) = delete;

		virtual void Cleanup() override;

		virtual void Bind(const uint8_t renderDataId) const override;
		virtual void Unbind(const uint8_t renderDataId) const override;

		virtual void SetPushConstant(const uint8_t renderDataId, const std::string& name, const int size, const void* value) override;

		virtual const std::string& GetName() const override { return shaderName; }

		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirv);
		
		const std::vector<VulkanShaderStage>& GetShaderStages() const { return shaderStages; }
		const std::unordered_map<std::string, InputShaderResource>& GetInputResources() const { return shaderInputs; }
		const std::unordered_map<std::string, UBOShaderResource>& GetUboResources() const { return shaderUbos; }
		const std::unordered_map<std::string, PushConstantResource>& GetPushConstantResources() const { return shaderPushConstants; }
		const std::unordered_map<std::string, ShaderDescriptorResource>& GetShaderDescriptorResources() const { return shaderDescriptors; }
		
		const std::vector<VkDescriptorSetLayoutBinding>& GetDescriptorSetLayoutBindings(const uint32_t set) const { return descriptorSetLayoutBindings[set]; }
		const std::vector<VkDescriptorSetLayout>& GetDescriptorSetLayouts() const { return descriptorSetLayouts; }
	private:
		std::vector<uint32_t> ConvertGLSLToSPIRV(const std::string& src, EShLanguage stage);
		std::unordered_map<EShLanguage, std::string> PreProcess(const std::string& src);
		uint32_t CalculateSize(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType& type);
		VkFormat GetDescriptionFormat(const spirv_cross::Compiler& compiler, const spirv_cross::SPIRType type);
		void Compile(const std::vector<uint32_t>& spirv, EShLanguage stage);
		void CreateDescriptorSetLayoutBinding(uint32_t set, uint32_t binding, VkDescriptorType type, uint32_t count, VkShaderStageFlags stageFlags);
		void CreateDescriptorSetLayouts();

		std::string shaderName;
		std::vector<VulkanShaderStage> shaderStages;
		std::unordered_map<std::string, InputShaderResource> shaderInputs;
		std::unordered_map<std::string, UBOShaderResource> shaderUbos;
		std::unordered_map<std::string, PushConstantResource> shaderPushConstants;
		std::unordered_map<std::string, ShaderDescriptorResource> shaderDescriptors;
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<std::vector<VkDescriptorSetLayoutBinding>> descriptorSetLayoutBindings;
	};
}