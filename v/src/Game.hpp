#pragma once
#include <Math.hpp>
#include <Core.hpp>
#include <renderer/Framebuffer.hpp>
#include <renderer/Buffer.hpp>
#include <renderer/Shader.hpp>
#include <components/Camera.hpp>
#include <imgui/ImguiLayer.hpp>
#include <entt/entt.hpp>

namespace V {
	class Game {
	public:
		Game(mist::ImguiLayer* layer);

		void Initialize();
		void OnUpdate();
		void OnImguiRender();
		void OnRender();
		void PostRender();
		void Cleanup();
	private:
		mist::ImguiLayer* parent;
		mist::Ref<mist::RenderData> renderData;
		entt::entity cameraEntity;
	};
}