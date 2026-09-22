#pragma once
#include "renderer/RenderAPI.hpp"

namespace mist {
	class VulkanRenderAPI : public RenderAPI {
	public:
		// Render API overrides
		virtual void Initialize() override;
		virtual void Shutdown() override;

		virtual void WaitForIdle() override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void BeginRenderPass(const uint8_t renderDataID) override;
		virtual void EndRenderPass() override;
		virtual void UpdateDirectionalLight(const uint8_t renderDataID, const DirectionalLight& light) override;
		virtual void UpdateCamera(const uint8_t renderDataID, const Camera& camera) override;
		virtual void BindMeshRenderer(const uint8_t renderDataID, const MeshRenderer& meshRenderer) override;
		virtual void Draw(uint32_t indexCount) override;
		virtual void DrawFullscreen() override;


		virtual RenderAPI::API GetAPI() override { return RenderAPI::API::Vulkan; }
	};
}