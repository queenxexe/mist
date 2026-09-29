#include "VulkanPipeline.hpp"
#include <set>
#include "renderer/vulkan/VulkanContext.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "VulkanDebug.hpp"

namespace mist {
	void VulkanPipeline::Cleanup() {
		VulkanContext& context = VulkanContext::GetContext();

		for (std::pair<const std::string, VkPipeline>& pipeline : pipelines) {
			vkDestroyPipeline(context.GetDevice(), pipeline.second, context.GetAllocationCallbacks());
		}
		pipelines.clear();
		
		for (std::pair<const std::string, VkPipelineLayout>& layout : pipelineLayouts) {
			vkDestroyPipelineLayout(context.GetDevice(), layout.second, context.GetAllocationCallbacks());
		}
		pipelineLayouts.clear();
	}

	void VulkanPipeline::CreateGraphicsPipeline(const VulkanShader& shader, const VkRenderPass& renderPass, const uint32_t colorAttachmentCount) {
		// going to have to generate all the configurations before they are used so at game launch or creating a cache file where all the shaders and variants are stored after compilation
		// Hold onto the pipeline in a unorderedmap/dictionary so the pipelines can be loaded when needed
		// read through this more https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer/

		VulkanContext& context = VulkanContext::GetContext();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VulkanHelper::GetVkCullFlagsFromCullMode(shader.GetCullMode());
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = nullptr;
		multisampling.alphaToCoverageEnable = VK_FALSE;
		multisampling.alphaToOneEnable = VK_FALSE;


		VkBool32 depthTestingEnabled = shader.IsDepthTesting() ? VK_TRUE : VK_FALSE;
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = depthTestingEnabled;
		depthStencil.depthWriteEnable = depthTestingEnabled;	// Probably should make this separate but will do for now
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		// 1 ColorBlendAttachmentState per attachment
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates(colorAttachmentCount);
		for (VkPipelineColorBlendAttachmentState& state : colorBlendAttachmentStates) {
			state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			state.blendEnable = VK_FALSE;
			state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			state.colorBlendOp = VK_BLEND_OP_ADD;
			state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			state.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = colorAttachmentCount;
		colorBlending.pAttachments = colorBlendAttachmentStates.data();
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = 2;
		dynamicState.pDynamicStates = dynamicStates;

		std::vector<VkDescriptorSetLayout> layouts = shader.GetDescriptorSetLayouts();

		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
		layoutInfo.pSetLayouts = layouts.data();

		std::vector<VkPushConstantRange> pushConstantData;
		for (const auto& res : shader.GetPushConstantResources()) {
			VkPushConstantRange range{};
			range.offset = res.second.offset;
			range.size = res.second.size;
			range.stageFlags = res.second.flags;
			pushConstantData.push_back(range);
		}
		layoutInfo.pPushConstantRanges = pushConstantData.data();
		layoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantData.size());

		VkPipelineLayout pipelineLayout;
		CheckVkResult(vkCreatePipelineLayout(context.GetDevice(), &layoutInfo, context.GetAllocationCallbacks(), &pipelineLayout));
		pipelineLayouts[shader.GetName()] = pipelineLayout;

		
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
		for (const auto& shaderStage : shader.GetShaderStages()) {
			VkPipelineShaderStageCreateInfo stageInfo{};
			stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageInfo.stage = shaderStage.stage;
			stageInfo.module = shaderStage.module;
			stageInfo.pName = "main";
			shaderStages.push_back(stageInfo);
		}

		std::vector<VkVertexInputBindingDescription> bindingDescriptions;
		std::vector<VkVertexInputAttributeDescription> attributeDescriptons;
		std::set<VkShaderStageFlagBits> setStages;
		std::set<uint32_t> setBindings;
		for (const auto& [name, res] : shader.GetInputResources()) {
			if (!(res.flags & VK_SHADER_STAGE_VERTEX_BIT))
				continue;

			VkVertexInputAttributeDescription attrib;
			attrib.binding = res.binding;
			attrib.location = res.location;
			attrib.format = res.format;
			attrib.offset = res.offset;
			attributeDescriptons.push_back(attrib);

			if (!setBindings.contains(res.binding)) {
				VkVertexInputBindingDescription binding;
				binding.binding = res.binding;
				binding.stride = res.stride;
				binding.inputRate = res.inputRate;
				bindingDescriptions.push_back(binding);
				setBindings.insert(res.binding);
			}
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)bindingDescriptions.size();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptons.size();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptons.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline graphicsPipeline;
		CheckVkResult(vkCreateGraphicsPipelines(context.GetDevice(), nullptr, 1, &pipelineInfo, context.GetAllocationCallbacks(), &graphicsPipeline));
	
		pipelines[shader.GetName()] = graphicsPipeline;
	}
}

