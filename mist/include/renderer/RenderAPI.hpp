#pragma once
#include <Math.hpp>
#include "Core.hpp"
#include "renderer/Buffer.hpp"
#include "components/Camera.hpp"
#include "components/MeshRenderer.hpp"
#include "components/DirectionalLight.hpp"
#include "Framebuffer.hpp"
#include "data/Image.hpp"
#include "renderer/Material.hpp"

namespace mist {
	class RenderAPI {
	public:
		enum API { None, Vulkan };
		enum VSYNC { Off, On, TripleBuffer };

		virtual ~RenderAPI() {}

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void WaitForIdle() = 0;
		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginRenderPass(const uint8_t renderDataID) = 0;
		virtual void EndRenderPass() = 0;
		virtual void UpdateDirectionalLight(const uint8_t renderDataID, const DirectionalLight& light) = 0;
		virtual void UpdateCamera(const uint8_t renderDataID, const Camera& camera) = 0;
		virtual void BindMeshRenderer(const uint8_t renderDataID, const MeshRenderer& meshRenderer) = 0;
		virtual void Draw(uint32_t indexCount) = 0;
		virtual void DrawFullscreen() = 0;

		virtual API GetAPI() = 0;
		
		void SetVsyncMode(RenderAPI::VSYNC newMode) { vsync = newMode; }
		RenderAPI::VSYNC GetVsyncMode() { return vsync; }

		void SetClearColor(glm::vec4& color) { clearColor = color; }
		glm::vec4 GetClearColor() { return clearColor; }
	protected:
		glm::vec4 clearColor = glm::vec4(0,0,0,1);   // RGBA
		RenderAPI::VSYNC vsync = RenderAPI::VSYNC::Off;
	};
}