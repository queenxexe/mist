#pragma once
#include <Math.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <components/Camera.hpp>
#include <imgui/ImguiLayer.hpp>
#include <entt/entt.hpp>
#include <data/RefTypes.hpp>

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

		inline const std::shared_ptr<mist::RenderData>& GetRenderData() { return renderData; }
		inline const ImTextureID GetFramebufferID() const { return framebufferID; }
	private:
	mist::ImguiLayer* parent;
	
		float xRotation = 0;
		float yRotation = 0;

		std::shared_ptr<mist::RenderData> renderData;
		ImTextureID framebufferID;
		entt::entity cameraEntity;
		mist::ImageRef skyboxImage;
		mist::ShaderRef skyboxShader;
		mist::MaterialRef skyboxMat;
	};
}