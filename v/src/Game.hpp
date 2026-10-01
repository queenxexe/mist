#pragma once
#include <Math.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <renderer/Shader.hpp>
#include <renderer/Material.hpp>
#include <components/Camera.hpp>
#include <imgui/ImguiLayer.hpp>
#include <entt/entt.hpp>

namespace V {
	class Game {
	public:
		Game(mist::ImguiLayer* layer);

		void Initialize();
		void OnUpdate();
		void OnRender();
		void OnImguiRender();
		void Cleanup();
		void Resize(const uint32_t& x, const uint32_t& y);

		inline const mist::Ref<mist::RenderData>& GetRenderData() { return renderData; }
		inline const ImTextureID GetFramebufferID() const { return framebufferID; }
	private:
		mist::ImguiLayer* parent;
		mist::Ref<mist::RenderData> renderData;
		ImTextureID framebufferID;
		entt::entity cameraEntity;
		mist::Ref<mist::Image> skyboxImage;
		mist::Ref<mist::Shader> skyboxShader;
		mist::Ref<mist::Material> skyboxMat;
	};
}