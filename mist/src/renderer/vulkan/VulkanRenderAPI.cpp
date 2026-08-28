#include "VulkanRenderAPI.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "data/RenderTypes.hpp"
#include "Debug.hpp"
#include "Application.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"

namespace mist {
	void VulkanRenderAPI::Initialize() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Initialize();
	}
	
	void VulkanRenderAPI::Shutdown() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Cleanup();
	}

	void VulkanRenderAPI::SetClearColor(glm::vec4& color) {
		clearColor = color;
	}
	
	void VulkanRenderAPI::BeginFrame() {
		VulkanContext& context = VulkanContext::GetContext();
		context.BeginFrame();
	}

	void VulkanRenderAPI::EndFrame() {
		VulkanContext& context = VulkanContext::GetContext();
		context.EndFrame();
	}

	void VulkanRenderAPI::BeginRenderPass(const uint8_t renderDataID) {
		VulkanContext& context = VulkanContext::GetContext();
		context.BeginRenderPass(renderDataID);
	}

	void VulkanRenderAPI::EndRenderPass() {
		VulkanContext& context = VulkanContext::GetContext();
		context.EndRenderPass();
	}

	void VulkanRenderAPI::UpdateDirectionalLight(const uint8_t renderDataID, const DirectionalLight& light) {
		DirectionalLightData lightData;
		lightData.u_LightDir = light.GetTransform().Forward();
		lightData.u_LightColor = light.lightColor;

		VulkanContext& context = VulkanContext::GetContext();
		Ref<VulkanRenderData> data = context.GetRenderData(renderDataID);
		data->descriptors.UpdateUniformBuffer({ context.GetCurrentFrameIndex(), "DirectionalLightData" }, lightData);
	}
	
	void VulkanRenderAPI::UpdateCamera(const uint8_t renderDataID, const Camera& camera) {
		CameraData camData;
		camData.u_ViewProjectionMatrix = VulkanHelper::GetFlippedViewProjectionMatrix(camera);
		
		VulkanContext& context = VulkanContext::GetContext();
		Ref<VulkanRenderData> data = context.GetRenderData(renderDataID);
		data->descriptors.UpdateUniformBuffer({ context.GetCurrentFrameIndex(), "CameraData" }, camData);
	}

	void VulkanRenderAPI::BindMeshRenderer(const uint8_t renderDataID, const MeshRenderer& meshRenderer) {
		VulkanContext& context = VulkanContext::GetContext();
		Ref<VulkanRenderData> data = context.GetRenderData(renderDataID);

		meshRenderer.vBuffer->Bind();
		meshRenderer.iBuffer->Bind();
		vkCmdBindDescriptorSets(context.GetCurrentFrameCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, data->pipeline.GetGraphicsPipelineLayout(meshRenderer.shaderName), 0, 1, &data->descriptors.GetDescriptorSet(meshRenderer), 0, nullptr);
		
		glm::mat4 matrix = meshRenderer.GetTransform().GetLocalToWorldMatrix();
		Application::Get().GetShaderLibrary()->Get(meshRenderer.shaderName)->SetUniformData(renderDataID, "ModelMatrix", sizeof(matrix), &matrix);
	}

	void VulkanRenderAPI::Draw(uint32_t indexCount) {
		VulkanContext& context = VulkanContext::GetContext();
		vkCmdDrawIndexed(context.GetCurrentFrameCommandBuffer(), indexCount, 1, 0, 0, 0);
	}
	
	void VulkanRenderAPI::WaitForIdle() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDeviceWaitIdle(context.GetDevice());
	}
}