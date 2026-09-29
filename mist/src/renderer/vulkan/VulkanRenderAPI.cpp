#include "VulkanRenderAPI.hpp"
#include "renderer/vulkan/VulkanContext.hpp"
#include "data/RenderTypes.hpp"
#include "Debug.hpp"
#include "Application.hpp"
#include "renderer/vulkan/VulkanHelper.hpp"
#include "renderer/vulkan/VulkanMaterial.hpp"

namespace mist {
	void VulkanRenderAPI::Initialize() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Initialize();
	}
	
	void VulkanRenderAPI::Shutdown() {
		VulkanContext& context = VulkanContext::GetContext();
		context.Cleanup();
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

	// Im aware this isnt great in the long run and all shaders should share one but this will do for now
	void VulkanRenderAPI::UpdateDirectionalLight(const uint8_t renderDataID, const DirectionalLight& light) {
		DirectionalLightData lightData;
		lightData.u_LightDir = light.GetTransform().Forward();
		lightData.u_LightColor = light.lightColor;

		auto& materials = Application::Get().GetMaterialLibrary()->GetAllMaterials();
		for (const auto&[id, material] : materials) {
			material->SetUniformData(renderDataID, "DirectionalLightData", sizeof(lightData), &lightData);
		}
	}
	
	// Im aware this isnt great in the long run and all shaders should share one but this will do for now
	void VulkanRenderAPI::UpdateCamera(const uint8_t renderDataID, const Camera& camera) {
		CameraData camData;
		camData.u_ViewProjectionMatrix = VulkanHelper::GetFlippedViewProjectionMatrix(camera);
		
		auto& materials = Application::Get().GetMaterialLibrary()->GetAllMaterials();
		for (const auto&[id, material] : materials) {
			material->SetUniformData(renderDataID, "CameraData", sizeof(camData), &camData);
		}
	}

	void VulkanRenderAPI::BindMeshRenderer(const uint8_t renderDataID, const MeshRenderer& meshRenderer) {
		VulkanContext& context = VulkanContext::GetContext();
		Ref<VulkanRenderData> data = context.GetRenderData(renderDataID);

		meshRenderer.vBuffer->Bind();
		meshRenderer.iBuffer->Bind();
	}

	void VulkanRenderAPI::Draw(uint32_t indexCount) {
		VulkanContext& context = VulkanContext::GetContext();
		vkCmdDrawIndexed(context.GetCurrentFrameCommandBuffer(), indexCount, 1, 0, 0, 0);
	}

	void VulkanRenderAPI::DrawFullscreen() {
		VulkanContext& context = VulkanContext::GetContext();
		vkCmdDraw(context.GetCurrentFrameCommandBuffer(), 3, 1, 0, 0);
	}
	
	void VulkanRenderAPI::WaitForIdle() {
		VulkanContext& context = VulkanContext::GetContext();
		vkDeviceWaitIdle(context.GetDevice());
	}
}